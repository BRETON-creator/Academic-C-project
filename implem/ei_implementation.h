/**
 * @file	ei_implementation.h
 *
 * @brief 	Private definitions.
 * 
 */

#ifndef EI_IMPLEMENTATION_H
#define EI_IMPLEMENTATION_H

#include "hw_interface.h"
#include "ei_types.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"
#include "ei_application.h"



/**
 * \brief	Fields common to all types of widget. Every widget classes specializes this base
 *		class by adding its own fields.
 */
typedef struct ei_impl_widget_t {
	ei_widgetclass_t*	wclass;		///< The class of widget of this widget. Avoids the field name "class" which is a keyword in C++.
	uint32_t		pick_id;	///< Id of this widget in the picking offscreen.
	ei_color_t*		pick_color;	///< pick_id encoded as a color.
	void*			user_data;	///< Pointer provided by the programmer for private use. May be NULL.
	ei_widget_destructor_t	destructor;	///< Pointer to the programmer's function to call before destroying this widget. May be NULL.

	/* Widget Hierachy Management */
	ei_widget_t		parent;		///< Pointer to the parent of this widget.
	ei_widget_t		children_head;	///< Pointer to the first child of this widget.	Children are chained with the "next_sibling" field.
	ei_widget_t		children_tail;	///< Pointer to the last child of this widget.
	ei_widget_t		next_sibling;	///< Pointer to the next child of this widget's parent widget.

	/* Geometry Management */
	ei_geom_param_t		geom_params;	///< Pointer to the geometry management parameters for this widget. If NULL, the widget is not currently managed and thus, is not displayed on the screen.
	ei_size_t		requested_size;	///< See \ref ei_widget_get_requested_size.
	ei_rect_t		screen_location;///< See \ref ei_widget_get_screen_location.
	ei_rect_t*		content_rect;	///< See ei_widget_get_content_rect. By defaults, points to the screen_location.
} ei_impl_widget_t;

/**
 * @brief   Implementation of the frame type of widget.
 *      Contains this self attributes :
 *          - ei_relief_t frame_relief: an enumerate specifying the type of relief of the widget
 *          - ei_color_t frame_color: color type defining the bg color of the frame
 *          - char* text : the text of the frame
 *          - ei_font_t text_font : define font
 *          - int text_size : define the size of the font
 *          - ei_color_t text_color : define color of text
 *          - ei_anchor_t text_anchor : define position of anchor
 *          - file* image : tell if there is supposed to be an image on the surface of the widget
 *          - ei_anchor_t image_anchor : define anchor of the image
 *          - ei_surface_t rect_image : give the surface of the image to use.
 */
typedef struct ei_impl_frame_t {
    ei_impl_widget_t widget;
    ei_relief_t frame_relief;
    ei_color_t frame_color;
    char* text;
    ei_font_t text_font;
    int text_size;
    ei_color_t text_color;
    ei_anchor_t text_anchor;
    ei_const_string_t image;
    ei_anchor_t image_anchor;
    ei_surface_t rec_image;
} ei_impl_frame_t;



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


/**
 * \brief	Fields common to all geometry managers. Every geometry manager specializes this by adding its own fields.
 */
typedef struct ei_impl_geom_param_t {
	ei_geometrymanager_t*		manager;	///< The geometry managers that manages this widget.
} ei_impl_geom_param_t;

//================================================================================================
/**
 *  \brief fonction pour alloué un espace pour un widget frame.
 *
 * @return un espace assez grand pour stocker un widget de classe frame.
 */
ei_widget_t ei_impl_alloc_frame();


/**
 * \brief Fonction pour free un espace alloué a un widget frame.
 *
 */
void ei_impl_release_frame(ei_widget_t);

/**
 * \brief Fonction pour dessiner un widget frame.
 */
void ei_impl_draw_frame(ei_widget_t widget,ei_surface_t surface,ei_surface_t pick_surface,ei_rect_t* clipper);

/**
 * \brief Fonction pour mettre les valeurs par defauts d'un widget frame
 */
void ei_impl_setdefaults_frame(ei_widget_t);

#endif
