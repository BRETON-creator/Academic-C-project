//
// Created by montelr on 5/14/24.
//

#include "ei_implementation.h"
#include "ei_draw.h"
#include "ei_event.h"
#include "var.h"
#include "hw_interface.h"
#include "ei_types.h"
#include "ei_widget_configure.h"
#include "ei_impl_placer.h"





/**
* \brief Fonction pour modifier la hiérarchie des widgets pour que le widget appelé écrase les autres fils de son parent : pour cela on le met à la fin
* de la liste des enfants.
*/

void modify_hierarchy( ei_widget_t widget , ei_widget_t parent)
{
        ei_widget_t tmp = parent->children_head;

        // Si le widget est déjà en dernier
        if (!widget->next_sibling)
                return;

        // Trouver le widget dans la liste des enfants du parent
        ei_widget_t prev = NULL;
        ei_widget_t cur = parent->children_head;

        while (cur != NULL) {
                if (cur == widget)
                        break;
                prev = cur;
                cur = cur->next_sibling;
        }
        if (!cur)
                return;
        if (prev)
                prev->next_sibling = cur->next_sibling;
        else
                parent->children_head = cur->next_sibling;

        cur->next_sibling = NULL;

        // Trouver le dernier enfant et attacher le widget à sa suite
        ei_widget_t last_child = parent->children_head;
        while (last_child->next_sibling)
                last_child = last_child->next_sibling;

        last_child->next_sibling = cur;
}

//============================= toplevel

bool toplevel_move;
bool resize;
ei_impl_frame_t* frame;
ei_point_t mouse_point;


bool ei_callback_toplevel(ei_widget_t	widget, struct ei_event_t*	event, ei_user_param_t	user_param) {

        if (event->type == ei_ev_mouse_buttonup && toplevel_move) {
                toplevel_move = 0;
                return 1;
        }

        if (!widget) return false;

        ei_point_t cur_point = event->param.mouse.where;

        if (strcmp(widget->wclass->name, (ei_widgetclass_name_t) {"toplevel\0"}) == 0) {

                ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) widget;
                ei_rect_t rect = toplevel->widget.screen_location;

                if (toplevel_move == 0 && event->type == ei_ev_mouse_buttondown) {
                        if (rect.top_left.y <= cur_point.y && cur_point.y <=
                                                              rect.top_left.y + k_default_button_corner_radius * 2 +
                                                              *toplevel->border_width) {
                                toplevel_move = 1;
                                mouse_point = event->param.mouse.where;
                                modify_hierarchy(toplevel, toplevel->widget.parent);
                                return 1;
                        }

                }

                if (toplevel_move && event->type == ei_ev_mouse_move) {
                        int x = cur_point.x - mouse_point.x;
                        int y = cur_point.y - mouse_point.y;

                        toplevel->widget.screen_location.top_left.x += x;
                        toplevel->widget.screen_location.top_left.y += y;
                        mouse_point = cur_point;

                        rect.size.width++;
                        rect.size.height++;

                        ei_widget_t child = toplevel->widget.children_head;
                        while (child){
                                (child->geom_params->manager->runfunc)(child);
                                child=child->next_sibling;
                        }
                        ei_impl_widget_draw_children(ei_app_root_widget(), ei_app_root_surface(), pick_surface, &rect);

                        return 1;
                }
        }
}

