//
// Created by montelr on 5/14/24.
//
#include "ei_types.h"
#include "ei_widget.h"

#ifndef PROJETC_IG_EI_IMPLEMENTATION_TOPLEVEL_H
#define PROJETC_IG_EI_IMPLEMENTATION_TOPLEVEL_H

//============================= toplevel

/**
 *  \brief fonction pour alloué un espace pour un widget toplevel.
 *
 * @return un espace assez grand pour stocker un widget de classe toplevel.
 */
ei_widget_t ei_impl_alloc_toplevel();

/**
 * \brief Fonction pour free un espace alloué a un widget toplevel.
 *
 */
void ei_impl_release_toplevel(ei_widget_t toplevel);

/**
* \brief Fonction pour mettre les valeurs par defauts d'un widget toplevel
*/

void ei_impl_setdefaults_toplevel(ei_widget_t widget);

/**
* \brief Fonction pour dessiner un widget toplevel.
*
*/
void ei_impl_draw_toplevel(ei_widget_t widget,ei_surface_t surface,ei_surface_t pick_surface,ei_rect_t* clipper);

bool toplevel_close(ei_widget_t	widget,
                    ei_event_t*	event,
                    ei_user_param_t user_param);

bool ei_callback_toplevel(ei_widget_t		widget, struct ei_event_t*	event, ei_user_param_t	user_param);

void ei_impl_geomnotify_toplevel(ei_widget_t widget);

#endif //PROJETC_IG_EI_IMPLEMENTATION_TOPLEVEL_H

