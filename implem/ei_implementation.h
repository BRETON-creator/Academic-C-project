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
#include "ei_event.h"
#include "ei_outil_geom.h"
#include "ei_implementation_toplevel.h"




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
 *      Contains these self attributes :
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
    int border_size;
    char* text;
    ei_font_t text_font;
    int text_size;
    ei_color_t text_color;
    ei_anchor_t text_anchor;
    ei_surface_t image;
    ei_anchor_t image_anchor;
    ei_rect_ptr_t rect_image;
} ei_impl_frame_t;

/**
 * @brief Implementation of the widget type button
 * Same as a frame but contains 3 self attributes :
 *  - int rayon : rayon des arrondis
 *  - ei_callback_t traitantfunc : adresse d'une fonction traitant. Doit etre appelée par la bibliothèque losque l'utilisateur clique sur le bouton.
 *  - void* user_params : adresse memoire permettant a l'utilisateur de passser un parametre spécifique a ce bouton lors de l'appel du traitant.
 */
typedef struct {
    ei_impl_frame_t frame;
    int rayon;
    ei_callback_t callback;
    void* user_params;
} ei_impl_button_t;

/**
 * @brief Implementation of widget type toplevel
 * Same as a widget but contains :
 * - char* title : title of the toplevel
 * - bool can_close : specify if the window is closable
 * - ei_axis_set_t resizable_axis : specifie quels axes sont redimmensionnables
 * - ei_size_t minimal_size : size minimale que le gestionnaire de géométrie devra prendre en compte.
 * -ei_widget_t button : bouton de fermeture du toplevel
 * -ei_widget_t frame : frame pour redimmensionner le toplevel
 */
typedef struct {
    ei_impl_widget_t widget;
    ei_color_t color;
    int border_width;
    char* title;
    bool can_close;
    ei_axis_set_t resizable_axis;
    ei_size_t minimal_size;
    ei_widget_t button;
    ei_widget_t frame;
    ei_widget_t contain_frame;
}ei_impl_toplevel_t;

/**
 * @brief Implementation of widget type entry
 */
typedef struct {

}ei_impl_entry_t;

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

typedef struct {
    ei_impl_geom_param_t geom_param;
    int x, y, height, width;
    float rel_x, rel_y, rel_height, rel_width;
    ei_anchor_t anchor;
} ei_placer_t;

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


//======================================= button

/**
 *  \brief fonction pour alloué un espace pour un widget button.
 *
 * @return un espace assez grand pour stocker un widget de classe button.
 */
ei_widget_t ei_impl_alloc_button();

/**
 * \brief Fonction pour free un espace alloué a un widget button.
 *
 */
void ei_impl_release_button(ei_widget_t button);

/**
* \brief Fonction pour mettre les valeurs par defauts d'un widget button
*/

void ei_impl_setdefaults_button(ei_widget_t widget);

/**
* \brief Fonction pour dessiner un widget button.
* dans la surface de la fenetre root ou de la fenetre du parent ?
* doit trouver le point ou on doit placer le button
* depend du point d'ancrage
* dessine le relief au bon endroit
* dessine le button au bon endroit
* pose le texte et l'image au bon endroit
*
*/
void ei_impl_draw_button(ei_widget_t widget,ei_surface_t surface,ei_surface_t pick_surface,ei_rect_t* clipper);





/**
 * @brief Callback interne des buttons. Utilise la variable globale "current_button_down" afin de savoir si on utilise mouse_buttonup en dehors du bouton courant
 * @param widget
 * @param event
 * @param user_param
 * @return la fonction callback a bien été utilisée
 */
bool ei_callback_clickbutton(ei_widget_t		widget, struct ei_event_t*	event, ei_user_param_t	user_param);

bool ei_callback_buttondown(ei_widget_t		widget, struct ei_event_t*	event, ei_user_param_t	user_param);

void supr_hierachy(ei_widget_t widget, ei_widget_t widget_supr);
#endif
