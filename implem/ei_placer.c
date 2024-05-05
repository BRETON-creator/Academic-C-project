/**
 *  @file	ei_placer.h
 *  @brief	Manages the positioning and sizing of widgets on the screen.
 *
 *  \author
 *  Created by François Bérard on 18.12.11.
 *  Copyright 2011 Ensimag. All rights reserved.
 *
 */



#include "ei_placer.h"
#include "ei_types.h"
#include "ei_widget.h"
#include "ei_implementation.h"


/**
 * \brief	Configures the geometry of a widget using the "placer" geometry manager.
 *
 *		The placer computes a widget's geometry relative to its parent *content_rect*.
 *
 * 		If the widget was already managed by the "placer", then this calls simply updates
 *		the placer parameters: arguments that are not NULL replace previous values.
 *		Arguments that are NULL don't change the current value of the parameter.
 *
 * 		When the arguments are passed as NULL, the placer uses default values (detailed in
 *		the argument descriptions below). If no size is provided (either absolute or
 *		relative), then either the requested size of the widget is used if one was provided,
 *		or the default size is used.
 *
 * @param	widget		The widget to place.
 * @param	anchor		How to anchor the widget to the position defined by the placer
 *				(defaults to ei_anc_northwest).
 * @param	x		The absolute x position of the widget (defaults to 0).
 * @param	y		The absolute y position of the widget (defaults to 0).
 * @param	width		The absolute width for the widget (defaults to the requested width or
 * 				the default width of the widget if rel_width is NULL, or 0 otherwise).
 * @param	height		The absolute height for the widget (defaults to the requested height or
 *				the default height of the widget if rel_height is NULL, or 0 otherwise).
 * @param	rel_x		The relative x position of the widget: 0.0 corresponds to the left
 *				side of the parent, 1.0 to the right side (defaults to 0.0).
 * @param	rel_y		The relative y position of the widget: 0.0 corresponds to the top
 *				side of the parent, 1.0 to the bottom side (defaults to 0.0).
 * @param	rel_width	The relative width of the widget: 0.0 corresponds to a width of 0,
 *				1.0 to the width of the parent (defaults to 0.0).
 * @param	rel_height	The relative height of the widget: 0.0 corresponds to a height of 0,
 *				1.0 to the height of the parent (defaults to 0.0).
 */
void		ei_place	(ei_widget_t		widget,
				 ei_anchor_t*		anchor,
				 int*			x,
				 int*			y,
				 int*			width,
				 int*			height,
				 float*			rel_x,
				 float*			rel_y,
				 float*			rel_width,
				 float*			rel_height){

        /**
         * ce qu'on veut de cette fonction : Qu'elle mette a jour correctement ces champs la de notre widget .
         * ei_size_t		requested_size;	///< See \ref ei_widget_get_requested_size.
         * ei_rect_t		screen_location;///< See \ref ei_widget_get_screen_location.
	     * ei_rect_t*		content_rect;	///< See ei_widget_get_content_rect. By defaults, points to the screen_location.
         *
         * pour :
         *  - requested_size : la taille de notre widget d'apres width et height ?
         *  - screen_location : le rectangle definissant l'emplacement de notre widget: possede deux champs :
         *      - top_left : ei_point_t {x,y} : coin haut gauche du rectangle, prenant en compte l'ancrage
         *      - size : {width, height} : taille du rectangle, prenant en compte ce qui est possible de faire
         *  - content_rect : g pas compris =QUESTION POUR LES ENCADRANTS!
         *
         *  place le widget:  - soit relativement a ses parents :widget->parent (avec les parametre rel)
         *                    - soit avec les valeurs absolue (x, y, etc)
         *
         *  cette fonction sera appelée dans les drawfunc (fonction a refaire au passage).
         */

        //if (rel_height) widget->requested_size.height = (widget->parent->requested_size.height) * (*rel_height);
        //if (rel_width) widget->requested_size.width = (widget->parent->requested_size.width) * (*rel_width);
        if (width) {
            widget->screen_location.size.width = *width;
            widget->requested_size.width = *width;
        }
        if (height) {
            widget->requested_size.height= *height;
            widget->screen_location.size.height = *height;
        }

        //calculer x et y en fonction de l'ancrage :

        int xpos = -1, ypos = -1;
        if (rel_x)
            xpos = (widget->requested_size.width) * (*rel_x);
        if (rel_y)
            ypos = (widget->requested_size.height) * (*rel_y);
        if (x) xpos = *x;
        if (y) ypos = *y;

        ei_anchor_t anc;
        if (!anchor) anc= ei_anc_northwest;
        else anc = *anchor;
        switch (anc){
            case ei_anc_northwest:
                xpos = xpos;
                ypos = ypos;
                break;
            case ei_anc_north:
                xpos = xpos-(widget->requested_size.width)/2;
                ypos = ypos;
                break;
            case ei_anc_northeast:
                xpos = xpos - widget->requested_size.width;
                ypos = ypos;
                break;
            case ei_anc_west:
                xpos = xpos;
                ypos = ypos - widget->requested_size.height /2;
                break;
            case ei_anc_center:
                xpos = xpos-(widget->requested_size.width)/2;
                ypos = ypos - widget->requested_size.height /2;
                break;
            case ei_anc_east:
                xpos = xpos - widget->requested_size.width;
                ypos = ypos - widget->requested_size.height /2;
                break;
            case ei_anc_southwest:
                xpos = xpos;
                ypos = ypos - widget->requested_size.height;
                break;
            case ei_anc_south:
                xpos = xpos - widget->requested_size.width / 2;
                ypos = ypos - widget->requested_size.height;
                break;
            case ei_anc_southeast:
                xpos = xpos - widget->requested_size.width;
                ypos = ypos - widget->requested_size.height;
                break;
        }

        if (xpos!= -1) widget->screen_location.top_left.x = xpos;
        if (ypos!= -1) widget->screen_location.top_left.y = ypos;


}

