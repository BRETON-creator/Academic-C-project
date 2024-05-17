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
#include "ei_draw.h"
#include "ei_event.h"
#include "ei_impl_binds.h"
#include "ei_impl_placer.h"
#include "var.h"


ei_surface_t pick_surface;
ei_impl_widget_t* root = NULL;
bool quit              = false;
ei_surface_t root_surface;
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

    //      register button class of widget
    ei_widgetclass_t* toplevelclass = calloc(1,sizeof(ei_widgetclass_t));
    toplevelclass->allocfunc        = ei_impl_alloc_toplevel;
    toplevelclass->releasefunc      = ei_impl_release_toplevel;
    toplevelclass->drawfunc         = ei_impl_draw_toplevel;
    toplevelclass->setdefaultsfunc  = ei_impl_setdefaults_toplevel;
    toplevelclass->geomnotifyfunc   = ei_impl_geomnotify_toplevel;
    toplevelclass->next             = NULL;
    strcpy(toplevelclass->name,(ei_widgetclass_name_t){"toplevel\0"});
    ei_widgetclass_register(toplevelclass);

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

    //free la surface offscreen
    //hw_surface_free(??)
    //free tous les widgets (parcours suffixe de l'arbre des widgets
    ei_widget_destroy(root);

    //free les widgets_class (on en a que 2...
    ei_widgetclass_t *tmp = ei_widgetclass_from_name("button\0");
    ei_widgetclass_t *suiv= tmp->next;
    while (suiv){
        free(tmp);
        tmp=suiv;
        suiv=suiv->next;
    }
    free(tmp);
    //free les geometrymanager (on en a qu'un pour le moment...
    free(ei_geometrymanager_from_name("placer\0"));

    hw_quit();
}

void release_linked_rect(ei_linked_rect_t* list){
    ei_linked_rect_t *tmp;
    while(list){
        tmp=list->next;
        free(list);
        list=tmp;
    }
}

void update_place(ei_widget_t widget){
        if (widget->geom_params) ei_impl_placer_runfunc(widget);
        ei_widget_t child = widget->children_head;
        while (child){
                update_place(child);
                child = child->next_sibling;
        }
}


/**
 * \brief	Runs the application: enters the main event loop. Exits when
 *		\ref ei_app_quit_request is called.
 */
void ei_app_run(void){

    update_place(root);

    ei_rect_t clipper           = hw_surface_get_rect(ei_app_root_surface());
    ei_impl_widget_draw_children(root, root_surface, pick_surface, &clipper);

    //boucle principale

    //binds interns
    ei_bind(ei_ev_mouse_buttondown, NULL,"button\0",ei_callback_clickbutton,NULL);
    ei_bind(ei_ev_mouse_buttonup,NULL,"button\0",ei_callback_clickbutton,NULL);

    ei_bind(ei_ev_mouse_buttondown, NULL,"toplevel\0",ei_callback_toplevel,NULL);
    ei_bind(ei_ev_mouse_move, NULL,"toplevel\0",ei_callback_toplevel,NULL);
    ei_bind(ei_ev_mouse_buttonup, NULL,"toplevel\0",ei_callback_toplevel,NULL);

    ei_widget_t cur = ei_app_root_widget();
    ei_event_t* event = calloc(1,sizeof(ei_event_t));
    ei_bind_t* bind;
    ei_bind_t* binds;
    bool change_event;
    while(!quit){
        binds=ei_get_head_binds();
        hw_event_wait_next(event);

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
        hw_surface_unlock(root_surface);
        hw_surface_update_rects(root_surface,rects);
        // IL faut release tout les rects
        release_linked_rect(rects);
        rects=NULL;
        hw_surface_lock(root_surface);
    }
    ei_unbind(ei_ev_mouse_buttondown, NULL,"button\0",ei_callback_clickbutton,NULL);
    ei_unbind(ei_ev_mouse_buttonup,NULL,"button\0",ei_callback_clickbutton,NULL);

    ei_unbind(ei_ev_mouse_buttondown, NULL,"toplevel\0",ei_callback_toplevel,NULL);
    ei_unbind(ei_ev_mouse_move, NULL,"toplevel\0",ei_callback_toplevel,NULL);
    ei_unbind(ei_ev_mouse_buttonup, NULL,"toplevel\0",ei_callback_toplevel,NULL);
    free(event);
}




/**
 * \brief	Adds a rectangle to the list of rectangles that must be updated on screen. The real
 *		update on the screen will be done at the right moment in the main loop.
 *
 * @param	rect		The rectangle to add, expressed in the root window coordinates.
 *				A copy is made, so it is safe to release the rectangle on return.
 */
void ei_app_invalidate_rect(const ei_rect_t* rect){
    ei_linked_rect_t* new_rect = calloc(1,sizeof(ei_linked_rect_t));
    new_rect->next = rects;
    new_rect->rect.top_left.x = rect->top_left.x;
    new_rect->rect.top_left.y = rect->top_left.y;
    new_rect->rect.size.width = rect->size.width;
    new_rect->rect.size.height = rect->size.height;
    rects = new_rect;
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