bool ei_resize_toplevel(ei_widget_t	widget, struct ei_event_t*	event, ei_user_param_t	user_param){

        if (event->type == ei_ev_mouse_buttonup) {
                resize= 0;
                return 1;
        }
        ei_point_t cur_point = event->param.mouse.where;

        if (widget && resize==0 && event->type==ei_ev_mouse_buttondown && strcmp(widget->wclass->name, (ei_widgetclass_name_t) {"frame\0"})==0){
                resize=1;
                mouse_point = event->param.mouse.where;
                frame = (ei_impl_frame_t *)widget;
                return 1;
        }

        if (resize && event->type==ei_ev_mouse_move) {
                //TODO gérer les contraintes
                int x = cur_point.x - mouse_point.x;
                int y = cur_point.y - mouse_point.y;

                ei_rect_t rect = frame->widget.parent->screen_location;
                rect.size.width++;
                rect.size.height++;

                frame->widget.parent->screen_location.size.width += x;
                frame->widget.parent->screen_location.size.height += y;

                frame->widget.parent->requested_size.width += x;
                frame->widget.parent->requested_size.height += y;

                mouse_point = cur_point;

                ei_widget_t child = frame->widget.parent->children_head;
                while (child){
                        (child->geom_params->manager->runfunc)(child);
                        child=child->next_sibling;
                }
                ei_impl_widget_draw_children(ei_app_root_widget(), ei_app_root_surface(), pick_surface, &rect);

                return 1;
        }
        return 0;
}


/**
 *  \brief fonction pour alloué un espace pour un widget toplevel.
 *
 * @return un espace assez grand pour stocker un widget de classe toplevel.
 */
ei_widget_t ei_impl_alloc_toplevel(){
        return calloc(1,sizeof(ei_impl_toplevel_t));
}

/**
 * \brief Fonction pour free un espace alloué a un widget toplevel.
 *
 */

void ei_impl_release_toplevel(ei_widget_t toplevel){
        supr_hierachy(ei_app_root_widget(), toplevel);
        free((ei_impl_toplevel_t*)toplevel);
}

bool toplevel_close(ei_widget_t	widget,
                    ei_event_t*	event,
                    ei_user_param_t user_param){
        widget->parent->geom_params->manager = NULL;
        ei_impl_widget_draw_children(ei_app_root_widget(),ei_app_root_surface(),pick_surface,&widget->parent->screen_location);
        ei_impl_release_toplevel(widget->parent);
        return true;
}

/**
* \brief Fonction pour mettre les valeurs par defauts d'un widget toplevel
*/
void ei_impl_setdefaults_toplevel(ei_widget_t widget){
        ei_impl_toplevel_t* toplevel = (ei_impl_toplevel_t*)widget;

        toplevel->widget.wclass =ei_widgetclass_from_name((ei_const_string_t){"toplevel\0"});
        toplevel->widget.user_data = NULL;
        toplevel->widget.destructor = NULL;
        /* Widget Hierachy Management */
        toplevel->widget.parent = ei_app_root_widget();		///< Pointer to the parent of this widget.
        toplevel->widget.children_head=NULL;	///< Pointer to the first child of this widget.	Children are chained with the "next_sibling" field.
        toplevel->widget.children_tail=NULL;	///< Pointer to the last child of this widget.
        toplevel->widget.next_sibling=NULL;	///< Pointer to the next child of this widget's parent widget.

        /* Geometry Management */
        toplevel->widget.geom_params = (ei_geom_param_t){NULL};	///< Pointer to the geometry management parameters for this widget. If NULL, the widget is not currently managed and thus, is not displayed on the screen.
        toplevel->widget.requested_size=(ei_size_t){320,240} ;	///< See \ref ei_widget_get_requested_size.
        toplevel->widget.screen_location=(ei_rect_t){(ei_point_t){0,0},(ei_size_t){320,240}};///< See \ref ei_widget_get_screen_location.

        toplevel->color = &ei_default_background_color;
        int* border = calloc(1, sizeof(int));
        *border = 4;
        toplevel->border_width = border;
        toplevel->title="Toplevel";
        toplevel->can_close = true;
        toplevel->resizable_axis = ei_axis_both;
        toplevel->minimal_size = (ei_size_t){160, 120};



        ei_widget_t button = ei_widget_create	("button", toplevel, NULL, NULL);
        ei_button_configure		(button, &(ei_size_t){12, 12},
                                            &(ei_color_t){235, 20, 20, 255},
                                            &(int){1}, &(int){5},
                                            &(ei_relief_t){ei_relief_raised},
                                            NULL, NULL,
                                            NULL, NULL, NULL, NULL, NULL,
                                            &(ei_callback_t){toplevel_close}, NULL);

        toplevel->button = button;

        ei_color_t color  = *toplevel->color;
        ei_color_t dark_color  = (ei_color_t){color.red -50, color.green -50, color.blue -50, color.alpha};

        ei_widget_t resize_frame = ei_widget_create	("frame", toplevel, NULL, NULL);
        ei_frame_configure		(resize_frame, &(ei_size_t){10, 10},
                                            &dark_color,
                                            &(int){0}, NULL,NULL,
                                            NULL, NULL,
                                            NULL, NULL, NULL, NULL);

        ei_bind(ei_ev_mouse_buttondown, resize_frame,NULL,ei_resize_toplevel,NULL);
        ei_bind(ei_ev_mouse_move, resize_frame,NULL,ei_resize_toplevel,NULL);
        ei_bind(ei_ev_mouse_buttonup, resize_frame,NULL,ei_resize_toplevel,NULL);

        toplevel->frame = resize_frame;
}

