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

/**
 * fonction de callback du bouton close des toplevels
 * @param widget
 * @param event
 * @param user_param
 * @return true
 */
bool toplevel_close(ei_widget_t	widget,
                    ei_event_t*	event,
                    ei_user_param_t user_param);


/**
 * callback intern des toplevels permettant de deplacer la toplevel
 * @param widget
 * @param event
 * @param user_param
 * @return
 */
bool ei_callback_toplevel(ei_widget_t		widget, struct ei_event_t*	event, ei_user_param_t	user_param);
/**
 * Callback interne des toplevels permettant de redimensionner les toplevel l'autorisant.
 * @param widget
 * @param event
 * @param user_param
 * @return
 */
bool ei_resize_toplevel(ei_widget_t	widget, struct ei_event_t*	event, ei_user_param_t	user_param);

void ei_impl_geomnotify_toplevel(ei_widget_t widget);

/**
* \brief Fonction pour modifier la hiérarchie des widgets pour que le widget appelé écrase les autres fils de son parent : pour cela on le met à la fin
* de la liste des enfants.
*/
void modify_hierarchy( ei_widget_t widget , ei_widget_t parent);

#endif //PROJETC_IG_EI_IMPLEMENTATION_TOPLEVEL_H

