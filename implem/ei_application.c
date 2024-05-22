/**
 *  @file	ei_application.c
 *  @brief	Implémentation des fonctions qui
 *          Manages the main steps of a graphical application: initialization, main window,
 *		    main loop, quitting, resource freeing.
 *
 */

#include <stdio.h>
#include "ei_application.h"
#include "ei_widgetclass.h"
#include "ei_implementation.h"
#include "ei_implementation_toplevel.h"
#include "ei_implementation_entry.h"
#include "ei_implementation_entry.h"
#include "ei_draw.h"
#include "ei_event.h"
#include "ei_impl_binds.h"
#include "ei_impl_placer.h"
#include "var.h"

//variable globale designant la surface offscreen de picking
ei_surface_t pick_surface;
//variable globale designant le widget root
ei_impl_widget_t* root = NULL;
//variable globale qui indique lorsque l'on quitte le programme (elle sera mis a jour dans ei_app_quit_request)
bool quit              = false;
//variable globale designant la surface root
ei_surface_t root_surface;
//variable globale designant la liste chainée des rectangles a update sur la root_surface
ei_linked_rect_t* rects = NULL;



/**
 * \brief	Creates an application.
 *		<ul>
 *			<li> initializes the hardware (calls \ref hw_init), </li>
 *			<li> registers all classes of widget and all geometry managers, </li>
 *			<li> creates the root window (either in a system window, or the entire screen), </li>
 *			<li> creates the root widget to access the root window. </li>
 *		</ul>
 *
 * @param	main_window_size	If "fullscreen" is false, the size of the root window of the
 *					application.
 *					If "fullscreen" is true, the current monitor resolution is
 *					used as the size of the root window. \ref hw_surface_get_size
 *					can be used with \ref ei_app_root_surface to get the size.
 * @param	fullScreen		If true, the root window is the entire screen. Otherwise, it
 *					is a system window.
 */
void ei_app_create(ei_size_t main_window_size, bool fullscreen){

    // initializes the hardware (calls \ref hw_init)
    hw_init();

    // registers all classes of widget and all geometry managers

    //      register frame class of widget
    ei_widgetclass_t* frameclass = calloc(1,sizeof(ei_widgetclass_t));
    frameclass->allocfunc        = ei_impl_alloc_frame;
    frameclass->releasefunc      = ei_impl_release_frame;
    frameclass->drawfunc         = ei_impl_draw_frame;
    frameclass->setdefaultsfunc  = ei_impl_setdefaults_frame;
    frameclass->geomnotifyfunc   = NULL;
    frameclass->next             = NULL;
    strcpy(frameclass->name,(ei_widgetclass_name_t){"frame\0"});
    ei_widgetclass_register(frameclass);

    //      register button class of widget
    ei_widgetclass_t* buttonclass = calloc(1,sizeof(ei_widgetclass_t));
    buttonclass->allocfunc        = ei_impl_alloc_button;
    buttonclass->releasefunc      = ei_impl_release_button;
    buttonclass->drawfunc         = ei_impl_draw_button;
    buttonclass->setdefaultsfunc  = ei_impl_setdefaults_button;
    buttonclass->geomnotifyfunc   = NULL;
    buttonclass->next             = NULL;
    strcpy(buttonclass->name,(ei_widgetclass_name_t){"button\0"});
    ei_widgetclass_register(buttonclass);

    //      register toplevel class of widget
    ei_widgetclass_t* toplevelclass = calloc(1,sizeof(ei_widgetclass_t));
    toplevelclass->allocfunc        = ei_impl_alloc_toplevel;
    toplevelclass->releasefunc      = ei_impl_release_toplevel;
    toplevelclass->drawfunc         = ei_impl_draw_toplevel;
    toplevelclass->setdefaultsfunc  = ei_impl_setdefaults_toplevel;
    toplevelclass->geomnotifyfunc   = ei_impl_geomnotify_toplevel;
    toplevelclass->next             = NULL;
    strcpy(toplevelclass->name,(ei_widgetclass_name_t){"toplevel\0"});
    ei_widgetclass_register(toplevelclass);

    //      register entry class of widget
    ei_widgetclass_t* entryclass = calloc(1,sizeof(ei_widgetclass_t));
    entryclass->allocfunc        = ei_impl_alloc_entry;
    entryclass->releasefunc      = ei_impl_release_entry;
    entryclass->drawfunc         = ei_impl_draw_entry;
    entryclass->setdefaultsfunc  = ei_impl_setdefaults_entry;
    entryclass->geomnotifyfunc   = ei_impl_geomnotify_entry;
    entryclass->next             = NULL;
    strcpy(entryclass->name,(ei_widgetclass_name_t){"entry\0"});
    ei_widgetclass_register(entryclass);

    //      register geometry manager "placer"
    ei_geometrymanager_t* placer = calloc(1,sizeof(ei_geometrymanager_t));
    placer->runfunc              = ei_impl_placer_runfunc;
    placer->releasefunc          = ei_impl_placer_releasefunc;
    strcpy(placer->name,"placer\0");
    ei_geometrymanager_register(placer);

    // creates the root window (either in a system window, or the entire screen
    ei_surface_t main_window= hw_create_window(main_window_size,fullscreen);
    hw_surface_lock(main_window);


    // creates the root widget to access the root window.
    root                                    = ei_impl_alloc_frame();

    ei_impl_setdefaults_frame(root);
    root->pick_id=0x000000FF;
    root->pick_color=calloc(1,sizeof(ei_color_t));
    root->pick_color->red = 0x00; root->pick_color->green = 0x00; root->pick_color->blue=0x00; root->pick_color->alpha=0xFF;
    root->parent=NULL;
    root->requested_size                    = main_window_size;
    ((ei_impl_frame_t*)root)->frame_relief  = ei_relief_none;
    root->screen_location                   = hw_surface_get_rect(main_window);
    root->requested_size                    = root->screen_location.size;
    root_surface                            = main_window;
    pick_surface                            = hw_surface_create(root_surface, hw_surface_get_size(main_window),false);
}

