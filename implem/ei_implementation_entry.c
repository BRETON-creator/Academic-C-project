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





