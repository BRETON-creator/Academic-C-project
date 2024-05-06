/**
 * @file	ei_implementation.h
 *
 * @brief 	Private definitions.
 * 
 */


#include "ei_implementation.h"
#include "ei_draw.h"



/**
 * @brief	Draws the children of a widget.
 * 		The children are draw withing the limits of the clipper and
 * 		the widget's content_rect.
 *
 * @param	widget		The widget which children are drawn.
 * @param	surface		A locked surface where to draw the widget's children.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void		ei_impl_widget_draw_children	(ei_widget_t		widget,
						 ei_surface_t		surface,
						 ei_surface_t		pick_surface,
						 ei_rect_t*		clipper){
    (widget->wclass->drawfunc)(widget,surface,pick_surface,clipper);
    ei_widget_t child = widget->children_head;
    while (child){
        ei_impl_widget_draw_children(child, surface,pick_surface,&(widget->screen_location));
        child = child->next_sibling;
    }
}



/**
 * \brief	Converts the red, green, blue and alpha components of a color into a 32 bits integer
 * 		than can be written directly in the memory returned by \ref hw_surface_get_buffer.
 * 		The surface parameter provides the channel order.
 *
 * @param	surface		The surface where to store this pixel, provides the channels order.
 * @param	color		The color to convert.
 *
 * @return 			The 32 bit integer corresponding to the color. The alpha component
 *				of the color is ignored in the case of surfaces that don't have an
 *				alpha channel.
 */
uint32_t	ei_impl_map_rgba(ei_surface_t surface, ei_color_t color);


//================================================================================================
/**
 *  \brief fonction pour alloué un espace pour un widget frame.
 *
 * @return un espace assez grand pour stocker un widget de classe frame.
 */
ei_widget_t ei_impl_alloc_frame(){
    return calloc(1,sizeof(ei_impl_frame_t));
}


/**
 * \brief Fonction pour free un espace alloué a un widget frame.
 *
 */
void ei_impl_release_frame(ei_widget_t frame){
    free((ei_impl_frame_t*)frame);
}

/**
 * \brief Fonction pour dessiner un widget frame.
 * TODO : dessiner correctement le frame
 * dans la surface de la fenetre root ou de la fenetre du parent ?
 * doit trouver le point ou on doit placer le frame
 * depend du point d'ancrage
 * dessine le relief au bon endroit
 * dessine le frame au bon endroit
 * pose le texte et l'image au bon endroit
 *
 */
void ei_impl_draw_frame(ei_widget_t widget,ei_surface_t surface,ei_surface_t pick_surface,ei_rect_t* clipper){
    hw_surface_unlock(surface);
    int h;
    ei_color_t color  = ((ei_impl_frame_t*)widget)->frame_color;
    ei_size_t size= widget->requested_size;
    ei_rect_t rect= widget->screen_location;
    h = size.height < size.width ? size.height /2 : size.width /2;
    int border = 0.05*h; // il faut definir border size dans widgetclass frame pour pouvoir controller la largeur du relief

    ei_point_t point_array[4] = {{rect.top_left.x +border, rect.top_left.y + border},
                                 {rect.top_left.x - border + size.width, rect.top_left.y + border},
                                 {rect.top_left.x + size.width -border, rect.top_left.y + size.height - border},
                                 {rect.top_left.x + border, rect.top_left.y + size.height - border}};
    //Pour créer du relief on dessine les deux moitiés de rectangle l'une plus claire et l'autre plus sombre, et par dessus on dessine le rectangle


    ei_point_t point_array_light[5] = {{rect.top_left.x , rect.top_left.y + size.height},
                                 {rect.top_left.x+ h , rect.top_left.y + size.height - h },
                                 {rect.top_left.x + size.width -h , rect.top_left.y + size.height - h},
                                 {rect.top_left.x+ size.width  , rect.top_left.y},
                                 {rect.top_left.x , rect.top_left.y}  };

    ei_point_t point_array_dark[5] = {{rect.top_left.x , rect.top_left.y + size.height},
                                 {rect.top_left.x+ h , rect.top_left.y + h },
                                 {rect.top_left.x + size.width -h , rect.top_left.y +h },
                                 {rect.top_left.x + size.width  , rect.top_left.y },
                                 {rect.top_left.x + size.width, rect.top_left.y + size.height}  };

    ei_color_t light_color  = ei_default_background_color;
    light_color.blue = color.blue + 20;
    light_color.green = color.green + 20;
    light_color.red = color.red + 20;

    ei_color_t dark_color  = ei_default_background_color;
    dark_color.blue = color.blue - 20;
    dark_color.green = color.green - 20;
    dark_color.red = color.red - 20;

    switch (((ei_impl_frame_t*) widget)->frame_relief){
        case ei_relief_none:
            ei_draw_polygon(surface,point_array_dark,5, color,clipper); //
            ei_draw_polygon(surface,point_array_light,5,color,clipper);
            break;
        case ei_relief_raised:
            ei_draw_polygon(surface,point_array_dark,5, dark_color,clipper);
            ei_draw_polygon(surface,point_array_light,5,light_color,clipper);
            break;
        case ei_relief_sunken:
            ei_draw_polygon(surface,point_array_dark,5, light_color,clipper);
            ei_draw_polygon(surface,point_array_light,5,dark_color,clipper);
            break;
    }

    ei_draw_polygon(surface,point_array,4, color,clipper);


    ei_surface_t surfacetext;
    //surfacetext = hw_text_create_surface(((ei_impl_frame_t*)widget)->text,((ei_impl_frame_t*)widget)->text_font,((ei_impl_frame_t*)widget)->text_color);
    hw_surface_update_rects(surface,NULL);
    //hw_surface_update_rects(surfacetext,NULL);
    hw_surface_lock(surface);
}