/**
 * \brief	Releases all the resources of the application, and releases the hardware
 *		(ie. calls \ref hw_quit).
 */
void ei_app_free(void){
    //TODO
    /*
     * doit release :
     * - toutes les widgets class
     * - tous les geometrymanager
     * - la surface offscreen (hw_surface_free)
     * - tous les widgets (ei_widget_destroy(root))
     * - ?
     *
    */

    ei_widget_destroy(root);

    //free les widgets_class (on en a que 3...
    ei_widgetclass_t *tmp = ei_widgetclass_from_name("toplevel\0");
    ei_widgetclass_t *suiv= tmp->next;
    while (suiv){
        free(tmp);
        tmp=suiv;
        suiv=suiv->next;
    }
    free(tmp);
    //free les geometrymanager (on en a qu'un pour le moment...
    free(ei_geometrymanager_from_name("placer\0"));
    //on supprime nos 2 surfaces
    hw_surface_free(root_surface);
    hw_surface_free(pick_surface);
    //on supprime les binds qui restent
    ei_bind_t* current = ei_get_head_binds();
    ei_bind_t* tmp_bind;
    while (current){
        tmp_bind = current->next_bind;
        if(!current->bind_isWidget)
            free(current->object.tag);
        free(current);
        current= tmp_bind;
    }
    hw_quit();
}

/**
 * @brief free la liste chainée des rectangles à update !
 * @param list la liste chainée des rectangles
 */
void release_linked_rect(ei_linked_rect_t* list){
    ei_linked_rect_t *tmp;
    while(list){
        tmp=list->next;
        free(list);
        list=tmp;
    }
}


/**
 * \brief	Runs the application: enters the main event loop. Exits when
 *		\ref ei_app_quit_request is called.
 */
void ei_app_run(void){

    ei_rect_t clipper           = hw_surface_get_rect(ei_app_root_surface());
    ei_impl_widget_draw_children(root, root_surface, pick_surface, &clipper);
    hw_surface_unlock(root_surface);
    //boucle principale

    //binds interns
    ei_bind(ei_ev_mouse_buttondown, NULL,"button\0",ei_callback_clickbutton,NULL);
    ei_bind(ei_ev_mouse_buttonup,NULL,"button\0",ei_callback_clickbutton,NULL);

    ei_bind(ei_ev_mouse_buttondown, NULL, "all\0", ei_callback_buttondown,NULL);

    ei_bind(ei_ev_mouse_buttondown, NULL,"toplevel\0",ei_callback_toplevel,NULL);
    ei_bind(ei_ev_mouse_move, NULL,"toplevel\0",ei_callback_toplevel,NULL);
    ei_bind(ei_ev_mouse_buttonup, NULL,"toplevel\0",ei_callback_toplevel,NULL);

    ei_bind(ei_ev_mouse_buttondown, NULL,"entry\0",ei_callback_entry,NULL);
    ei_bind(ei_ev_keydown, NULL,"entry\0",ei_callback_entry,NULL);

    ei_event_t* event = calloc(1,sizeof(ei_event_t));
    ei_bind_t* bind;
    ei_bind_t* binds;
    bool change_event = true;
    while(!quit){
        binds=ei_get_head_binds();
        hw_event_wait_next(event);
        change_event = true;
        do {
            bind = ei_bind_from_event(event,binds); // on cherche si on a un bind en lien avec l'evenement
            if (bind) {
                if (event->type == ei_ev_mouse_buttondown || event->type == ei_ev_mouse_buttonup || event->type == ei_ev_mouse_move) { //on regarde le type de notre event pour pouvoir utiliser mouse.where
                    change_event = (bind->callback)(ei_widget_pick(&event->param.mouse.where), event, bind->user_param); //on appelle notre callback
                }else{ // si notre event a pas de picking
                    change_event = (bind->callback)(NULL,event,bind->user_param);
                }
            }
            if (bind && ! change_event) { //si on a pas finit notre callback alors on regarde la suite
                binds = bind->next_bind;
            }


        }while(!change_event && bind);
        if (rects) {
            hw_surface_unlock(root_surface);
            //TODO utiliser rects à la place de clipper (il faut calculer l'union de tout les rects)
            clipper = get_smallest_containing_rect(rects);
            ei_impl_widget_draw_children(root, root_surface, pick_surface, &clipper);
            hw_surface_update_rects(root_surface, rects);
            // IL faut release tout les rects
            release_linked_rect(rects);
            rects = NULL;
            hw_surface_lock(root_surface);
        }


    }
    free(event);
}


