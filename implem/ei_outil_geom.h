//
// Created by anto on 15/05/24.
//

#ifndef PROJETC_IG_EI_OUTIL_GEOM_H
#define PROJETC_IG_EI_OUTIL_GEOM_H

#include "ei_implementation.h"

/**
 * @brief fonction pour avoir le plus petit rectangle contenant une liste de rectangle
 *
 * @return renvoie le plus petit rectangle contenant tout les rectangles de la liste
 */
ei_rect_t get_smallest_containing_rect(ei_linked_rect_t *list);

/**
* \brief Fonction pour obtenir l'union de deux rectangles
*
*/
ei_rect_t get_rect_union( ei_rect_t old_rect , ei_rect_t new_rect);

/**
* \brief Fonction pour obtenir l'intersection de deux rectangles
*
*/
ei_rect_t get_rect_intersection( ei_rect_t old_rect , ei_rect_t new_rect);

/**
 * fonction permettant qui ecrit dans circle l'ensemble des points pour creer une frame arrondie aux bords.
 * @param circle
 * @param rect
 * @param radius
 */
void give_rounded_frame(ei_point_t* circle, ei_rect_t rect, int radius);

/**
 * Fonction qui ecrit dans lower_frame la partie basse du relief d'un bouton/frame
 * @param rounded_frame
 * @param rect
 * @param h
 * @param lower_frame
 */
void give_lower_frame(ei_point_t* rounded_frame,ei_rect_t rect, int h, ei_point_t* lower_frame);

/**
 * Fonction qui ecrit dans upper_frame la partie haute du relief d'un bouton/frame
 * @param rounded_frame
 * @param rect
 * @param h
 * @param upper_frame
 */
void give_upper_frame(ei_point_t* rounded_frame,ei_rect_t rect, int h, ei_point_t* upper_frame);

/**
 * Donne le coin topleft pour placer un rectangle de texte de taille size_text
 * @param rect
 * @param anchor
 * @param size_text
 * @return
 */
ei_point_t place_text(ei_rect_t rect, ei_anchor_t anchor, ei_size_t size_text);

#endif //PROJETC_IG_EI_OUTIL_GEOM_H
