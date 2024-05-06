/**
* @file	ei_offscreen.c
 *
 * @brief 	File for drawing of pick_surface and picking management
 *
 */

#include "ei_types.h"
#include "ei_widgetclass.h"
#include "ei_implementation.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

uint32_t id_counter = 0;

// Fonction pour générer un nouvel identifiant = entier de 32 bits décomposés en 4 octets (rgb + alpha)

uint32_t create_new_pick_id(void){
    uint8_t red, blue, green, transparancy;
    do {
        red = rand() % 256;
        blue = rand() % 256;
        green = rand() % 256;
        transparancy = 255;
    } while (red > 200 || (blue > 200) ||  (green> 200));

    uint32_t identifiant = (transparancy << 24) | (green << 16) | (blue << 8) | red;

    return identifiant;
}

//génére une couleur à partir de l'id du widget

ei_color_t generate_color(uint32_t pick_id) {
    srand(time(NULL)); // Initialiser le générateur de nombres aléatoires
    ei_color_t color= ei_font_default_color;

    uint8_t red = (pick_id >> 24) & 0xFF;
    uint8_t blue = (pick_id >> 16) & 0xFF;
    uint8_t green = (pick_id >> 8) & 0xFF;
    uint8_t transparancy = pick_id & 0xFF;

    color.red = red;
    color.blue = blue;
    color.green = green;
    color.alpha = transparancy;
    return color;

}

/**
 * @brief	Function that gives a color to a new created widget that has not been given by a previous widget
 *
 * @param	widget		The widget that is going to be destroyed.

 *
 */


void pick_surface_draw(ei_surface_t pick_surface , ei_widget_t widget ){
    ei_rect_t rect= widget->screen_location;
    ei_point_t origin = rect.top_left;
    hw_surface_set_origin(pick_surface, origin);
    *(widget-> pick_color) = generate_color(widget->pick_id);

}

