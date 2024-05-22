/**
 * @file	ei_widget_configure.c
 *
 * @brief 	implementation API for widgets configuration.
 *
 *  Created by François Bérard on 10.04.2023
 *  Copyright 2023 Ensimag. All rights reserved.
 */

#include "ei_widget_configure.h"
#include "ei_implementation.h"
#include "ei_placer.h"
#include "var.h"

/**
 * @brief	Configures the attributes of widgets of the class "frame".
 *
 *		Parameters obey the "optional parameters" protocol: if a parameter is "NULL", the state
 *		of the configured frame for that parameter does not change.
 *		When a frame is created, its parameters are set with the default values specified here.
 *
 * @param	widget		The widget to configure.
 * @param	requested_size	The size requested for this widget, including the widget's borders.
 *				The geometry manager may override this size due to other constraints.
 *				Defaults to the "natural size" of the widget, ie. big enough to
 *				display the border and the text or the image. This may be (0, 0)
 *				if the widget has border_width=0, and no text and no image.
 * @param	color		The color of the background of the widget. Defaults to
 *				\ref ei_default_background_color.
 * @param	border_width	The width in pixel of the border decoration of the widget. The final
 *				appearance depends on the "relief" parameter. Defaults to 0.
 * @param	relief		Appearance of the border of the widget. Defaults to
 *				\ref ei_relief_none.
 * @param	text		The text to display in the widget, or NULL. Only one of the
 *				parameter "text" and "img" should be used (i.e. non-NULL). Defaults
 *				to NULL.
 * @param	text_font	The font used to display the text. Defaults to \ref ei_default_font.
 * @param	text_color	The color used to display the text. Defaults to
 *				\ref ei_font_default_color.
 * @param	text_anchor	The anchor of the text, i.e. where it is placed within the widget.
 *				Defines both the anchoring point on the parent and on the widget.
 *				Defaults to \ref ei_anc_center.
 * @param	img		The image to display in the widget, or NULL. Any surface can be
 *				used, but usually a surface returned by \ref hw_image_load. Only one
 *				of the parameter "text" and "img" should be used (i.e. non-NULL).
 				Defaults to NULL.
 * @param	img_rect	If not NULL, the rectangle defines a subpart of "img" to use as the
 *				image displayed in the widget. Defaults to NULL.
 * @param	img_anchor	The anchor of the image, i.e. where it is placed within the widget
 *				when the size of the widget is bigger than the size of the image.
 *				Defaults to \ref ei_anc_center.
 */
void			ei_frame_configure		(ei_widget_t		widget,
							 ei_size_t*		requested_size,
							 const ei_color_t*	color,
							 int*			border_width,
							 ei_relief_t*		relief,
							 ei_string_t*		text,
							 ei_font_t*		text_font,
							 ei_color_t*		text_color,
							 ei_anchor_t*		text_anchor,
							 ei_surface_t*		img,
							 ei_rect_ptr_t*		img_rect,
							 ei_anchor_t*		img_anchor){
    ei_impl_frame_t* frame = ((ei_impl_frame_t*)widget);
    if (color)
        frame->frame_color = *color;
    if (!relief && !(frame->frame_relief) ) frame->frame_relief = ei_relief_none;
    else if (relief) frame->frame_relief = *relief;
    if (border_width) ((ei_impl_frame_t*)widget)->border_size =*border_width;
    if (requested_size) {
        widget->requested_size=*requested_size;
        widget->screen_location.size=*requested_size;
    }
    if (text) {
        if (!frame->text) frame->text = calloc(50, sizeof(char));
        strcpy(frame->text, *text);
    }
    if (text_font) frame->text_font = *text_font;
    if (text_color) {
        frame->text_color = *text_color;
    }
    if (text_anchor) frame->text_anchor = *text_anchor;

    if (img_rect){
        frame->rect_image = calloc(1,sizeof(ei_rect_t));
        frame->rect_image->top_left = (*img_rect)->top_left;
        frame->rect_image->size = (*img_rect)->size;
    }
    if (img){
            if (!img_rect && !frame->rect_image){
                frame->rect_image = calloc(1,sizeof(ei_rect_t));
                ei_rect_t rect = hw_surface_get_rect(*img);
                frame->rect_image->top_left=rect.top_left;
                frame->rect_image->size=rect.size;
            }
            frame->image = hw_surface_create(ei_app_root_surface(),frame->rect_image->size,false);

            hw_surface_lock(frame->image);
            hw_surface_lock(*img);
            ei_copy_surface(frame->image,&(ei_rect_t){{0,0},frame->rect_image->size}, *img, frame->rect_image, false);
            hw_surface_unlock(frame->image);
            hw_surface_unlock(*img);
    }
    if (img_anchor) frame->image_anchor= *img_anchor;
    ei_app_invalidate_rect(&widget->screen_location);
}





/**
 * @brief	Configures the attributes of widgets of the class "button".
 *
 * @param	widget, requested_size, color, border_width, relief,
 *		text, text_font, text_color, text_anchor,
 *		img, img_rect, img_anchor
 *				See the parameter definition of \ref ei_frame_configure. The only
 *				difference is that relief defaults to \ref ei_relief_raised
 *				and border_width defaults to \ref k_default_button_border_width.
 * @param	corner_radius	The radius (in pixels) of the rounded corners of the button.
 *				0 means straight corners. Defaults to \ref k_default_button_corner_radius.
 * @param	callback	The callback function to call when the user clicks on the button.
 *				Defaults to NULL (no callback).
 * @param	user_param	A programmer supplied parameter that will be passed to the callback
 *				when called. Defaults to NULL.
 */
