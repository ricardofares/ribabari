#include <stdio.h>
#include <stdlib.h>

#include "list.h"

/* List Node Functions */

/**
 * It creates a node holding the
 * specified content. Further, it
 * returns null if the system does
 * not have enough memory to allocate
 * the node.
 *
 * @param content the content
 *
 * @return the created node or
 *         NULL (not enough memory)
 */
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

/**
 * It initializes the list.
 *
 * @param list the list
 */
list_t* list_init() {
    list_t* list = (list_t *)malloc(sizeof(list_t));

    /* It checks if the list has not been allocated */
    if (!list) {
        printf("Not enough memory to allocate a list!\n");
        exit(0);
    }

    list->head = NULL;
    list->tail = NULL;
    return list;
}

/**
 * It adds the content at the end of
 * the linked list.
 *
 * @param list the list
 * @param content the content to be
 *                added at the end
 */
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

/**
 * It removes the list head and
 * returns the node there located.
 *
 * @param list the list
 */
list_node_t *list_remove_head(list_t *list) {
    list_node_t *temp = list->head;

    if (list->head) {
        if (list->head->next)
            list->head->next->prev = NULL;
        else list->tail = NULL;
        list->head = list->head->next;

        /* Detach the node from the list */
        temp->next = NULL;
        temp->prev = NULL;
    }

    return temp;
}

/**
 * It removes the list tail and
 * returns the node there located.
 *
 * @param list the list
 *
 * @return the removed tail node
 */
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

/**
 * It releases the memory allocated
 * to the list.
 *
 * @param list the list to be freed
 */
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