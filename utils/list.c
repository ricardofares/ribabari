#include <stdio.h>
#include <stdlib.h>

#include "list.h"

/* List Node Functions */

list_node_t *list_node_create(void *content) {
    list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));

    /* It checks if the node has not been allocated */
    if (!node) {
        printf("Not enough memory to allocate a node!\n");
        return NULL;
    }

    node->content = content;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

/* List Functions */

void list_init(list_t **list) {
    (*list) = (list_t *)malloc(sizeof(list_t));

    /* It checks if the list has not been allocated */
    if (!(*list)) {
        printf("Not enough memory to allocate a list!\n");
        return;
    }

    (*list)->head = NULL;
    (*list)->tail = NULL;
}

void list_add(list_t *list, void *content) {
    list_node_t *node = list_node_create(content);

    /* It checks if the node has not been created */
    if (!node)
        return;

    /* It checks if the list is empty */
    if (!list->tail)
        list->head = node;
    else {
        list->tail->next = node;
        node->prev = list->tail;
    }

    list->tail = node;
}

list_node_t *list_remove_head(list_t *list) {
    list_node_t *temp = list->head;

    if (list->head) {
        if (list->head->next)
            list->head->next->prev = NULL;
        else list->tail = NULL;
        list->head = list->head->next;
    }

    /* Detach the node from the list */
    temp->next = NULL;
    temp->prev = NULL;
    return temp;
}

list_node_t *list_remove_tail(list_t *list) {
    list_node_t *temp = list->tail;

    if (list->tail) {
        if (list->tail->prev)
            list->tail->prev->next = NULL;
        else list->head = NULL;
        list->tail = list->tail->prev;

        /* Detach the node from the list */
        temp->next = NULL;
        temp->prev = NULL;
    }

    return temp;
}

void list_free(list_t *list) {
    if (list) {
        list_node_t *curr = list->head;
        list_node_t *temp;

        while (curr) {
            temp = curr->next;
            free(curr);
            curr = temp;
        }

        free(list);
    }
}