//
// Created by montelr on 5/14/24.
//

#include "ei_implementation.h"
#include "ei_draw.h"
#include "ei_placer.h"
#include "ei_event.h"
#include "var.h"
#include "hw_interface.h"
#include "ei_types.h"
#include "ei_implementation.h"
#include "ei_widget_configure.h"
//============================= toplevel

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
        free((ei_impl_toplevel_t*)toplevel);
}

/**
* \brief Fonction pour mettre les valeurs par defauts d'un widget toplevel
*/
bool toplevel_close(ei_widget_t	widget,
                    ei_event_t*	event,
                    ei_user_param_t user_param){

        ei_impl_toplevel_t* toplevel = (ei_impl_toplevel_t*)user_param;

        ei_impl_release_button(widget);
        ei_impl_release_toplevel(toplevel);

        return true;
}

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



        ei_widget_t button = ei_widget_create	("button", widget, NULL, NULL);
        ei_button_configure		(button, &(ei_size_t){15, 15},
                                            &(ei_color_t){235, 20, 20, 255},
                                            &(int){2}, &k_default_button_corner_radius,
                                            &(ei_relief_t){ei_relief_raised},
                                            NULL, NULL,
                                            NULL, NULL, NULL, NULL, NULL,
                                            &(ei_callback_t){toplevel_close}, toplevel);

        ei_bind(ei_ev_mouse_buttondown, button,NULL,ei_callback_clickbutton,toplevel);
        ei_bind(ei_ev_mouse_buttonup,button,NULL,ei_callback_clickbutton,toplevel);

        toplevel->button = button;
}

/**
* \brief Fonction pour dessiner un widget toplevel.
*
*/
void ei_impl_draw_toplevel(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper){

        hw_surface_unlock(surface);
        ei_impl_toplevel_t* toplevel = (ei_impl_toplevel_t*)widget;

        ei_rect_t rect= widget->screen_location;

        ei_color_t color  = *toplevel->color;
        ei_color_t dark_color  = (ei_color_t){color.red -50, color.green -50, color.blue -50, color.alpha};

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


        ei_draw_polygon(surface,square_frame,4, dark_color, clipper);
        ei_draw_polygon(surface,rounded_frame,21, dark_color, clipper);
        ei_draw_polygon(surface,little_square_frame,4, color, clipper);

        //ei_draw_polygon(pick_surface,rounded_frame_temp,40,*(widget->pick_color),clipper);
        ei_color_t white_color = (ei_color_t){255,255,255, 255};

        if (toplevel->title) {

                ei_point_t where = (ei_point_t){ border + rect.top_left.x + 1.5*toplevel->button->requested_size.width, rect.top_left.y};
                ei_font_t font = hw_text_font_create(ei_default_font_filename, ei_style_normal, 18);
                ei_draw_text(surface, &where, toplevel->title,
                             font, white_color,
                             &widget->screen_location);
                hw_text_font_free(font);
        }

        ei_app_invalidate_rect(&widget->screen_location);

        hw_surface_lock(surface);
}

bool ei_callback_toplevel(ei_widget_t		widget, struct ei_event_t*	event, ei_user_param_t	user_param){

}