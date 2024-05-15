//
// Created by anto on 15/05/24.
//

#ifndef PROJETC_IG_EI_IMPL_BINDS_H
#define PROJETC_IG_EI_IMPL_BINDS_H

#include "ei_implementation.h"

//======================================== event

/**
 * @brief Type definissant les binds
 * Il conviendra d'initialiser une variable globale pour acceder a tous les binds et
 * de release les binds à la fin.
 */
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

/**
 * @brief Fonction qui return la variable globale qui pointe sur les binds
 * @return variable globale pointant sur le dernier bind implémenté.
 */
ei_bind_t* ei_get_head_binds();

/**
 * @brief Fonction retournant le premier bind correspondant au type d'evennement de event
 *
 * @param event l'évenement
 * @param current_bind le bind suivant de celui qui vient d'être executé
 *
 * @return le bind correspondant au type d'evennemebt de event.
 */
ei_bind_t* ei_bind_from_event(ei_event_t* event, ei_bind_t* current_bind);

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
                    void*			user_param);

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
                    void*			user_param);



#endif //PROJETC_IG_EI_IMPL_BINDS_H
