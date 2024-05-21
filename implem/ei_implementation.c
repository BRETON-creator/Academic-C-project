/**
 * @file	ei_implementation.h
 *
 * @brief 	Private definitions.
 * 
 */


#include <ei_widget_configure.h>
#include "ei_implementation.h"
#include "ei_draw.h"
#include "ei_event.h"
#include "var.h"
#include "hw_interface.h"
#include "ei_types.h"

ei_widget_t current_button_down = NULL;
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
    if (widget!=ei_app_root_widget() && (! widget->geom_params || ! widget->geom_params->manager)) return;
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
        supr_hierachy(frame->parent, frame);
        if (((ei_impl_frame_t*)frame)->text) free((((ei_impl_frame_t*)frame)->text));
        free((ei_impl_frame_t*)frame);
}


/**
 * \brief Fonction pour dessiner un widget frame.
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
    ei_rect_t new_clipper = get_rect_intersection(rect,*clipper);
    h = size.height < size.width ? size.height /2 : size.width /2;
    int border = ((ei_impl_frame_t*)widget)->border_size;

    ei_point_t rounded_frame[40];

    int radius;
    if (strcmp(widget->wclass->name,"button\0")==0) radius = ((ei_impl_button_t*)widget)->rayon;
    else radius = 0;

    give_rounded_frame(rounded_frame, rect, radius);
    ei_point_t upper_frame[23];
    ei_point_t lower_frame[23];
    ei_point_t smaller_frame[40];

    give_upper_frame(rounded_frame,rect,h,upper_frame);
    give_lower_frame(rounded_frame,rect,h,lower_frame);
    give_rounded_frame(smaller_frame,(ei_rect_t){(ei_point_t){rect.top_left.x+border,rect.top_left.y+border},
                                                 (ei_size_t){rect.size.width - 2*border, rect.size.height - 2*border}},radius-border);

    ei_color_t light_color  = (ei_color_t){color.red + 20, color.green +20, color.blue +20, color.alpha};

    ei_color_t dark_color  = (ei_color_t){abs(color.red -20), abs(color.green -20), abs(color.blue -20), color.alpha};
    switch (((ei_impl_frame_t*) widget)->frame_relief){
        case ei_relief_none:
            ei_draw_polygon(surface,lower_frame,23, color,&new_clipper); //
            ei_draw_polygon(surface,upper_frame,23,color,&new_clipper);
            break;
        case ei_relief_raised:
            ei_draw_polygon(surface,lower_frame,23, dark_color,&new_clipper);
            ei_draw_polygon(surface,upper_frame,23,light_color,&new_clipper);
            break;
        case ei_relief_sunken:

            ei_draw_polygon(surface,lower_frame,23, light_color,&new_clipper);
            ei_draw_polygon(surface,upper_frame,23,dark_color,&new_clipper);
            break;
    }

    if (((ei_impl_frame_t*)widget)->image){
            ei_surface_t surface_img = (((ei_impl_frame_t*)widget)->image);
            //ei_surface_t surface_img = hw_image_load("misc/klimt.jpg", ei_app_root_surface());
            ei_rect_t* rect_img = ((ei_impl_frame_t*)widget)->rect_image;

            hw_surface_lock(surface_img);
            ei_rect_t dst_rect = (ei_rect_t){{rect.top_left.x + border, rect.top_left.y + border},
                                             rect_img->size};
            ei_copy_surface(surface, &dst_rect, surface_img, &(ei_rect_t){{0,0},rect_img->size}, false);
            hw_surface_unlock(surface_img);
    }

    else ei_draw_polygon(surface,smaller_frame,40, color,&new_clipper);
    //on dessine sur la pick surface aussi. pour afficher la pick surface decommenter la ligne du dessous
    //ei_draw_polygon(surface,rounded_frame,40,*(widget->pick_color),&new_clipper);
    ei_draw_polygon(pick_surface,rounded_frame,40,*(widget->pick_color),&new_clipper);


    if (((ei_impl_frame_t*)widget)->text) {
        //printf("%s", ((ei_impl_frame_t*)widget)->text);
        ei_surface_t surface_text = hw_text_create_surface(((ei_impl_frame_t *) widget)->text,
                                                           ((ei_impl_frame_t *) widget)->text_font,
                                                           ((ei_impl_frame_t *) widget)->text_color);

        ei_point_t where = place_text(rect,((ei_impl_frame_t *) widget)->text_anchor, hw_surface_get_size(surface_text));
        if (((ei_impl_frame_t*)widget)->frame_relief == ei_relief_sunken)
            where = (ei_point_t){where.x,where.y + 2};
        ei_draw_text(surface, &where, ((ei_impl_frame_t *) widget)->text,
                     ((ei_impl_frame_t *) widget)->text_font, ((ei_impl_frame_t *) widget)->text_color,
                     &widget->screen_location);
        hw_surface_free(surface_text);
    }

    hw_surface_lock(surface);
}

/**
 * \brief Fonction pour mettre les valeurs par defauts d'un widget frame
 * TODO : completer cette fonction avec les bonnes valeures
 */
