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
        if (!parent) return;
        ei_widget_t tmp = parent->children_head;

        // Si le widget est déjà en dernier
        if (!widget->next_sibling) {
                modify_hierarchy(widget->parent, widget->parent->parent);
                return;
        }
        // Si le parent du widget est un toplevel alors on ne modifie pas sa hierarchie (elle sera toujours la meme)
        if (strcmp(parent->wclass->name,"toplevel\0") == 0){
            modify_hierarchy(widget->parent, widget->parent->parent);
            return;
        }
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
        modify_hierarchy(widget->parent, widget->parent->parent);
}

//============================= toplevel

bool toplevel_move;
bool resize;
ei_impl_frame_t* frame;
ei_point_t mouse_point;
ei_impl_toplevel_t* current_moving_toplevel = NULL;


bool ei_callback_toplevel(ei_widget_t	widget, struct ei_event_t*	event, ei_user_param_t	user_param) {

        ei_point_t cur_point = event->param.mouse.where;
        if (event->type == ei_ev_mouse_buttonup && toplevel_move) {
            toplevel_move = 0;
            current_moving_toplevel = NULL;
            return 1;
        }

        if (toplevel_move && event->type == ei_ev_mouse_move && current_moving_toplevel) {
            int x = cur_point.x - mouse_point.x;
            int y = cur_point.y - mouse_point.y;

            ei_rect_t clip = current_moving_toplevel->widget.screen_location;
            ei_rect_t rect = current_moving_toplevel->widget.screen_location;

            current_moving_toplevel->widget.screen_location.top_left.x += x;
            current_moving_toplevel->widget.screen_location.top_left.y += y;
            if (current_moving_toplevel->widget.screen_location.top_left.y < 0)
                current_moving_toplevel->widget.screen_location.top_left.y = 0;
            mouse_point = cur_point;


            clip.size.width+=abs(x);
            clip.size.height+=abs(y);
            if (current_moving_toplevel->widget.screen_location.top_left.x < clip.top_left.x )
                clip.top_left.x = current_moving_toplevel->widget.screen_location.top_left.x < 0 ? 0 : current_moving_toplevel->widget.screen_location.top_left.x ;
            if (current_moving_toplevel->widget.screen_location.top_left.y < clip.top_left.y )
                clip.top_left.y = current_moving_toplevel->widget.screen_location.top_left.y < 0 ? 0 : current_moving_toplevel->widget.screen_location.top_left.y ;

            ei_widget_t child = current_moving_toplevel->widget.children_head;
            while (child){
                if (child->geom_params)(child->geom_params->manager->runfunc)(child);
                child=child->next_sibling;
            }
            ei_app_invalidate_rect(&clip);
            return 1;
        }


        if (!widget) return false;


        if (strcmp(widget->wclass->name, (ei_widgetclass_name_t) {"toplevel\0"}) == 0) {

                ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) widget;
                ei_rect_t rect = toplevel->widget.screen_location;

                if (toplevel_move == 0 && event->type == ei_ev_mouse_buttondown) {
                        if (rect.top_left.y <= cur_point.y && cur_point.y <=
                                                              rect.top_left.y + k_default_button_corner_radius * 2 +
                                                              toplevel->border_width) {
                                toplevel_move = 1;
                                current_moving_toplevel = toplevel;
                                mouse_point = event->param.mouse.where;
                                modify_hierarchy(widget, toplevel->widget.parent);
                                ei_app_invalidate_rect(&ei_app_root_widget()->screen_location);
                                return 1;
                        }

                }


        }
        return false;

}

