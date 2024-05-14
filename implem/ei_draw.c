/**
 *  @file	ei_draw.h
 *  @brief	Graphical primitives to draw lines, polygons, text, and operation of drawing
 *		surfaces.
 *
 *  \author 
 *  Created by François Bérard on 30.12.11.
 *  Copyright 2011 Ensimag. All rights reserved.
 *
 */


#include "ei_draw.h"
#include "ei_implementation.h"

/**
 * \brief	Draws text by calling \ref hw_text_create_surface.
 *
 * @param	surface 	Where to draw the text. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	where		Coordinates, in the surface, where to anchor the top-left corner of
 *				the rendered text.
 * @param	text		The string of the text. Can't be NULL.
 * @param	font		The font used to render the text. If NULL, the \ref ei_default_font
 *				is used.
 * @param	color		The text color. Can't be NULL. The alpha parameter is not used.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void	ei_draw_text		(ei_surface_t		surface,
				 const ei_point_t*	where,
				 ei_const_string_t	text,
				 ei_font_t		font,
				 ei_color_t		color,
				 const ei_rect_t*	clipper){
    ei_surface_t surfacetext = hw_text_create_surface(text,font,color);
    ei_rect_t dst_rect = (ei_rect_t){*where, hw_surface_get_size(surfacetext)};
    hw_surface_lock(surface);
    hw_surface_lock(surfacetext);
    ei_copy_surface(surface, &dst_rect, surfacetext, NULL, true);
    hw_surface_unlock(surface);
    hw_surface_unlock(surfacetext);


}

/**
 * \brief	Fills the surface with the specified color.
 *
 * @param	surface		The surface to be filled. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	color		The color used to fill the surface. If NULL, it means that the
 *				caller want it painted black (opaque).
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void	ei_fill			(ei_surface_t		surface,
				 const ei_color_t*	color,
				 const ei_rect_t*	clipper);


/**
 * \brief	Copies pixels from a source surface to a destination surface.
 *		The source and destination areas of the copy (either the entire surfaces, or
 *		subparts) must have the same size before considering clipping.
 *		Both surfaces must be *locked* by \ref hw_surface_lock.
 *
 * @param	destination	The surface on which to copy pixels.
 * @param	dst_rect	If NULL, the entire destination surface is used. If not NULL,
 *				defines the rectangle on the destination surface where to copy
 *				the pixels.
 * @param	source		The surface from which to copy pixels.
 * @param	src_rect	If NULL, the entire source surface is used. If not NULL, defines the
 *				rectangle on the source surface from which to copy the pixels.
 * @param	alpha		If true, the final pixels are a combination of source and
 *				destination pixels weighted by the source alpha channel and
 *				the transparency of the final pixels is set to opaque.
 *				If false, the final pixels are an exact copy of the source pixels,
 				including the alpha channel.
 *
 * @return			Returns 0 on success, 1 on failure (different sizes between
 * 				source and destination).false
 */
int	ei_copy_surface		(ei_surface_t		destination,
				 const ei_rect_t*	dst_rect,
				 ei_surface_t		source,
				 const ei_rect_t*	src_rect,
				 bool			alpha){

    /*On initialise les valeurs dont on aura besoin...*/

    uint32_t *pixel_dst = (uint32_t*)hw_surface_get_buffer(destination);
    uint32_t *pixel_src = (uint32_t*)hw_surface_get_buffer(source);
    int x_dst,y_dst,x_src,y_src, width_src, width_dst, height_src, height_dst;
    ei_rect_t dst_surf_rect = hw_surface_get_rect(destination);
    ei_rect_t src_surf_rect = hw_surface_get_rect(source);
    if (src_rect){
        x_src = src_rect->top_left.x;
        y_src = src_rect->top_left.y;
        width_src = src_rect->size.width;
        height_src = src_rect->size.height;
    }else{
        x_src = 0;
        y_src = 0;
        width_src = src_surf_rect.size.width;
        height_src = src_surf_rect.size.height;
    }
    if (dst_rect) {
        x_dst = dst_rect->top_left.x;
        y_dst = dst_rect->top_left.y;
        width_dst = dst_rect->size.width;
        height_dst = dst_rect->size.height;
    }else{
        x_dst = 0;
        y_dst = 0;
        width_dst = dst_surf_rect.size.width;
        height_dst = dst_surf_rect.size.height;
    }
    pixel_dst= pixel_dst + x_dst + y_dst*dst_surf_rect.size.width;
    pixel_src = pixel_src + x_src + y_src*src_surf_rect.size.width;
    uint8_t *red_dst, *green_dst, *blue_dst, *alpha_dst;
    uint8_t *red_src, *green_src, *blue_src, *alpha_src;
    /*On copie la source dans la destination*/

    if (width_dst != width_src || height_dst != height_src) {
        printf("probleme");
        return 1;
    }

    int min_width = width_src < width_dst ? width_src : width_dst;
    int min_height = height_src < height_dst ? height_src : height_dst;
    for (int y=0; y < min_height; y++){
        for (int x=0; x < min_width; x++){
            if (x + x_dst < dst_surf_rect.size.width && x + x_dst > dst_surf_rect.top_left.x && y + y_dst > dst_surf_rect.top_left.y && y+y_dst<dst_surf_rect.size.height) {
                red_dst = (uint8_t *) (pixel_dst + x + y * dst_surf_rect.size.width);
                green_dst = red_dst + 1;
                blue_dst = red_dst + 2;
                //alpha_dst = red_dst + 3;
                red_src = (uint8_t *) (pixel_src + x + y * src_surf_rect.size.width);
                green_src = red_src + 1;
                blue_src = red_src + 2;
                alpha_src = red_src + 3;
                *(red_dst) = ((*red_dst) * (255 - (*alpha_src)) + (*red_src) * (*alpha_src)) / 255;
                *(green_dst) = ((*green_dst) * (255 - (*alpha_src)) + (*green_src) * (*alpha_src)) / 255;
                *(blue_dst) = ((*blue_dst) * (255 - (*alpha_src)) + (*blue_src) * (*alpha_src)) / 255;
            }
        }
    }

    return 0;
}


