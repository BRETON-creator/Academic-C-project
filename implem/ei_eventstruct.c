//
// Created by anto on 09/05/24.
//


#include "ei_eventstruct.h"
#include "ei_event.h"

//definit les binds
ei_bind_t* binds = NULL;

ei_linked_tag_t* linked_tag = &(ei_linked_tag_t){"all\0",NULL};

ei_bind_t* ei_get_head_binds(){
    return binds;
}

ei_bind_t* ei_bind_from_event(ei_event_t* event, ei_bind_t* current_bind){
    ei_bind_t* cur_bind = current_bind;
    while (cur_bind){
        if (event->type == cur_bind->eventtype && (event->type == ei_ev_mouse_buttondown || event->type == ei_ev_mouse_buttonup || event->type == ei_ev_mouse_move)){
            if (cur_bind->bind_isWidget && cur_bind->object.widget == ei_widget_pick(&event->param.mouse.where)){
                return cur_bind;
            }
            if (! cur_bind->bind_isWidget && strcmp(cur_bind->object.tag,"all\0")==0){
                return cur_bind;
            }
            //... TODO
        }
        if (event->type == cur_bind->eventtype && event->type == ei_ev_keydown && !cur_bind->bind_isWidget && strcmp(cur_bind->object.tag,"all\0")==0){
            return cur_bind;
        }
        cur_bind = cur_bind->next_bind;
    }
    return NULL;
}

void ei_create_bind(ei_eventtype_t		eventtype,
                    ei_widget_t		widget,
                    ei_tag_t		tag,
                    ei_callback_t		callback,
                    void*			user_param){
    ei_bind_t* new_bind = calloc(1, sizeof(ei_bind_t));
    ei_bind_t* tmp = binds;
    binds=new_bind;
    new_bind->next_bind=tmp;
    new_bind->callback = callback;
    if (widget) {
        new_bind->object.widget = widget;
        new_bind->bind_isWidget = true;
    }else{
        strcpy(new_bind->object.tag,tag);
        new_bind->bind_isWidget = false;
    }
    new_bind->eventtype = eventtype;
    new_bind->user_param= user_param;
}

void ei_delete_bind(ei_eventtype_t		eventtype,
                    ei_widget_t		widget,
                    ei_tag_t		tag,
                    ei_callback_t		callback,
                    void*			user_param){
    ei_bind_t *current;
    ei_bind_t *prec = binds;
    if      (prec->eventtype==eventtype &&
             ((prec->bind_isWidget && prec->object.widget == widget) || (!prec->bind_isWidget && strcmp(prec->object.tag, tag)==0)) &&
             prec->callback == callback &&
             prec->user_param == user_param){
        binds = prec->next_bind;
        free(prec);
        return;
    }
    while (prec){
        current = prec->next_bind;
        if      (current->eventtype==eventtype &&
                ((current->bind_isWidget && current->object.widget == widget) || (!current->bind_isWidget && strcmp(current->object.tag, tag)==0)) &&
                current->callback == callback &&
                current->user_param == user_param){
            prec->next_bind=current->next_bind;
            free(current);
            return;
        }
        prec = current;
    }
}
