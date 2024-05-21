//
// Created by montelr on 5/17/24.
//
//
// Created by montelr on 5/14/24.
//

#include "ei_implementation.h"
#include "ei_draw.h"
#include "ei_event.h"
#include "var.h"
#include "hw_interface.h"
#include "ei_types.h"
#include "ei_widget_configure.h"
#include "ei_impl_placer.h"

/**
 *  \brief fonction pour alloué un espace pour un widget entry.
 *
 * @return un espace assez grand pour stocker un widget de classe entry.
 */
ei_widget_t ei_impl_alloc_entry(){
        return calloc(1,sizeof(ei_impl_entry_t));
}

/**
 * \brief Fonction pour free un espace alloué a un widget entry.
 *
 */

void ei_impl_release_entry(ei_widget_t entry){
        supr_hierachy(ei_app_root_widget(), entry);
        free((ei_impl_entry_t*)entry);
}

/**
* \brief Fonction pour mettre les valeurs par defauts d'un widget entry
*/
void ei_impl_setdefaults_entry(ei_widget_t widget){

}

/**
* \brief Fonction pour dessiner un widget entry.
*
*/
void ei_impl_draw_entry(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper){

}

void ei_impl_geomnotify_entry(ei_widget_t widget){

}

void ei_entryconfigure(ei_widget_t widget, int* border_size,char* text,ei_font_t* text_font,int* text_size,ei_color_t* text_color,ei_anchor_t* text_anchor){
    ei_impl_entry_t * entry = (ei_impl_entry_t*)widget;
    if (border_size) entry->border_size= *border_size;
    if (text){
        if (entry->text) free(entry->text);
        entry->text = calloc(50,sizeof(char));
        strcpy(entry->text,text);
    }
    if (text_font) entry->text_font = *text_font;
    if (text_size) entry->text_size = *text_size;
    if (text_color) entry->text_color = *text_color;
    if (text_anchor) entry->text_anchor = *text_anchor;
}




