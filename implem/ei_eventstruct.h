//
// Created by anto on 09/05/24.
//

#ifndef EI_EVENTSTRUCT_H
#define EI_EVENTSTRUCT_H

#include "ei_implementation.h"
#include "ei_event.h"

typedef struct ei_bind_t {
    union{
        ei_widget_t widget;
        ei_tag_t tag;
    } object;
    bool bind_isWidget; // 0 si object = tag, 1 si object = widget
    ei_eventtype_t eventtype;
    ei_callback_t callback;
    void* user_param;
    struct ei_bind_t* next_bind;
}ei_bind_t;

ei_bind_t* ei_get_head_binds();

ei_bind_t* ei_callback_from_event(ei_event_t* event, ei_bind_t* current_bind);

void ei_create_bind(ei_eventtype_t		eventtype,
                    ei_widget_t		widget,
                    ei_tag_t		tag,
                    ei_callback_t		callback,
                    void*			user_param);

void ei_delete_bind(ei_eventtype_t		eventtype,
                    ei_widget_t		widget,
                    ei_tag_t		tag,
                    ei_callback_t		callback,
                    void*			user_param);




#endif //EI_EVENTSTRUCT_H
