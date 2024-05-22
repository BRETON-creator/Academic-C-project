/**
 * @file	ei_widget.h
 *
 * @brief 	API for widgets management: creation, destruction
 *
 *  Created by François Bérard on 30.12.11.
 *  Copyright 2011 Ensimag. All rights reserved.
 */


#include "ei_widget.h"
#include "ei_implementation.h"
#include "var.h"
#include "ei_widgetclass.h"

/**
 * @brief Definition de la variable globale permettant de donner les pick id aux widgets,
 * elle est incrémentée au fur et a mesure d'ajout de widget.
 */
uint32_t next_pick_id = 0x00000FFF;

/**
 * @brief Donne la couleur equivalente au pick id, il faudra bien penser a free la couleur attribuée
 * @param pickid le pick id du widget dont on souhaite la couleur
 * @return la couleur correspondant au pick id.
 */
ei_color_t* give_color_pickid(uint32_t pickid){
    ei_color_t* pickcolor = calloc(1,sizeof(ei_color_t));
    pickcolor->alpha = pickid & 0xFF;
    pickcolor->red = (pickid >> 24) & 0xFF;
    pickcolor->green = (pickid >> 16) & 0xFF;
    pickcolor->blue = (pickid >> 8) & 0xFF;
    return pickcolor;
}

/**
 * @brief	Creates a new instance of a widget of some particular class, as a descendant of
 *		an existing widget.
 *
 *		The widget is not displayed on screen until it is managed by a geometry manager.
 *		When no more needed, the widget must be released by calling \ref ei_widget_destroy.
 *
 * @param	class_name	The name of the class of the widget that is to be created.
 * @param	parent 		A pointer to the parent widget. Can not be NULL.
 * @param	user_data	A pointer provided by the programmer for private use. May be NULL.
 * @param	destructor	A pointer to a function to call before destroying a widget structure. May be NULL.
 *
 * @return			The newly created widget, or NULL if there was an error.
 */
ei_widget_t		ei_widget_create		(ei_const_string_t	class_name,
							 ei_widget_t		parent,
							 ei_user_param_t	user_data,
							 ei_widget_destructor_t destructor){
    ei_widget_t new_widget = ei_widgetclass_from_name(class_name)->allocfunc();
    new_widget->wclass = ei_widgetclass_from_name(class_name);
    new_widget->wclass->setdefaultsfunc(new_widget);


    if (strcmp(parent->wclass->name, (ei_widgetclass_name_t){"toplevel\0"})==0) {

            if (((ei_impl_toplevel_t*)parent)->contain_frame) {

                    parent=((ei_impl_toplevel_t*)parent)->contain_frame;
            }
    }

    new_widget->parent=parent;
    parent->children_tail = new_widget;
    ei_widget_t prec = parent->children_head;
    if (prec==NULL) {
        parent->children_head=new_widget;
    }else{
        while (prec->next_sibling != NULL){
            prec = prec -> next_sibling;
        }
        prec->next_sibling = new_widget;
    }
    new_widget->next_sibling = NULL;

    new_widget->user_data=user_data;
    new_widget->destructor=destructor;
    new_widget->pick_id = next_pick_id;
    next_pick_id += 0x00000100;
    new_widget->pick_color = give_color_pickid(new_widget->pick_id);

    return new_widget;
}

/**
 * @brief	Destroys a widget.
 * 		Removes the widget from the screen if it is currently displayed.
 * 		Destroys all its descendants.
 *		Calls its destructor if it was provided.
 * 		Frees the memory used by the widget (e.g. the widget param).
 *
 * @param	widget		The widget that is to be destroyed.
 */
void			ei_widget_destroy		(ei_widget_t		widget){
    ei_widget_t child = widget->children_head;
    ei_widget_t next_child;
    while (child) {
        next_child = child->next_sibling;
        ei_widget_destroy(child);
        child = next_child;

    }
    free(widget->pick_color);
    if (widget->destructor) (widget->destructor)(widget);
    if (widget->geom_params && widget->geom_params->manager) (widget->geom_params->manager->releasefunc)(widget);
    (widget->wclass->releasefunc)(widget);
}


/**
 * @brief	Returns if the widget is currently displayed (i.e. managed by a geometry manager).
 *
 * @param	widget		The widget.
 *
 * @return			true if the widget is displayed, false otherwise.
 */
bool	 		ei_widget_is_displayed		(ei_widget_t		widget){
    return (widget->geom_params!=NULL && widget->geom_params->manager!=NULL);
}


uint32_t* get_pixel_point( ei_point_t point){
    hw_surface_lock(pick_surface);
    uint32_t *pixel_ptr = (uint32_t *) hw_surface_get_buffer(pick_surface);
    //
    //pixel_ptr == (0,0)
    ei_size_t size = hw_surface_get_size(pick_surface);
    int idx_point = point.x + point.y*size.width;
    uint32_t * pixel_ptr_n = pixel_ptr+ idx_point;
    //int x = ((pixel_ptr_n - pixel_ptr))%size.width; // \/ c'etait pour le debug
    //int y = ((pixel_ptr_n - pixel_ptr))/size.width;
    //printf("%u %u, x: %d, y: %d\n",pixel_ptr,pixel_ptr_n,x,y );
    //ei_color_t *col = give_color_pickid(*pixel_ptr_n);
    //printf("%08x\n",(*pixel_ptr_n)<<8 | 0xFF);
    //printf("R:%02x G:%02x B:%02x A:%02x\n",col->red,col->green,col->blue,col->alpha);
    //free(col);
    hw_surface_unlock(pick_surface);
    return pixel_ptr_n;
}

/**
 * @brief Donne le widget correspondant a la pickid dans les descendant de widget (fonction recursive)
 *
 * @param current widget racine
 * @param pick_id pick id que l'on recherche
 * @return le widget qui correspond a la pick id ou NULL.
 */
ei_widget_t widget_from_pickid(ei_widget_t current, uint32_t pick_id){
    if (!current) return NULL;
    if (current->pick_id == pick_id) return current;
    ei_widget_t child = current->children_head;
    ei_widget_t result = widget_from_pickid(child,pick_id);
    while (child && !result){
        child=child->next_sibling;
        result = widget_from_pickid(child,pick_id);
    }
    return result;
}
/**
 * @brief	Returns the widget that is at a given location on screen.
 *
 * @param	where		The location on screen, expressed in the root window coordinates.
 *
 * @return			The top-most widget at this location, or NULL if there is no widget
 *				at this location (except for the root widget).
 */
ei_widget_t		ei_widget_pick			(ei_point_t*		where){
    //on recupere le pick_id du widget du pixel sur lequel on se trouve
    uint32_t pick_id= (*get_pixel_point(*where)) << 8 | 0xFF; // pourquoi ? idk
    //on trouve a quel widget il appartient et on renvoit ce widget, si c'est la racine on renvoie NULL
    ei_widget_t current = ei_app_root_widget();
    current = widget_from_pickid(current,pick_id);
    //printf("Je suis sur le widget %s %08x\n", current->wclass->name, current->pick_id);
    if (current == ei_app_root_widget()) return NULL;
    return current;
}





