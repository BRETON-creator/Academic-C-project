//
// Created by montelr on 5/17/24.
//
#include "ei_types.h"
#include "hw_interface.h"

#ifndef PROJETC_IG_EI_IMPLEMENTATION_ENTRY_H
#define PROJETC_IG_EI_IMPLEMENTATION_ENTRY_H

/**
 *  \brief fonction pour alloué un espace pour un widget entry.
 *
 * @return un espace assez grand pour stocker un widget de classe entry.
 */
ei_widget_t ei_impl_alloc_entry();

/**
 * \brief Fonction pour free un espace alloué a un widget entry.
 *
 */

void ei_impl_release_entry(ei_widget_t entry);

/**
* \brief Fonction pour mettre les valeurs par defauts d'un widget entry
*/
void ei_impl_setdefaults_entry(ei_widget_t widget);

/**
* \brief Fonction pour dessiner un widget entry.
*
*/
void ei_impl_draw_entry(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);

void ei_impl_geomnotify_entry(ei_widget_t widget);

typedef struct letter;

const char* from_list_to_string(struct letter* tab)

#endif //PROJETC_IG_EI_IMPLEMENTATION_ENTRY_H

