//
// Created by anto on 15/05/24.
//

#ifndef PROJETC_IG_EI_OUTIL_GEOM_H
#define PROJETC_IG_EI_OUTIL_GEOM_H

#include "ei_implementation.h"

ei_rect_t get_rect_union( ei_rect_t old_rect , ei_rect_t new_rect);

ei_rect_t get_rect_intersection( ei_rect_t old_rect , ei_rect_t new_rect);

void give_rounded_frame(ei_point_t* circle, ei_rect_t rect, int radius);

void give_lower_frame(ei_point_t* rounded_frame,ei_rect_t rect, int h, ei_point_t* lower_frame);

void give_upper_frame(ei_point_t* rounded_frame,ei_rect_t rect, int h, ei_point_t* upper_frame);

ei_point_t place_text(ei_rect_t rect, ei_anchor_t anchor, ei_size_t size_text);

#endif //PROJETC_IG_EI_OUTIL_GEOM_H
