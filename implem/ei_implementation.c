/**
 * @file	ei_implementation.h
 *
 * @brief 	Private definitions.
 * 
 */


#include "ei_implementation.h"




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
    ei_draw_polygon		(surface,point_array,4,((ei_impl_frame_t*)widget)->frame_color,clipper);
}

/**
 * \brief Fonction pour mettre les valeurs par defauts d'un widget frame
 */
void ei_impl_setdefaults_frame(ei_widget_t widget){
    ei_impl_frame_t* frame = (ei_impl_frame_t*)widget;
}

