#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget_configure.h"
#include "ei_placer.h"
#include "ei_utils.h"
#include "ei_widget_attributes.h"


ei_string_t		image_filename	= "misc/klimt.jpg";
ei_surface_t image ;
ei_size_t image_size;
ei_size_t		image_rect_size;
ei_rect_t		img_rect;
ei_rect_ptr_t		img_rect_ptr		= &img_rect;

/*
 * button_press --
 *
 *	Callback called when a user clicks on the button.
 */
bool button_press(ei_widget_t widget, ei_event_t* event, ei_user_param_t user_param)
{
	printf("Click !\n");
	return true;
}

/*
 * default_handler --
 *
 *	Callback called to handle keypress and window close events.
 */
bool default_handler(ei_widget_t widget, ei_event_t* event, ei_user_param_t user_param)
{
	if ( (event->type == ei_ev_close) ||
	    ((event->type == ei_ev_keydown) && (event->param.key_code == SDLK_ESCAPE))) {
		ei_app_quit_request();
		return true;
	} else
		return false;
}

int main(int argc, char** argv)
{
	ei_widget_t	button;

	/* Create the application and change the color of the background. */
	ei_app_create			((ei_size_t){1400, 1000}, false);
	ei_frame_set_bg_color		(ei_app_root_widget(), (ei_color_t){0x52, 0x7f, 0xb4, 0xff});

	/* Create, configure and place the button on screen. */
	button = ei_widget_create	("button", ei_app_root_widget(), NULL, NULL);
	image = hw_image_load(image_filename, ei_app_root_surface());
	image_size	= hw_surface_get_size(image);
	//ei_point_t point = button->screen_location;
	image_rect_size = (ei_size_t){image_size.width + 10 , image_size.height +10};
	//img_rect		= ei_rect( point,
	//							image_rect_size);
	ei_button_configure		(button, &image_rect_size,
						&(ei_color_t){0x88, 0x88, 0x88, 0xff},
					 	&(int){0},
					 	&(int){0},
					 	&(ei_relief_t){ei_relief_raised},
					 	&(ei_string_t){"Mon premier Bouton !"}, NULL,
					 	&(ei_color_t){0x00, 0x00, 0x00, 0xff}, NULL, &image, NULL, NULL,
					 	&(ei_callback_t){button_press}, NULL);
	ei_place_xy			(button, 150, 200);

	/* Register the default callback to events of interest. */
	ei_bind(ei_ev_keydown,		NULL, "all", default_handler, NULL);
	ei_bind(ei_ev_close,		NULL, "all", default_handler, NULL);

	/* Run the application's main loop. */
	ei_app_run();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_unbind(ei_ev_keydown,	NULL, "all", default_handler, NULL);
	ei_unbind(ei_ev_close,		NULL, "all", default_handler, NULL);

	ei_app_free();

	return (EXIT_SUCCESS);
}
