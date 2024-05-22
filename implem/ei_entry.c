//
// Created by François Bérard on 15/04/2024.
//


#include "ei_entry.h"
#include "ei_implementation.h"

//variable globale designant l'entry qui possède le focus.
ei_impl_entry_t *current_entry_focus = NULL;

/**
 * @brief	Configures the attributes of widgets of the class "entry".
 *
 * @param	widget		The widget to configure.
 * @param	requested_char_size	The size requested for this widget in "chars". If provided, the requested size
 * 					of the widget must be just large enough to show this amount of 'm' characters
 * 					(the largest character). Defaults to 10.
 * @param	color		The color of the background of the widget. Defaults to
 *				\ref ei_default_background_color.
 * @param	border_width	The width in pixel of the border of the widget. Defaults to 2. Minimum is 2.
 * 					When the widget does not have the focus, its border is shown with a
 * 					width of <border_width> - 1, but the content is enlarged so that
 * 					the widget has the same size as when in focus.
 * @param	text_font	The font used to display the text. Defaults to \ref ei_default_font.
 * @param	text_color	The color used to display the text. Defaults to \ref ei_font_default_color.
 */
void			ei_entry_configure		(ei_widget_t		widget,
							 int*			requested_char_size,
							 const ei_color_t*	color,
							 int*			border_width,
							 ei_font_t*		text_font,
							 ei_color_t*		text_color){
    ei_impl_entry_t * entry = (ei_impl_entry_t*)widget;
    //if (requested_char_size) entry->requested_char_size = *requested_char_size;
    if (border_width) entry->border_size= *border_width;
    if (text_font) entry->text_font = *text_font;
    if (text_color) entry->text_color = *text_color;
    //if (color) entry->color = color;
    //TODO : modif entry pour que ca corresponde
    ei_app_invalidate_rect(&widget->screen_location);
}

/**
 * @brief	Sets the text displayed in the entry widget.
 *
 * @param	widget		The widget to set.
 * @param	text		The text to show in the widget.
 */
void			ei_entry_set_text		(ei_widget_t		widget,
							 ei_const_string_t 	text){
    size_t length = strlen(text);
    char* new_text = (char*)malloc(length * sizeof(char));
    strncpy(new_text, text, length);
    ((ei_impl_entry_t*)widget)->text = new_text;
}

/**
 * @brief	Gets the text that is displayed in the entry widget.
 *
 * @param	widget		The widget to get the text from.
 *
 * @return			The text currently shown in the widget.
 */
ei_const_string_t 	ei_entry_get_text		(ei_widget_t		widget){
    //le texte qui est affiché est les m derniers caractères (m = widget->screen_location)
}

/**
 * @brief	Gives the keyboard input focus to the entry widget.
 *
 * @param	widget		The widget to receive the keyboard input focus.
 */
void			ei_entry_give_focus		(ei_widget_t		widget){
    current_entry_focus=(ei_impl_entry_t*)widget;
}

