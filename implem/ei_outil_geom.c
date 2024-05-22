//
// Created by anto on 15/05/24.
//

#include "ei_outil_geom.h"

/**
 * @brief fonction pour avoir le plus petit rectangle contenant une liste de rectangle
 *
 * @return renvoie le plus petit rectangle contenant tout les rectangles de la liste
 */
ei_rect_t get_smallest_containing_rect(ei_linked_rect_t* list){
    if (!list) return hw_surface_get_rect(ei_app_root_surface()) ;
    int max_x, max_y, min_x, min_y;
    ei_linked_rect_t* current= list;
    max_x = current->rect.top_left.x + current->rect.size.width;
    max_y = current->rect.top_left.y + current->rect.size.height;
    min_x = current->rect.top_left.x;
    min_y = current->rect.top_left.y;

    while (current){
        if (current->rect.top_left.x < min_x) min_x = current->rect.top_left.x;
        if (current->rect.top_left.y < min_y) min_y = current->rect.top_left.y;
        if (current->rect.top_left.x + current->rect.size.width > max_x) max_x = current->rect.top_left.x + current->rect.size.width;
        if (current->rect.top_left.y + current->rect.size.height > max_y) max_y = current->rect.top_left.y + current->rect.size.height;
        current=current->next;
    }
    return (ei_rect_t){{min_x,min_y}, {max_x-min_x,max_y-min_y}};
}


/**
* \brief Fonction pour obtenir l'union de deux rectangles
*
*/

//utile pour le redessin ?

ei_rect_t get_rect_union( ei_rect_t old_rect , ei_rect_t new_rect){
    ei_rect_t union_rect;
    //pour le top_left on choisit entre les deux top_left des rectangles celui qui est le plus proche de l'origine
    union_rect.top_left.x  = old_rect.top_left.x <= new_rect.top_left.x ? old_rect.top_left.x : new_rect.top_left.x;
    union_rect.top_left.y  = old_rect.top_left.y <= new_rect.top_left.y ? old_rect.top_left.y : new_rect.top_left.y;
    // On determine la size du rectangle englobant
    int max_x  = old_rect.top_left.x + old_rect.size.width <= new_rect.top_left.x + new_rect.size.width ?  new_rect.top_left.x + new_rect.size.width: old_rect.top_left.x + old_rect.size.width;
    int max_y  = old_rect.top_left.y + old_rect.size.height <= new_rect.top_left.y + new_rect.size.height ?  new_rect.top_left.y + new_rect.size.height: old_rect.top_left.y + old_rect.size.height;
    union_rect.size.height = max_x  -  union_rect.top_left.x;
    union_rect.size.width = max_y -  union_rect.top_left.y;

    return union_rect;

}

/**
* \brief Fonction pour obtenir l'intersection de deux rectangles
*
*/

ei_rect_t get_rect_intersection( ei_rect_t old_rect , ei_rect_t new_rect){
    ei_rect_t intersection_rect;
    intersection_rect.top_left.x = (old_rect.top_left.x > new_rect.top_left.x) ? old_rect.top_left.x : new_rect.top_left.x;
    intersection_rect.top_left.y = (old_rect.top_left.y > new_rect.top_left.y) ? old_rect.top_left.y : new_rect.top_left.y;

    intersection_rect.size.width = (( old_rect.top_left.x + old_rect.size.width < new_rect.top_left.x + new_rect.size.width) ?  old_rect.top_left.x + old_rect.size.width : new_rect.top_left.x + new_rect.size.width) - intersection_rect.top_left.x;
    intersection_rect.size.height = ((old_rect.top_left.y + old_rect.size.height < new_rect.top_left.y + new_rect.size.height) ? old_rect.top_left.y + old_rect.size.height : new_rect.top_left.y + new_rect.size.height) - intersection_rect.top_left.y;

    if (intersection_rect.size.width < 0 || intersection_rect.size.height < 0) {
        intersection_rect.size.width = 0;
        intersection_rect.size.height = 0;
    }
    return intersection_rect;
}

/**
 * fonction permettant qui ecrit dans circle l'ensemble des points pour creer une frame arrondie aux bords.
 * @param circle
 * @param rect
 * @param radius
 */