bool ei_resize_toplevel(ei_widget_t	widget, struct ei_event_t*	event, ei_user_param_t	user_param){

        if (event->type == ei_ev_mouse_buttonup && resize) {
                resize= 0;
                return 1;
        }
        ei_point_t cur_point = event->param.mouse.where;

        if (widget && resize==0 && event->type==ei_ev_mouse_buttondown && strcmp(widget->wclass->name, (ei_widgetclass_name_t) {"frame\0"})==0){
                ei_impl_toplevel_t* t =(ei_impl_toplevel_t*)widget->parent;
                if (strcmp(widget->parent->wclass->name, (ei_widgetclass_name_t) {"toplevel\0"})==0 && widget->pick_id==((ei_impl_toplevel_t*)widget->parent)->frame->pick_id){
                        resize=1;
                        mouse_point = event->param.mouse.where;
                        frame = (ei_impl_frame_t *)widget;
                        return 1;
                }
        }

        if (resize && event->type==ei_ev_mouse_move) {
                ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) frame->widget.parent;
                if (toplevel->resizable_axis==ei_axis_none)
                    return true;
                ei_rect_t rect = toplevel->widget.screen_location;
                rect.size.width++;
                rect.size.height++;

                int x = 0;
                int y = 0;

                if (toplevel->resizable_axis==ei_axis_both || toplevel->resizable_axis==ei_axis_x){
                        x = cur_point.x - mouse_point.x;
                }

                if (toplevel->resizable_axis==ei_axis_both || toplevel->resizable_axis==ei_axis_y){
                        y = cur_point.y - mouse_point.y;
                }

                x +=toplevel->widget.requested_size.width;
                y +=toplevel->widget.requested_size.height;

                if (toplevel->minimal_size.width>x) x=toplevel->minimal_size.width;
                if (toplevel->minimal_size.height>y) y=toplevel->minimal_size.height;

                toplevel->widget.screen_location.size.width = x;
                toplevel->widget.screen_location.size.height = y;

                toplevel->widget.requested_size.width = x;
                toplevel->widget.requested_size.height = y;

                ei_impl_frame_t *contain_frame = (ei_impl_frame_t *) toplevel->contain_frame;
                contain_frame->widget.screen_location.size.width = x- 2*(toplevel->border_width);
                contain_frame->widget.screen_location.size.height = y- 2*(toplevel->border_width)-k_default_button_corner_radius*2;

                contain_frame->widget.requested_size.width = x- 2*(toplevel->border_width);
                contain_frame->widget.requested_size.height = y- 2*(toplevel->border_width)-k_default_button_corner_radius*2;

                mouse_point = cur_point;

                ei_widget_t child = frame->widget.parent->children_head;
                while (child){
                        (child->geom_params->manager->runfunc)(child);
                        child=child->next_sibling;
                }

                child = contain_frame->widget.children_head;
                while (child){
                        (child->geom_params->manager->runfunc)(child);
                        child=child->next_sibling;
                }
                ei_app_invalidate_rect(&rect);

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
        if (current_moving_toplevel && toplevel->pick_id==current_moving_toplevel->widget.pick_id) {
            current_moving_toplevel = NULL;
            toplevel_move = false;
        }
        if (resize && frame->widget.pick_id == ((ei_impl_toplevel_t*)toplevel)->frame->pick_id){
            frame = NULL;
            resize= false;
        }
        supr_hierachy(toplevel->parent, toplevel);
        free((ei_impl_toplevel_t*)toplevel);
}

