//
// Created by anto on 15/05/24.
//

#include "ei_impl_placer.h"


//===================================== placer

/**
 * @brief   Fonction run geometrymanager de PLACER
 */
void ei_impl_placer_runfunc(ei_widget_t widget){
    int *x              = ((ei_placer_t*)widget->geom_params)->x;
    int *y              = ((ei_placer_t*)widget->geom_params)->y;
    int *width          =  ((ei_placer_t*)widget->geom_params)->width;
    int *height         =  ((ei_placer_t*)widget->geom_params)->height;
    float *rel_x        =  ((ei_placer_t*)widget->geom_params)->rel_x;
    float *rel_y        =  ((ei_placer_t*)widget->geom_params)->rel_y;
    float *rel_width    =  ((ei_placer_t*)widget->geom_params)->rel_width;
    float *rel_height   =  ((ei_placer_t*)widget->geom_params)->rel_height;

    ei_rect_t *old_surface = &(ei_rect_t){(ei_point_t){widget->screen_location.top_left.x, widget->screen_location.top_left.y},
                                          (ei_size_t){widget->screen_location.size.width,widget->screen_location.size.height}};

    ei_place(widget, ((ei_placer_t*)widget->geom_params)->anchor,x,y,width,height,rel_x,rel_y,rel_width,rel_height);
    ei_rect_t new_surface = (widget->screen_location);
    widget->screen_location = *old_surface;
    ei_geometry_run_finalize(widget, &new_surface);
}

/**
 * @brief Release function of placer
 */
void  ei_impl_placer_releasefunc(ei_widget_t widget){
    free(widget->geom_params);
}