/**
 * @brief ajoute a la tete de la liste de rectangle a update un rectangle
 * @param rect
 */
void add_head_rects(const ei_rect_t* rect){
    ei_linked_rect_t* new_rect = calloc(1,sizeof(ei_linked_rect_t));
    ei_rect_t rect_correction = get_rect_intersection(*rect, root->screen_location);
    new_rect->next = rects;
    new_rect->rect.top_left.x = rect_correction.top_left.x;
    new_rect->rect.top_left.y = rect_correction.top_left.y;
    new_rect->rect.size.width = rect_correction.size.width;
    new_rect->rect.size.height = rect_correction.size.height;
    rects = new_rect;
}
/**
 * \brief	Adds a rectangle to the list of rectangles that must be updated on screen. The real
 *		update on the screen will be done at the right moment in the main loop.
 *
 * @param	rect		The rectangle to add, expressed in the root window coordinates.
 *				A copy is made, so it is safe to release the rectangle on return.
 */
void ei_app_invalidate_rect(const ei_rect_t* rect){
    ei_rect_t rect_correct = get_rect_intersection(*(ei_rect_t*)rect,root->screen_location);
    if (!rects) {
            add_head_rects(&rect_correct);
            return;
    }
    ei_linked_rect_t* prec = rects;
    ei_linked_rect_t* current;
    ei_rect_t inter;
    inter=get_rect_intersection(prec->rect,rect_correct);
    if (inter.size.width == (prec->rect).size.width &&
        inter.size.height == (prec->rect).size.height &&
        inter.top_left.x == (prec->rect).top_left.x &&
        inter.top_left.y == (prec->rect).top_left.y){

            rects = rects->next;
            free(prec);
            ei_app_invalidate_rect(rect);
            return;
    }
    while(prec->next){
        current=prec->next;
        inter=get_rect_intersection(current->rect,rect_correct);
        // si l'intersection entre le nouveau rectangle et current est le rect current alors on supprime current (current est inclu dans le nouveau rect)
        if (inter.size.width == (current->rect).size.width &&
            inter.size.height == (current->rect).size.height &&
            inter.top_left.x == (current->rect).top_left.x &&
            inter.top_left.y == (current->rect).top_left.y){

                prec->next = current->next;
                free(current);

        }
        // si l'intersection entre le nouveau rectangle et current est le nouveau rectangle alors on ajoute pas le nouveau rectangle(current contient le nouveau rect)
        if (inter.size.width == (rect_correct).size.width &&
            inter.size.height == (rect_correct).size.height &&
            inter.top_left.x == (rect_correct).top_left.x &&
            inter.top_left.y == (rect_correct).top_left.y){

            return;
        }
        prec = current;
    }
    add_head_rects(&rect_correct);
}

/**
 * \brief	Tells the application to quit. Is usually called by an event handler (for example
 *		when pressing the "Escape" key).
 */
void ei_app_quit_request(void){
        quit = true;
}

/**
 * \brief	Returns the "root widget" of the application: a "frame" widget that span the entire
 *		root window.
 *
 * @return 			The root widget.
 */
ei_widget_t ei_app_root_widget(void){
    return root;
}

/**
 * \brief	Returns the surface of the root window. Can be used to create surfaces with similar
 * 		r, g, b channels.
 *
 * @return 			The surface of the root window.
 */
ei_surface_t ei_app_root_surface(void){
    return root_surface;
}





