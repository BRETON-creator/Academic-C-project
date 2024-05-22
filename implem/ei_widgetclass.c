/**
 *  @file	ei_widgetclass.h
 *  @brief	Definition and registration of widget classes.
 *
 *  \author
 *  Created by François Bérard on 02.12.11.
 *  Copyright 2011 Ensimag. All rights reserved.
 *
 */

#include "ei_widgetclass.h"
#include "ei_implementation.h"

/**
 * on implémente une liste de toutes les widgetclass.
 */
ei_widgetclass_t* tete=NULL;

/**
 * \brief	Returns the size of the opaque struct that is pointed to by \ref ei_widget_t
 * 			(i.e. the size of \ref ei_impl_widget_t).
 */
size_t		ei_widget_struct_size(){
    return sizeof(ei_impl_widget_t);
}

/**
 * @brief	Registers a class to the program so that widgets of this class can be created
 * 		with \ref ei_widget_create.
 *		This must be done only once per widget class in the application.
 *
 * @param	widgetclass	The structure describing the class.
 */
void			ei_widgetclass_register		(ei_widgetclass_t* widgetclass){
    //On ajoute en tete de la liste chainée des widgetclass le widget qu'on vient de creer.
    ei_widgetclass_t * tmp = tete;
    tete=widgetclass;
    widgetclass->next=tmp;
}


/**
 * @brief	Returns the structure describing a class, from its name.
 *
 * @param	name		The name of the class of widget.
 *
 * @return			The structure describing the class.
 */
ei_widgetclass_t*	ei_widgetclass_from_name	(ei_const_string_t name){
    ei_widgetclass_t* current = tete;
    while (current){
        if (strcmp(current->name,name)==0) return current;
        current=current->next;
    }
    return NULL;
}





