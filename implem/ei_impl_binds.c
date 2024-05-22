//
// Created by anto on 15/05/24.
//

#include "ei_impl_binds.h"

// ============================ event

//definit les binds
ei_bind_t* binds = NULL;

/**
 * @brief Fonction qui return la variable globale qui pointe sur les binds
 * @return variable globale pointant sur le dernier bind implémenté.
 */
ei_bind_t* ei_get_head_binds(){
        return binds;
}

/**
 * @brief Fonction retournant le premier bind correspondant au type d'evennement de event
 *
 * @param event l'évenement
 * @param current_bind le bind suivant de celui qui vient d'être executé
 *
 * @return le bind correspondant au type d'evennemebt de event.
 */
ei_bind_t* ei_bind_from_event(ei_event_t* event, ei_bind_t* current_bind){
        ei_bind_t* cur_bind = current_bind;
        while (cur_bind){
                if (event->type == cur_bind->eventtype){
                        return cur_bind;
                }
                cur_bind = cur_bind->next_bind;
        }
        return NULL;
}

/**
 * @brief creer un bind (et le link au autre binds de la variable globale
 * @param eventtype
 * @param widget
 * @param tag
 * @param callback
 * @param user_param
 */
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
                new_bind->object.tag=calloc(1,sizeof(ei_tag_t));
                strcpy(new_bind->object.tag,tag);
                new_bind->bind_isWidget = false;
        }
        new_bind->eventtype = eventtype;
        new_bind->user_param= user_param;
}


/**
 * @brief supprime un bind
 * @param eventtype
 * @param widget
 * @param tag
 * @param callback
 * @param user_param
 */
void ei_delete_bind(ei_eventtype_t		eventtype,
                    ei_widget_t		widget,
                    ei_tag_t		tag,
                    ei_callback_t		callback,
                    void*			user_param){
        ei_bind_t *current = NULL;
        ei_bind_t *prec = binds;
        if   (prec->eventtype==eventtype &&
             ((prec->bind_isWidget && prec->object.widget == widget) || (!prec->bind_isWidget && strcmp(prec->object.tag, tag)==0)) &&
             prec->callback == callback &&
             prec->user_param == user_param){
                binds = prec->next_bind;
                free(prec);
                return;
        }
        while (prec->next_bind){
                current = prec->next_bind;
                if (current->eventtype==eventtype &&
                    ((current->bind_isWidget && current->object.widget == widget) || (!current->bind_isWidget && strcmp(current->object.tag, tag)==0)) &&
                    current->callback == callback &&
                    current->user_param == user_param){
                        prec->next_bind=current->next_bind;
                        if (!current->bind_isWidget) free(current->object.tag);
                        free(current);
                        return;
                }
                prec = current;
        }
}

