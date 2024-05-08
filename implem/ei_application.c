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
#include "ei_draw.h"
#include "ei_placer.h"
#include "ei_event.h"
#include "ei_widget_configure.h"
#include "var.h"


ei_surface_t pick_surface;
ei_impl_widget_t* root = NULL;
bool quit              = false;
ei_surface_t root_surface;



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
    //TODO : creer le off screen aussi !
    // et initialiser le picking?...

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
    root->parent=NULL;
    root->requested_size                    = main_window_size;
    ((ei_impl_frame_t*)root)->frame_relief  =ei_relief_none;
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
    //free la surface offscreen
    //hw_surface_free(??)
    //free tous les widgets (parcours suffixe de l'arbre des widgets
    ei_widget_destroy(root);
    hw_quit();
}
bool point_in_surface(int x, int y, ei_rect_t rect){
        int x_min = rect.top_left.x;
        int y_min = rect.top_left.y;
        int x_max = rect.size.width + x_min;
        int y_max = rect.size.height + y_min;

        return (x_min<=x && x<=x_max && y_min<=y && y<=y_max);
}

ei_widget_t find_widget(ei_event_t event){
        int x = event.param.mouse.where.x;
        int y = event.param.mouse.where.y;

        ei_widget_t widget = root->children_head;
        ei_rect_t rect = widget->screen_location;

        while (true){
                if (point_in_surface(x, y, rect)) break;
                if (widget == NULL) break;
                widget=widget->children_head;
        }

        return widget;
}

/**
 * \brief	Runs the application: enters the main event loop. Exits when
 *		\ref ei_app_quit_request is called.
 */
void ei_app_run(void){
    //dessin des widgets dans l'arbre
    // TODO : gerer pick surface et clipper

    ei_surface_t surface        = ei_app_root_surface();
    ei_rect_t clipper           = hw_surface_get_rect(ei_app_root_surface());

    ei_impl_widget_draw_children(root, surface,pick_surface,&clipper);

    ei_widget_t previous_widget;
    while (!quit){
            ei_app_invalidate_rect(root);
            ei_event_t* event = malloc(sizeof(ei_event_t));
            hw_event_wait_next(event);
            ei_widget_t widget = find_widget(*event);

            if ((event->type == ei_ev_mouse_buttonup) && ((ei_impl_frame_t*)previous_widget)->frame_relief ==  ei_relief_sunken){
                ((ei_impl_button_t*)previous_widget)->frame.frame_relief = ei_relief_raised;
                ei_impl_widget_draw_children(previous_widget, surface,pick_surface,&clipper);
            }


            if (widget){
                    ei_widgetclass_name_t name = {"button\0"};
                    if (strcmp(widget->wclass->name, name)==0 && event->type == ei_ev_mouse_buttondown && ((ei_impl_button_t*) widget)->frame.frame_relief ==  ei_relief_raised){
                            previous_widget = widget;
                            ((ei_impl_button_t*)widget)->callback(widget, event,((ei_impl_button_t*)widget)->user_params);
                            ((ei_impl_button_t*)widget)->frame.frame_relief = ei_relief_sunken;

                            ei_impl_widget_draw_children(widget, surface,pick_surface,&clipper);


                    }


            }



    }
}




/**
 * \brief	Adds a rectangle to the list of rectangles that must be updated on screen. The real
 *		update on the screen will be done at the right moment in the main loop.
 *
 * @param	rect		The rectangle to add, expressed in the root window coordinates.
 *				A copy is made, so it is safe to release the rectangle on return.
 */
void ei_app_invalidate_rect(const ei_rect_t* rect){

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