/**
* \brief Fonction pour dessiner un widget toplevel.
*
*/
void ei_impl_draw_toplevel(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper){

        hw_surface_unlock(surface);
        ei_impl_toplevel_t* toplevel = (ei_impl_toplevel_t*)widget;
        ei_rect_t new_clipper = get_rect_intersection(*clipper, widget->screen_location);
        ei_rect_t rect= widget->screen_location;

        ei_color_t color  = *toplevel->color;
        ei_color_t dark_color  = (ei_color_t){abs(color.red -50), abs(color.green -50) , abs(color.blue -50) , color.alpha};

        int border = *toplevel->border_width;

        int radius = k_default_button_corner_radius;
        ei_point_t rounded_frame_temp[40];
        ei_point_t rounded_frame[21];

        give_rounded_frame(rounded_frame_temp, rect, radius);

        for (int i=0; i<21;i++){
                rounded_frame[i] = rounded_frame_temp[i];
        }

        ei_point_t square_frame[4] = {{rect.top_left.x , rect.top_left.y + radius},
                                      {rect.top_left.x + rect.size.width , rect.top_left.y + radius},
                                      {rect.top_left.x + rect.size.width , rect.top_left.y + rect.size.height },
                                      {rect.top_left.x, rect.top_left.y + rect.size.height}};

        ei_point_t little_square_frame[4] = {{rect.top_left.x + border , rect.top_left.y + 2*radius + border},
                                             {rect.top_left.x + rect.size.width - border, rect.top_left.y + 2*radius + border},
                                             {rect.top_left.x + rect.size.width - border, rect.top_left.y + rect.size.height - border},
                                             {rect.top_left.x + border, rect.top_left.y + rect.size.height - border}};


        ei_draw_polygon(surface,square_frame,4, dark_color, &new_clipper);
        ei_draw_polygon(surface,rounded_frame_temp,40, dark_color, &new_clipper);
        ei_draw_polygon(surface,little_square_frame,4, color, &new_clipper);
        ei_draw_polygon(pick_surface,rounded_frame,21,*(widget->pick_color),&new_clipper);


        ei_color_t white_color = (ei_color_t){255,255,255, 255};

        if (toplevel->title) {

                ei_point_t where = (ei_point_t){ border + rect.top_left.x + 2*toplevel->button->requested_size.width, rect.top_left.y + border};
                ei_font_t font = hw_text_font_create(ei_default_font_filename, ei_style_normal, 18);
                ei_draw_text(surface, &where, toplevel->title,
                             font, white_color,
                             clipper);
                hw_text_font_free(font);
        }

        ei_app_invalidate_rect(&widget->screen_location);

        hw_surface_lock(surface);
}

