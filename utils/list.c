#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
list_node_t *list_node_create(void *content, unsigned long long int content_size) {
    register int i;
    list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));

    /* It checks if the node has not been allocated */
    if (!node) {
        printf("Not enough memory to allocate a node!\n");
        return NULL;
    }

    node->content = (void *)malloc(content_size);

    /* It checks if the node could not be allocated */
    if (!node->content) {
        printf("Not enough memory to allocate the content size.\n");
        exit(EXIT_FAILURE);
    }

    /* Copy the content to the allocated space */
    memcpy(node->content, content, content_size);

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
 * @param content_size the content size, it can be
 *                     provided by the use of sizeof
 */
void list_add(list_t *list, void *content, unsigned long long int content_size) {
    list_node_t *node = list_node_create(content, content_size);

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
 * It returns a list node if the matcher matches
 * the specified content with that in the node.
 * If does not any node has been matched, then
 * NULL is returned.
 *
 * @param list the list
 * @param content the content to be matched
 * @param matcher the matcher
 *
 * @return a list node if some node in the list
 *         has been matched; otherwise NULL.
 *
 */
list_node_t *list_search(list_t *list, void *content, int (*matcher)(void *, void *)) {
    list_node_t* matched;
    for (matched = list->head; matched != NULL; matched = matched->next)
        if (matcher(matched->content, content))
            return matched;
    return NULL;
}

/**
 * It returns 1 if the list is empty.
 * Otherwise, it returns 0.
 *
 * @return 1 if the list is empty;
 *         otherwise, it returns 0.
 */
int list_empty(list_t *list) {
    return list->head == NULL;
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
 * It removes the node from the list.
 *
 * @param list the list to have its
 *             node removed
 * @param node the node to be removed
 */
void list_remove_node(list_t *list, list_node_t *node) {
    if (node->prev)
        node->prev->next = node->next;
    else if (list->head == node) list->head = node->next;

    if (node->next)
        node->next->prev = node->prev;
    else if (list->tail == node) list->tail = node->prev;
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