void give_rounded_frame(ei_point_t* circle, ei_rect_t rect, int radius) {
    float pi = 355./113.;
    float xpos, ypos;
    ei_point_t center;
    for (int i = 0; i < 40; i++) {
        xpos = cosf(((float) i / 20) * pi);
        ypos = sinf(((float) i / 20) * pi);
        if (0 <= i && i < 10)
            center = (ei_point_t) {(rect.top_left.x) + (rect.size.width) - radius, (rect.top_left.y) + radius};
        if (10 <= i && i < 20) center = (ei_point_t) {(rect.top_left.x) + radius, (rect.top_left.y) + radius};
        if (20 <= i && i < 30)
            center = (ei_point_t) {(rect.top_left.x) + radius, (rect.top_left.y) + (rect.size.height) - radius};
        if (30 <= i)
            center = (ei_point_t) {(rect.top_left.x) + (rect.size.width) - radius,
                                   (rect.top_left.y) + (rect.size.height) - radius};
        circle[i] = (ei_point_t) {(center.x) + (xpos * radius), (center.y) - (ypos * radius)};
    }
    //minimiser les erreurs d'arrondis
    circle[9].y = circle[10].y;
    circle[19].x = circle[20].x;
    circle[29].y = circle[30].y;
    circle[39].x = circle[0].x;
}
/**
 * Fonction qui ecrit dans lower_frame la partie basse du relief d'un bouton/frame
 * @param rounded_frame
 * @param rect
 * @param h
 * @param lower_frame
 */
void give_lower_frame(ei_point_t* rounded_frame,ei_rect_t rect, int h, ei_point_t* lower_frame){
    for (int i=0; i<15; i++){
        lower_frame[i] = rounded_frame[25+i];
    }
    for (int i=0;i<6;i++){
        lower_frame[15+i] = rounded_frame[i];
    }
    lower_frame[21] = (ei_point_t){rect.top_left.x +rect.size.width -h, rect.top_left.y + h};
    lower_frame[22] = (ei_point_t){rect.top_left.x + h                , rect.top_left.y + h};
}
/**
 * Fonction qui ecrit dans upper_frame la partie haute du relief d'un bouton/frame
 * @param rounded_frame
 * @param rect
 * @param h
 * @param upper_frame
 */
void give_upper_frame(ei_point_t* rounded_frame,ei_rect_t rect, int h, ei_point_t* upper_frame){
    for (int i=0; i<21; i++){
        upper_frame[i] = rounded_frame[5+i];
    }
    upper_frame[21] = (ei_point_t){rect.top_left.x + h                , rect.top_left.y + h};
    upper_frame[22] = (ei_point_t){rect.top_left.x +rect.size.width -h, rect.top_left.y + h};
}

/**
 * Donne le coin topleft pour placer un rectangle de texte de taille size_text
 * @param rect
 * @param anchor
 * @param size_text
 * @return
 */
ei_point_t place_text(ei_rect_t rect, ei_anchor_t anchor, ei_size_t size_text){
    switch (anchor){
        case ei_anc_northwest:
            return rect.top_left;
        case ei_anc_north:
            return (ei_point_t){rect.top_left.x + rect.size.width/2 - size_text.width/2, rect.top_left.y};
        case ei_anc_northeast:
            return (ei_point_t){rect.top_left.x + rect.size.width - size_text.width, rect.top_left.y};
        case ei_anc_west:
            return (ei_point_t){rect.top_left.x, rect.top_left.y + rect.size.height/2 - size_text.height/2};
        case ei_anc_none: case ei_anc_center:
            return (ei_point_t){rect.top_left.x + rect.size.width/2 - size_text.width/2,rect.top_left.y + rect.size.height/2 - size_text.height/2};
        case ei_anc_east:
            return (ei_point_t){rect.top_left.x + rect.size.width/2 - size_text.width/2,rect.top_left.y + rect.size.height/2 - size_text.height/2};
        case ei_anc_southwest:
            return (ei_point_t){rect.top_left.x,rect.top_left.y + rect.size.height - size_text.height};
        case ei_anc_south:
            return (ei_point_t){rect.top_left.x + rect.size.width/2 - size_text.width/2,rect.top_left.y + rect.size.height- size_text.height};
        case ei_anc_southeast:
            return (ei_point_t){rect.top_left.x + rect.size.width - size_text.width,rect.top_left.y + rect.size.height - size_text.height};
    }
}