//(surfacetext,where,((ei_impl_frame_t*)widget)->text,((ei_impl_frame_t*)widget)->text_font,
//                 ((ei_impl_frame_t*)widget)->text_color,clippertext)



/**
 * \brief Fonction pour mettre les valeurs par defauts d'un widget frame
 * TODO : completer cette fonction avec les bonnes valeures
 */
void ei_impl_setdefaults_frame(ei_widget_t widget){
    ei_impl_frame_t* frame = (ei_impl_frame_t*)widget;
    frame->widget.wclass = ei_widgetclass_from_name((ei_const_string_t){"frame\0"});
    //frame->widget.pick_id;
    //frame->widget.pick_color;
    frame->widget.user_data = NULL;
    frame->widget.destructor = NULL;
    /* Widget Hierachy Management */
    frame->widget.parent = ei_app_root_widget();		///< Pointer to the parent of this widget.
    frame->widget.children_head=NULL;	///< Pointer to the first child of this widget.	Children are chained with the "next_sibling" field.
    frame->widget.children_tail=NULL;	///< Pointer to the last child of this widget.
    frame->widget.next_sibling=NULL;	///< Pointer to the next child of this widget's parent widget.

    /* Geometry Management */
    frame->widget.geom_params = NULL;	///< Pointer to the geometry management parameters for this widget. If NULL, the widget is not currently managed and thus, is not displayed on the screen.
    frame->widget.requested_size=(ei_size_t){100,100} ;	///< See \ref ei_widget_get_requested_size.
    //frame->widget.screen_location;///< See \ref ei_widget_get_screen_location.
    //frame->widget.content_rect;	///< See ei_widget_get_content_rect. By defaults, points to the screen_location.

    frame->frame_relief=0;
    frame->frame_color=ei_default_background_color; //default : noir
    frame->text=NULL;
    frame->text_font=ei_default_font;
    frame->text_size=ei_font_default_size;
    frame->text_color=ei_font_default_color;
    frame->text_anchor=ei_anc_center;
    frame->image=NULL;
    frame->image_anchor=ei_anc_center;
    //frame->rect_image;
}

void give_rounded_frame(ei_point_t* circle, ei_rect_t rect, int radius) {
    float pi = 3.14;
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
}

void give_lower_frame(ei_point_t* rounded_frame,ei_rect_t rect, int h, ei_point_t* lower_frame){
    for (int i=0; i<15; i++){
        lower_frame[i] = rounded_frame[25+i];
    }
    for (int i=0;i<5;i++){
        lower_frame[15+i] = rounded_frame[i];
    }
    lower_frame[20] = (ei_point_t){rect.top_left.x +rect.size.width -h, rect.top_left.y + h};
    lower_frame[21] = (ei_point_t){rect.top_left.x + h                , rect.top_left.y + h};
}

void give_upper_frame(ei_point_t* rounded_frame,ei_rect_t rect, int h, ei_point_t* upper_frame){
    for (int i=0; i<20; i++){
        upper_frame[i] = rounded_frame[5+i];
    }
    upper_frame[20] = (ei_point_t){rect.top_left.x + h                , rect.top_left.y + h};
    upper_frame[21] = (ei_point_t){rect.top_left.x +rect.size.width -h, rect.top_left.y + h};
}

