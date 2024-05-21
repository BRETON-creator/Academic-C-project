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


void ei_impl_geomnotify_entry(ei_widget_t widget){

}

/*On crée une structure pour le stockage des lettres*/

typedef struct letter {
    char data;
    struct letter* next;
} letter;

// Pour créer une nouvelle lettre
letter* create_letter(char data) {
    letter* new_letter = (letter*)malloc(sizeof(letter));
    new_letter->data = data;
    new_letter->next = NULL;
    return new_letter;
}

// Fonction pour insérer une lettre à une position donnée par le curseur
void insert_at_cursor_position(letter** tab, int position, char data) {
    letter* new_letter = create_letter(data);
    if (position == 0) {
        new_letter->next = *tab;
        *tab = new_letter;
        return;
    }

    letter* current = *tab;
    for (int i = 0; i < position - 1 && current != NULL; i++) {
        current = current->next;
    }
    new_letter->next = current->next;
    current->next = new_letter;
}

// Fonction pour supprimer un élément à une position donnée par le curseur
void delete_at_cursor_position(letter** tab, int position) {

    letter* tmp = *tab;

    if (position == 0) {
        *tab = tmp->next;
        free(tmp);
        return;
    }

    letter* current = *tab;
    for (int i = 0; i < position - 1 && current != NULL; i++) {
        current = current->next;
    }
    letter* next = current->next->next;
    free(current->next);
    current->next = next;
}


bool ei_callback_entry(ei_widget_t		widget, struct ei_event_t*	event, ei_user_param_t	user_param)
{
    if (!widget) return false;
    if (strcmp( widget->wclass->name, (ei_widgetclass_name_t){"entry\0"}) != 0 ){
        return false; //Si le widget n'est pas un entry on retourne false
    }
    if (event->type == ei_ev_mouse_buttondown){
        ei_app_invalidate_rect(&widget->screen_location);
        ei_point_t where_cursor;
        ei_point_t where;
        where_cursor = where;
        char* tmp;
        tmp = ((ei_impl_entry_t*)widget)->text;
    }
    if (event->type == ei_ev_keydown)
    {
     // switch ((event->type == ei_ev_keydown) && (event->param.key_code == SDLK_ESCAPE)
        switch ((event->type == ei_ev_keydown) && (event->param.key_code == SDLK_a ||
            event->param.key_code == SDLK_b ||
            event->param.key_code == SDLK_c ||
            event->param.key_code == SDLK_d ||
            event->param.key_code == SDLK_e ||
            event->param.key_code == SDLK_f ||
            event->param.key_code == SDLK_g ||
            event->param.key_code == SDLK_h ||
            event->param.key_code == SDLK_i ||
            event->param.key_code == SDLK_j ||
            event->param.key_code == SDLK_k ||
            event->param.key_code == SDLK_l ||
            event->param.key_code == SDLK_m ||
            event->param.key_code == SDLK_n ||
            event->param.key_code == SDLK_o ||
            event->param.key_code == SDLK_p ||
            event->param.key_code == SDLK_q ||
            event->param.key_code == SDLK_r ||
            event->param.key_code == SDLK_s ||
            event->param.key_code == SDLK_t ||
            event->param.key_code == SDLK_u ||
            event->param.key_code == SDLK_v ||
            event->param.key_code == SDLK_w ||
            event->param.key_code == SDLK_x ||
            event->param.key_code == SDLK_y ||
            event->param.key_code == SDLK_z)){

            ei_string_t letter = event->param.key_code;
            insert_at_cursor_position( ((ei_impl_entry_t*)widget)->text , ((ei_impl_entry_t*)widget)->position , letter);
        }

    }
}



/**
 * \brief Fonction pour dessiner un widget entry.
 * On suppose qu'a chaque evenement, on met à jour le texte à afficher
 * la fonction dessine simplement un carré blanc avec le texte à afficher + curseur
 *
 */

void ei_impl_draw_entry(ei_widget_t widget,ei_surface_t surface,ei_surface_t pick_surface,ei_rect_t* clipper){
    hw_surface_unlock(surface);
    int h;
    ei_color_t white  = {0xff , 0xff , 0xff , 0xff };
    ei_color_t black  = {0x00 , 0x00 , 0x00 , 0xff };
    ei_color_t bg_color= ei_default_background_color;

    ei_size_t size= widget->requested_size;
    ei_rect_t rect= widget->screen_location;
    ei_rect_t new_clipper = get_rect_intersection(rect,*clipper);
    h = size.height < size.width ? size.height /2 : size.width /2;
    int border = ((ei_impl_entry_t*)widget)->border_size;

    ei_point_t white_frame[4] = { (ei_point_t){rect.top_left.x+border,rect.top_left.y+border},
                    (ei_point_t){rect.top_left.x+border,rect.top_left.y+ size.height + border},
                    (ei_point_t){rect.top_left.x+border + size.width,rect.top_left.y+border},
                    (ei_point_t){rect.top_left.x+border + size.width,rect.top_left.y+border+ size.height}};

    ei_point_t bigger_frame[4] = { (ei_point_t){rect.top_left.x,rect.top_left.y},
                    (ei_point_t){rect.top_left.x,rect.top_left.y+ size.height },
                    (ei_point_t){rect.top_left.x + size.width,rect.top_left.y},
                    (ei_point_t){rect.top_left.x + size.width,rect.top_left.y + size.height}};

    ei_draw_polygon(surface,white_frame,4, white,&new_clipper);
    ei_draw_polygon(surface,white_frame,4, bg_color ,&new_clipper);
    ei_draw_polygon(pick_surface,bigger_frame,4,*(widget->pick_color),&new_clipper);

    if (((ei_impl_frame_t*)widget)->text) {
        //printf("%s", ((ei_impl_frame_t*)widget)->text);
        ei_surface_t surface_text = hw_text_create_surface(((ei_impl_frame_t *) widget)->text,
                                                           ((ei_impl_frame_t *) widget)->text_font,
                                                           ((ei_impl_frame_t *) widget)->text_color);

        ei_point_t where = place_text(rect,((ei_impl_frame_t *) widget)->text_anchor, hw_surface_get_size(surface_text));
        ei_draw_text(surface, &where, ((ei_impl_frame_t *) widget)->text,
                     ((ei_impl_frame_t *) widget)->text_font, ((ei_impl_frame_t *) widget)->text_color,
                     &widget->screen_location);
        hw_surface_free(surface_text);
    }

    hw_surface_lock(surface);
}