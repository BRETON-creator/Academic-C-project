/**
* @file	ei_event.c
 *
 * @brief 	Implémentation des fonctions liées à la gestion des évenements

 */

#include "var.h"
#include "ei_event.h"
#include "ei_implementation.h"
#include "ei_impl_binds.h"

/**
 * \brief	Binds a callback to an event type and a widget or a tag.
 *
 * @param	eventtype	The type of the event.
 * @param	widget		The callback is only called if the event is related to this widget.
 *				This parameter must be NULL if the "tag" parameter is not NULL.
 * @param	tag		The callback is only called if the event is related to a widget that
 *				has this tag. A tag can be a widget class name, or the tag "all".
 *				This parameter must be NULL is the "widget" parameter is not NULL.
 * @param	callback	The callback (i.e. the function to call).
 * @param	user_param	A user parameter that will be passed to the callback when it is called.
 */
void		ei_bind			(ei_eventtype_t		eventtype,
                     ei_widget_t		widget,
                     ei_tag_t		tag,
                     ei_callback_t		callback,
                     void*			user_param){
    ei_create_bind(eventtype,widget,tag,callback,user_param);

}

/**
 * \brief	Unbinds a callback from an event type and widget or tag.
 *
 * @param	eventtype, widget, tag, callback, user_param
 *				All parameters must have the same value as when \ref ei_bind was
 *				called to create the binding.
 */
void		ei_unbind		(ei_eventtype_t		eventtype,
                     ei_widget_t		widget,
                     ei_tag_t		tag,
                     ei_callback_t		callback,
                     void*			user_param){
    ei_delete_bind(eventtype,widget,tag,callback,user_param);

}