void ei_impl_draw_button(ei_widget_t widget,ei_surface_t surface,ei_surface_t pick_surface,ei_rect_t* clipper){
    hw_surface_unlock(surface);
    int h;
    ei_color_t color  = ((ei_impl_button_t*)widget)->frame.frame_color;
    ei_size_t size= widget->requested_size;
    ei_rect_t rect= widget->screen_location;
    h = size.height < size.width ? size.height /2 : size.width /2;
    int border = 0.05*h; // il faut definir border size dans widgetclass frame pour pouvoir controller la largeur du relief

    ei_point_t rounded_frame[40];
    int radius= ((ei_impl_button_t*)widget)->rayon;
    give_rounded_frame(rounded_frame, rect, radius);
    ei_point_t upper_frame[22];
    ei_point_t lower_frame[22];
    ei_point_t smaller_frame[40];

    give_upper_frame(rounded_frame,rect,h,upper_frame);
    give_lower_frame(rounded_frame,rect,h,lower_frame);
    give_rounded_frame(smaller_frame,(ei_rect_t){(ei_point_t){rect.top_left.x+border,rect.top_left.y+border},
                                                            (ei_size_t){rect.size.width - 2*border, rect.size.height - 2*border}},radius);

    ei_color_t light_color  = (ei_color_t){color.red + 20, color.green +20, color.blue +20, color.alpha};

    ei_color_t dark_color  = (ei_color_t){color.red -20, color.green -20, color.blue -20, color.alpha};
    switch (((ei_impl_frame_t*) widget)->frame_relief){
        case ei_relief_none:
            ei_draw_polygon(surface,lower_frame,22, color,clipper); //
            ei_draw_polygon(surface,upper_frame,22,color,clipper);
            break;
        case ei_relief_raised:
            ei_draw_polygon(surface,lower_frame,22, dark_color,clipper);
            ei_draw_polygon(surface,upper_frame,22,light_color,clipper);
            break;
        case ei_relief_sunken:

            ei_draw_polygon(surface,lower_frame,22, light_color,clipper);
            ei_draw_polygon(surface,upper_frame,22,dark_color,clipper);
            break;
    }

    //ei_draw_polygon(surface,rounded_frame,40,light_color,clipper);
    ei_draw_polygon(surface,smaller_frame,40, color,clipper);

    hw_surface_update_rects(surface,NULL);
    //ei_surface_t surfacetext = hw_text_create_surface(((ei_impl_frame_t*)widget)->text,((ei_impl_frame_t*)widget)->text_font,((ei_impl_frame_t*)widget)->text_color);
    //hw_surface_update_rects(surfacetext,NULL);
    hw_surface_lock(surface);
}

//===================================== placer

/**
 * @brief   Fonction run geometrymanager de PLACER
 */
void ei_impl_placer_runfunc(ei_widget_t widget){}

/**
 * @brief Release function of placer
 */
void  ei_impl_placer_releasefunc(ei_widget_t widget){}

//======================================= button

/**
 *  \brief fonction pour alloué un espace pour un widget button.
 *
 * @return un espace assez grand pour stocker un widget de classe button.
 */
ei_widget_t ei_impl_alloc_button(){
        return calloc(1,sizeof(ei_impl_button_t));
}


/**
 * \brief Fonction pour free un espace alloué a un widget button.
 *
 */
void ei_impl_release_button(ei_widget_t button){
        free((ei_impl_button_t*)button);
}

/**
* \brief Fonction pour mettre les valeurs par defauts d'un widget button
* TODO : completer cette fonction avec les bonnes valeures
*/

void ei_impl_setdefaults_button(ei_widget_t widget){
        ei_impl_button_t* button = (ei_impl_button_t*)widget;
        button->frame.widget.wclass = ei_widgetclass_from_name((ei_const_string_t){"button\0"});
        //button->widget.pick_id;
        //button->widget.pick_color;
        button->frame.widget.user_data = NULL;
        button->frame.widget.destructor = NULL;
        /* Widget Hierachy Management */
        button->frame.widget.parent = ei_app_root_widget();		///< Pointer to the parent of this widget.
        button->frame.widget.children_head=NULL;	///< Pointer to the first child of this widget.	Children are chained with the "next_sibling" field.
        button->frame.widget.children_tail=NULL;	///< Pointer to the last child of this widget.
        button->frame.widget.next_sibling=NULL;	///< Pointer to the next child of this widget's parent widget.

        /* Geometry Management */
        button->frame.widget.geom_params = NULL;	///< Pointer to the geometry management parameters for this widget. If NULL, the widget is not currently managed and thus, is not displayed on the screen.
        button->frame.widget.requested_size=(ei_size_t){100,100} ;	///< See \ref ei_widget_get_requested_size.
        //button->widget.screen_location;///< See \ref ei_widget_get_screen_location.
        //button->widget.content_rect;	///< See ei_widget_get_content_rect. By defaults, points to the screen_location.

        button->frame.frame_relief=ei_relief_raised;
        button->frame.frame_color=ei_default_background_color; //default : noir
        button->frame.text=NULL;
        button->frame.text_font=ei_default_font;
        button->frame.text_size=ei_font_default_size;
        button->frame.text_color=ei_font_default_color;
        button->frame.text_anchor=ei_anc_center;
        button->frame.image=NULL;
        button->frame.image_anchor=ei_anc_center;
        //button->rect_image;

        button->rayon = 30;
        button->user_params=NULL;
}



