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

#define MAXAPP 500


ei_impl_widget_t* root = NULL;
bool quit = false;
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
    ei_widgetclass_t* frameclass=malloc(sizeof(ei_widgetclass_t));
    frameclass->allocfunc = ei_impl_alloc_frame;
    frameclass->releasefunc = ei_impl_release_frame;
    frameclass->drawfunc = ei_impl_draw_frame;
    frameclass->setdefaultsfunc = ei_impl_setdefaults_frame;
    frameclass->geomnotifyfunc = NULL;
    frameclass->next = NULL;
    strcpy(frameclass->name,(ei_widgetclass_name_t){"frame\0"});
    ei_widgetclass_register(frameclass);

    //      register button class of widget
    ei_widgetclass_t* buttonclass=malloc(sizeof(ei_widgetclass_t));
    buttonclass->allocfunc = ei_impl_alloc_button;
    buttonclass->releasefunc = ei_impl_release_button;
    buttonclass->drawfunc = ei_impl_draw_button;
    buttonclass->setdefaultsfunc = ei_impl_setdefaults_button;
    buttonclass->geomnotifyfunc = NULL;
    buttonclass->next = NULL;
    strcpy(buttonclass->name,(ei_widgetclass_name_t){"button\0"});
    ei_widgetclass_register(buttonclass);

    //      register geometry manager "placer"
    ei_geometrymanager_t* placer=calloc(1,sizeof(ei_geometrymanager_t));
    strcpy(placer->name,"placer\0");
    placer->runfunc = ei_impl_placer_runfunc;
    placer->releasefunc = ei_impl_placer_releasefunc;
    ei_geometrymanager_register(placer);

    // creates the root window (either in a system window, or the entire screen
    ei_surface_t main_window= hw_create_window(main_window_size,fullscreen);
    hw_surface_lock(main_window);


    // creates the root widget to access the root window.
    root= ei_impl_alloc_frame();
    ei_impl_setdefaults_frame(root);
    root->parent=NULL;
    root->requested_size = main_window_size;
    ((ei_impl_frame_t*)root)->frame_relief=ei_relief_none;
    root->screen_location = hw_surface_get_rect(main_window);
    root->requested_size= root->screen_location.size;
    root_surface = main_window;

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

    hw_quit();
}

/**
 * \brief	Runs the application: enters the main event loop. Exits when
 *		\ref ei_app_quit_request is called.
 */
void ei_app_run(void){
    //dessin des widgets dans l'arbre
    ei_widget_t current=ei_app_root_widget();
    ei_widget_t stack[MAXAPP];
    stack[0]=NULL;
    ei_widget_t child=NULL;
    int stack_size=0;
    // TODO : gerer les differentes surfaces, pick_surface et clipper !

    ei_surface_t surface=ei_app_root_surface();
    ei_surface_t pick_surface=ei_app_root_surface();
    ei_rect_t clipper = hw_surface_get_rect(ei_app_root_surface());
    while (stack_size || current){
        while (current) {
            //clipper = rectangle de la surface du parent, des surfaces des widgets qui sont devant lui dans l'offscreen
            current->wclass->drawfunc(current, surface, pick_surface, &clipper);
            if (current->children_head) child = current->children_head->next_sibling;
            while (child) {
                stack[stack_size] = child;
                stack_size++;
                child = child->next_sibling;
            }
            current = current->children_head;
            //il faudrait mettre à jour surface , pick surface , et clipper pour chaque widget qu'on veut dessiner
        }
        if (stack_size){
            stack_size--;
            current=stack[stack_size];
            stack[stack_size]=NULL;
        }
    }

    getchar();

    while (!quit){
            //ei_app_invalidate_rect(...);

            ei_event_t* event = malloc(sizeof(ei_event_t));
            hw_event_wait_next(event);

    }
    /*
     * ME SUIS PRIS LA TETE POUR RIEN JE SUIS STUPIDE OUI
     * Y'a jsute a faire ca :
     * dessiner root
     * appeler ei_impl_widget_draw_children
     *
     * bien plus simple. :/
     * imma so stupid
     * => TODO implementer ei_impl_widget_draw_children (ei_implementation.c)
     * */
}

/**
 * \brief	Change the color of the background window (root widget) with the given color in parameters.
 *
 * @param	widget (the root widget)	, color (the structure representing a color)
 *
 */
void ei_frame_set_bg_color(ei_widget_t* widget , ei_color_t color){
    ei_impl_frame_t* frame = (ei_impl_frame_t*)widget;
    frame->frame_color = color;
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