void ei_impl_setdefaults_frame(ei_widget_t widget){
    ei_impl_frame_t* frame = (ei_impl_frame_t*)widget;
    widget->destructor = NULL;
    frame->widget.wclass = ei_widgetclass_from_name((ei_const_string_t){"frame\0"});
    frame->widget.user_data = NULL;
    frame->widget.destructor = NULL;
    /* Widget Hierachy Management */
    frame->widget.parent = ei_app_root_widget();		///< Pointer to the parent of this widget.
    frame->widget.children_head=NULL;	///< Pointer to the first child of this widget.	Children are chained with the "next_sibling" field.
    frame->widget.children_tail=NULL;	///< Pointer to the last child of this widget.
    frame->widget.next_sibling=NULL;	///< Pointer to the next child of this widget's parent widget.

    /* Geometry Management */
    frame->widget.geom_params = (ei_geom_param_t){NULL};	///< Pointer to the geometry management parameters for this widget. If NULL, the widget is not currently managed and thus, is not displayed on the screen.
    frame->widget.requested_size=(ei_size_t){40,30} ;	///< See \ref ei_widget_get_requested_size.
    frame->widget.screen_location=(ei_rect_t){(ei_point_t){0,0},(ei_size_t){40,30}};///< See \ref ei_widget_get_screen_location.
    //frame->widget.content_rect;	///< See ei_widget_get_content_rect. By defaults, points to the screen_location.

    frame->frame_relief=0;
    frame->frame_color=ei_default_background_color; //default : noir
    frame->text=NULL;
    frame->text_font=ei_default_font;
    frame->text_size=ei_font_default_size;
    frame->text_color=ei_font_default_color;
    frame->text_anchor=ei_anc_center;
    frame->border_size=k_default_button_border_width;
    frame->image=NULL;
    frame->image_anchor=ei_anc_center;
    //frame->rect_image;
}

//======================================= button
void ei_impl_draw_button(ei_widget_t widget,ei_surface_t surface,ei_surface_t pick_surface,ei_rect_t* clipper){
    //dessin du cadre

    ei_impl_draw_frame(widget,surface,pick_surface,clipper);
    //dessin des attributs propres au button?
}



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
        if (current_button_down && button->pick_id == current_button_down->pick_id)
            current_button_down = NULL;
        supr_hierachy(button->parent, button);
        if (((ei_impl_frame_t*)button)->text) free((((ei_impl_frame_t*)button)->text));
        free((ei_impl_button_t*)button);
}

/**
* \brief Fonction pour mettre les valeurs par defauts d'un widget button
*/

void ei_impl_setdefaults_button(ei_widget_t widget){
        ei_impl_button_t* button = (ei_impl_button_t*)widget;
        ei_impl_setdefaults_frame(widget);
        widget->wclass =ei_widgetclass_from_name((ei_const_string_t){"button\0"});
        button->rayon = k_default_button_corner_radius;
        button->user_params=NULL;
        button->callback=NULL;
}

bool ei_callback_clickbutton(ei_widget_t		widget, struct ei_event_t*	event, ei_user_param_t	user_param){
    // cas ou on relache le clic en dehors du button
    if (current_button_down && event->type==ei_ev_mouse_buttonup && widget!=current_button_down){
        ((ei_impl_button_t*)current_button_down)->frame.frame_relief = ei_relief_raised;
        ei_impl_draw_button(current_button_down,ei_app_root_surface(), pick_surface,&current_button_down->parent->screen_location);
        current_button_down = NULL;
        return true;
    }
    if (!widget) return false;
    if (strcmp( widget->wclass->name, (ei_widgetclass_name_t){"button\0"}) != 0 ){
        return false; //Si le widget n'est pas un boutton on retourne false
    }else{
        switch (event->type)
        {
            case ei_ev_mouse_buttondown:
                //si on clique sur le bouton on modifie l'apparance du bouton up -> down
                if (((ei_impl_button_t*) widget)->frame.frame_relief ==  ei_relief_raised){
                    ((ei_impl_button_t*) widget)->frame.frame_relief = ei_relief_sunken;
                    ei_app_invalidate_rect(&widget->screen_location);
                    //il manque la modification de l'ancrage du texte
                    current_button_down = widget;
                }
                break;
            case ei_ev_mouse_buttonup:
                //si on relache le bouton on modifie l'apparance du bouton down -> up

                if (((ei_impl_frame_t*) widget)->frame_relief ==  ei_relief_sunken) {
                    ((ei_impl_frame_t*) widget)->frame_relief =  ei_relief_raised;
                    //il manque la modification de l'ancrage du texte
                    //et geom notify ? ou que pour redimension
                    ei_app_invalidate_rect(&widget->screen_location);

                    if (((ei_impl_button_t*)widget)->callback) ((ei_impl_button_t*)widget)->callback(widget,event,((ei_impl_button_t*)widget)->user_params);
                    current_button_down = NULL;
                }
                break;
            default:
                break;
        }
        return true;
    }
}

bool ei_callback_buttondown (ei_widget_t		widget, struct ei_event_t*	event, ei_user_param_t	user_param){
    if (!widget) return false;
    modify_hierarchy(widget,widget->parent);
    ei_app_invalidate_rect(&ei_app_root_widget()->screen_location);
    return false;
}
int size = 1;

void add_element(char** tab, int* size, const char* element) {
    tab = realloc(tab, *size* sizeof(char*));
    tab[*size] = strdup(element);
    (*size)++;
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
            event->param.key_code == SDLK_h )||
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
            event->param.key_code == SDLK_z)
        {
            ei_string_t letter = event->param.key_code;
            add_element( ((ei_impl_entry_t*)widget)->text , size , event->param.key_code );
        }

    }
}


void supr_hierachy(ei_widget_t widget, ei_widget_t widget_supr){
        if (!widget) return;
        ei_widget_t prec= widget->children_head;
        if (prec==widget_supr) {
            widget->children_head = widget->children_head->next_sibling;
            return;
        }else{
            ei_widget_t current = prec->next_sibling;
            while(current){
                if (current==widget_supr){
                    prec->next_sibling = current->next_sibling;
                    if (current == widget->children_tail) widget->children_tail = prec;
                    return;
                }
                prec=current;
                current=current->next_sibling;
            }
        }
}