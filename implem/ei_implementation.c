/**
 * @file	ei_implementation.h
 *
 * @brief 	Private definitions.
 * 
 */


#include "ei_implementation.h"
#include "ei_draw.h"



/**
 * @brief	Draws the children of a widget.
 * 		The children are draw withing the limits of the clipper and
 * 		the widget's content_rect.
 *
 * @param	widget		The widget which children are drawn.
 * @param	surface		A locked surface where to draw the widget's children.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void		ei_impl_widget_draw_children	(ei_widget_t		widget,
						 ei_surface_t		surface,
						 ei_surface_t		pick_surface,
						 ei_rect_t*		clipper);



/**
 * \brief	Converts the red, green, blue and alpha components of a color into a 32 bits integer
 * 		than can be written directly in the memory returned by \ref hw_surface_get_buffer.
 * 		The surface parameter provides the channel order.
 *
 * @param	surface		The surface where to store this pixel, provides the channels order.
 * @param	color		The color to convert.
 *
 * @return 			The 32 bit integer corresponding to the color. The alpha component
 *				of the color is ignored in the case of surfaces that don't have an
 *				alpha channel.
 */
uint32_t	ei_impl_map_rgba(ei_surface_t surface, ei_color_t color);


//================================================================================================
/**
 *  \brief fonction pour alloué un espace pour un widget frame.
 *
 * @return un espace assez grand pour stocker un widget de classe frame.
 */
ei_widget_t ei_impl_alloc_frame(){
    return malloc(sizeof(ei_impl_frame_t));
}


/**
 * \brief Fonction pour free un espace alloué a un widget frame.
 *
 */
void ei_impl_release_frame(ei_widget_t frame){
    free((ei_impl_frame_t*)frame);
}

/**
 * \brief Fonction pour dessiner un widget frame.
 */
void ei_impl_draw_frame(ei_widget_t widget,ei_surface_t surface,ei_surface_t pick_surface,ei_rect_t* clipper){
    hw_surface_unlock(surface);
    ei_size_t size= hw_surface_get_size(surface);
    ei_rect_t rect= hw_surface_get_rect(surface);
    ei_point_t point_array[4] = {rect.top_left,
                                 {rect.top_left.x + size.width, rect.top_left.y},
                                 {rect.top_left.x + size.width, rect.top_left.y + size.height},
                                 {rect.top_left.x, rect.top_left.y + size.height}};
    ei_draw_polygon(surface,point_array,4,((ei_impl_frame_t*)widget)->frame_color,clipper);
    hw_surface_update_rects(surface,NULL);
    hw_surface_lock(surface);
}



/**
 * \brief Fonction pour mettre les valeurs par defauts d'un widget frame
 */
void ei_impl_setdefaults_frame(ei_widget_t widget){
    ei_impl_frame_t* frame = (ei_impl_frame_t*)widget;
    frame->widget.wclass = ei_widgetclass_from_name((ei_const_string_t){"frame"});
    //frame->widget.pick_id;
    //frame->widget.pick_color;
    frame->widget.user_data = NULL;
    frame->widget.destructor = NULL;
    /* Widget Hierachy Management */
    frame->widget.parent = ei_app_root_widget();		///< Pointer to the parent of this widget.
    frame->widget.children_head=NULL;	///< Pointer to the first child of this widget.	Children are chained with the "next_sibling" field.
    frame->widget.children_tail=NULL;	///< Pointer to the last child of this widget.
    frame->widget.next_sibling=NULL;	///< Pointer to the next child of this widget's parent widget.

    /* Geometry Management */
    frame->widget.geom_params = NULL;	///< Pointer to the geometry management parameters for this widget. If NULL, the widget is not currently managed and thus, is not displayed on the screen.
    //frame->widget.requested_size;	///< See \ref ei_widget_get_requested_size.
    //frame->widget.screen_location;///< See \ref ei_widget_get_screen_location.
    //frame->widget.content_rect;	///< See ei_widget_get_content_rect. By defaults, points to the screen_location.
}

