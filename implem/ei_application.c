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

#define MAXAPP 500


ei_impl_widget_t* root;
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
    ei_size_t size;
    // initializes the hardware (calls \ref hw_init)
    hw_init();
    // registers all classes of widget and all geometry managers
    ei_widgetclass_t* frameclass=malloc(sizeof(ei_widgetclass_t));
    frameclass->allocfunc = ei_impl_alloc_frame;
    frameclass->releasefunc = ei_impl_release_frame;
    frameclass->drawfunc = ei_impl_draw_frame;
    frameclass->setdefaultsfunc = ei_impl_setdefaults_frame;
    frameclass->geomnotifyfunc = NULL;
    frameclass->next = NULL;
    strcpy(frameclass->name,(ei_widgetclass_name_t){"frame\0"});
    ei_widgetclass_register(frameclass);
    // creates the root window (either in a system window, or the entire screen
    ei_surface_t main_window= hw_create_window(main_window_size,fullscreen);
    //hw_surface_unlock(main_window);
    //hw_surface_update_rects(main_window,NULL);

    // creates the root widget to access the root window.
    root= ei_impl_alloc_frame();
    ei_impl_setdefaults_frame(root);
    root->parent=NULL;
    root_surface = main_window;

}

/**S
 * \brief	Releases all the resources of the application, and releases the hardware
 *		(ie. calls \ref hw_quit).
 */
void ei_app_free(void){}

/**
 * \brief	Runs the application: enters the main event loop. Exits when
 *		\ref ei_app_quit_request is called.
 */
void ei_app_run(void){
    //dessin des widgets dans l'arbre
    ei_widget_t current=ei_app_root_widget();
    ei_widget_t stack[MAXAPP];
    stack[0]=NULL;
    ei_widget_t child;
    int stack_size=0;
    // TODO : gerer les differentes surfaces, pick_surface et clipper !
    ei_surface_t surface=ei_app_root_surface();
    ei_surface_t pick_surface=ei_app_root_surface();
    ei_rect_t clipper = hw_surface_get_rect(ei_app_root_surface());
    while (stack_size || current){
        while (current) {
            current->wclass->drawfunc(current, surface, pick_surface, &clipper);
            if (current->children_head) child = current->children_head->next_sibling;
            while (child!=current->children_tail) {
                stack[stack_size] = child;
                stack_size++;
                child = child->next_sibling;
            }
            current = current->children_head;
        }
        if (stack_size){
            stack_size--;
            current=stack[stack_size];
            stack[stack_size]=NULL;
        }
    }

    getchar();
}

/**
 * \brief	Adds a rectangle to the list of rectangles that must be updated on screen. The real
 *		update on the screen will be done at the right moment in the main loop.
 *
 * @param	rect		The rectangle to add, expressed in the root window coordinates.
 *				A copy is made, so it is safe to release the rectangle on return.
 */
void ei_app_invalidate_rect(const ei_rect_t* rect){}

/**
 * \brief	Tells the application to quit. Is usually called by an event handler (for example
 *		when pressing the "Escape" key).
 */
void ei_app_quit_request(void){}

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