void			ei_button_configure		(ei_widget_t		widget,
							 ei_size_t*		requested_size,
							 const ei_color_t*	color,
							 int*			border_width,
							 int*			corner_radius,
							 ei_relief_t*		relief,
							 ei_string_t*		text,
							 ei_font_t*		text_font,
							 ei_color_t*		text_color,
							 ei_anchor_t*		text_anchor,
							 ei_surface_t*		img,
							 ei_rect_ptr_t*		img_rect,
							 ei_anchor_t*		img_anchor,
							 ei_callback_t*		callback,
							 ei_user_param_t*	user_param){

        ei_frame_configure(widget, requested_size, color,border_width,relief,text,text_font,text_color,text_anchor,img,img_rect,img_anchor);
        if (corner_radius) ((ei_impl_button_t*)widget)->rayon = *corner_radius;
        if (callback) ((ei_impl_button_t*)widget)->callback = *callback;
        if (user_param) ((ei_impl_button_t*)widget)->user_params = *user_param;

        if (callback) ((ei_impl_button_t*)widget)->callback = *callback;
        if (user_param) ((ei_impl_button_t*)widget)->user_params = *user_param;
        ei_app_invalidate_rect(&widget->screen_location);
}


/**
 * @brief	Configures the attributes of widgets of the class "toplevel".
 *
 * @param	widget		The widget to configure.
 * @param	requested_size	The content size requested for this widget, this does not include
 *				the decorations	(border, title bar). The geometry manager may
 *				override this size due to other constraints.
 *				Defaults to (320x240).
 * @param	color		The color of the background of the content of the widget. Defaults
 *				to \ref ei_default_background_color.
 * @param	border_width	The width in pixel of the border of the widget. Defaults to 4.
 * @param	title		The string title displayed in the title bar. Defaults to "Toplevel".
 *				Uses the font \ref ei_default_font.
 * @param	closable	If true, the toplevel is closable by the user, the toplevel must
 *				show a close button in its title bar. Defaults to true.
 * @param	resizable	Defines if the widget can be resized horizontally and/or vertically
 *				by the user. Defaults to \ref ei_axis_both.
 * @param	min_size	For resizable widgets, defines the minimum size of its content.
 *				The default minimum size of a toplevel is (160, 120).
 *				If *min_size is NULL, this requires the min_size to be configured to
 *				the default size.
 */
void			ei_toplevel_configure		(ei_widget_t		widget,
							 ei_size_t*		requested_size,
							 const ei_color_t*	color,
							 int*			border_width,
							 ei_string_t*		title,
							 bool*			closable,
							 ei_axis_set_t*		resizable,
						 	 ei_size_ptr_t*		min_size){

        ei_impl_toplevel_t * toplevel = ((ei_impl_toplevel_t*)widget);


        if (color) toplevel->color = *color;
        if (border_width) toplevel->border_width = *border_width;
        if (title) toplevel->title = *title;
        if (closable) toplevel->can_close = *closable;
        else toplevel->can_close = true;
        if (resizable) toplevel->resizable_axis = *resizable;
        if (min_size) toplevel->minimal_size = **min_size;

        if (requested_size) {
                requested_size->width+=2*toplevel->border_width;
                requested_size->height+=2*toplevel->border_width+2*k_default_button_corner_radius;
                toplevel->widget.requested_size = *requested_size;
                toplevel->widget.screen_location.size = *requested_size;
        }

        if (toplevel->can_close==false){
                (toplevel->button->geom_params->manager->releasefunc)(toplevel->button);
                toplevel->button->geom_params = NULL;
                ei_impl_release_button(toplevel->button);
                toplevel->button=NULL;
        }

        if (toplevel->frame && toplevel->resizable_axis==ei_axis_none){
            if (toplevel->frame->geom_params && toplevel->frame->geom_params->manager)
                (toplevel->frame->geom_params->manager->releasefunc)(toplevel->frame);
            toplevel->frame->geom_params = NULL;
            ei_widget_destroy(toplevel->frame);
            toplevel->frame=NULL;
        }
        else{
                ei_place(toplevel->frame, &(ei_anchor_t){ei_anc_southeast},
                         NULL, NULL, NULL, NULL, &(float){1.0}, &(float){1.0}, NULL, NULL);
        }

        ei_impl_frame_t * frame = ((ei_impl_frame_t*)toplevel->contain_frame);

        int border =toplevel->border_width;
        ei_frame_configure		(frame, &(ei_size_t){toplevel->widget.requested_size.width-2*border,
                                                               toplevel->widget.requested_size.height-2*border-k_default_button_corner_radius*2},
                                           color,
                                           &(int){0}, NULL,NULL,
                                           NULL, NULL,
                                           NULL, NULL, NULL, NULL);


        ei_place(frame, &(ei_anchor_t){ei_anc_northwest},
                 &(int){border}, &(int){border+k_default_button_corner_radius*2}, NULL, NULL, &(float){0.0}, &(float){0.0}, NULL, NULL);


    ei_app_invalidate_rect(&widget->screen_location);
}