bool toplevel_close(ei_widget_t	widget,
                    ei_event_t*	event,
                    ei_user_param_t user_param){
        ei_widget_destroy(widget->parent);
        ei_app_invalidate_rect(&widget->parent->screen_location);
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

        toplevel->color = ei_default_background_color;

        int border =4;
        toplevel->border_width = border;
        toplevel->title="Toplevel";
        toplevel->can_close = true;
        toplevel->resizable_axis = ei_axis_both;
        toplevel->minimal_size = (ei_size_t){160, 120};



        ei_widget_t button = ei_widget_create	("button", (ei_widget_t)(toplevel), NULL, NULL);
        ei_button_configure		(button, &(ei_size_t){12, 12},
                                            &(ei_color_t){235, 20, 20, 255},
                                            &(int){1}, &(int){5},
                                            &(ei_relief_t){ei_relief_raised},
                                            NULL, NULL,
                                            NULL, NULL, NULL, NULL, NULL,
                                            &(ei_callback_t){toplevel_close}, NULL);

        toplevel->button = button;

        ei_place(toplevel->button, &(ei_anchor_t){ei_anc_northwest},
                 &(int){toplevel->border_width + 3}, &(int){toplevel->border_width+3}, NULL,
                 NULL, &(float){0.0}, &(float){0.0}, NULL, NULL);

        ei_color_t color  = toplevel->color;
        ei_color_t dark_color  = (ei_color_t){abs(color.red -50), abs(color.green -50), abs(color.blue -50), 50};

        ei_widget_t resize_frame = ei_widget_create	("frame", (ei_widget_t)(toplevel), NULL, NULL);
        ei_frame_configure		(resize_frame, &(ei_size_t){10, 10},
                                            &dark_color,
                                            &(int){0}, NULL,NULL,
                                            NULL, NULL,
                                            NULL, NULL, NULL, NULL);

        ei_bind(ei_ev_mouse_buttondown, resize_frame,NULL,ei_resize_toplevel,NULL);
        ei_bind(ei_ev_mouse_move, resize_frame,NULL,ei_resize_toplevel,NULL);
        ei_bind(ei_ev_mouse_buttonup, resize_frame,NULL,ei_resize_toplevel,NULL);

        toplevel->frame = resize_frame;

        ei_place(toplevel->frame, &(ei_anchor_t){ei_anc_southeast},
                 NULL, NULL, NULL, NULL, &(float){1.0}, &(float){1.0}, NULL, NULL);

        ei_widget_t frame = ei_widget_create	("frame", (ei_widget_t)(toplevel), NULL, NULL);
        ei_frame_configure		(frame, &(ei_size_t){toplevel->widget.requested_size.width-2*(border),
                                                               toplevel->widget.requested_size.height-2*(border)}-k_default_button_corner_radius*2,
                                           &color,
                                           &(int){0}, NULL,NULL,
                                           NULL, NULL,
                                           NULL, NULL, NULL, NULL);

        ei_place(frame, &(ei_anchor_t){ei_anc_southeast},
                 &(int){-border}, &(int){-border}, NULL, NULL, &(float){1}, &(float){1}, NULL, NULL);

        toplevel->contain_frame = frame;
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

        ei_color_t color  = ei_font_default_color;
        ei_color_t dark_color  = (ei_color_t){abs(color.red -100), abs(color.green -100) , abs(color.blue -100) , 210};

        int border = toplevel->border_width;

        int radius = k_default_button_corner_radius;
        ei_point_t rounded_frame_temp[40];
        ei_point_t rounded_frame[22];

        give_rounded_frame(rounded_frame_temp, rect, radius);

        for (int i=0; i<20;i++){
                rounded_frame[i] = rounded_frame_temp[i];
        }

        rounded_frame[20]=(ei_point_t){rect.top_left.x,rect.top_left.y + radius*2+border};
        rounded_frame[21]=(ei_point_t){rect.top_left.x+rect.size.width,rect.top_left.y + radius*2+border};



        ei_point_t square_frame[8] = {{rect.top_left.x , rect.top_left.y + 2*radius + border + 1},
                                      {rect.top_left.x , rect.top_left.y  + rect.size.height},
                                      {rect.top_left.x + rect.size.width, rect.top_left.y + rect.size.height},
                                      {rect.top_left.x + rect.size.width, rect.top_left.y + 2*radius + border +1 },
                                      {rect.top_left.x - border + rect.size.width, rect.top_left.y + 2*radius + border +1 },
                                      {rect.top_left.x - border + rect.size.width, rect.top_left.y - border + rect.size.height},
                                      {rect.top_left.x + border-1, rect.top_left.y - border + rect.size.height},
                                      {rect.top_left.x + border-1, rect.top_left.y + 2*radius + border + 1},
        };


        ei_draw_polygon(surface,square_frame,8, dark_color, &new_clipper);
        ei_draw_polygon(surface,rounded_frame,22, dark_color, &new_clipper);
        ei_draw_polygon(pick_surface,rounded_frame_temp,40,*(widget->pick_color),&new_clipper);


        ei_color_t white_color = (ei_color_t){255,255,255, 255};

        if (toplevel->title) {
                uint32_t size = 10;
                if (toplevel->button) size = toplevel->button->requested_size.width;
                ei_point_t where = (ei_point_t){ border + rect.top_left.x + 2*size, rect.top_left.y -1 };
                ei_font_t font = hw_text_font_create(ei_default_font_filename, ei_style_normal, 18);
                ei_draw_text(surface, &where, toplevel->title,
                             font, white_color,
                             &new_clipper);
                hw_text_font_free(font);
        }
        hw_surface_lock(surface);
}

void ei_impl_geomnotify_toplevel(ei_widget_t widget){

}
