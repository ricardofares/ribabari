#ifndef OS_PROJECT_LIST_H
#define OS_PROJECT_LIST_H

/**
 * A doubly-linked node.
 */
typedef struct ListNode {
    /**
     * It holds the node content.
     */
    void *content;

    /**
     * It holds a pointer to the
     * next linked node.
     */
    struct ListNode *next;

    /**
     * It holds a pointer to the
     * previous linked node.
     */
    struct ListNode *prev;
} list_node_t;

/**
 * A doubly-linked list.
 */
typedef struct List {
    /**
     * It holds a pointer to the
     * head node.
     */
    list_node_t *head;

    /**
     * It holds a pointer to the
     * tail node.
     */
    list_node_t *tail;
} list_t;

/* List Node Prototypes */

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
list_node_t *list_node_create(void *content);

/* List Prototypes */

/**
 * It initializes the list.
 */
list_t* list_init();

/**
 * It adds the content at the end of
 * the linked list.
 *
 * @param list the list
 * @param content the content to be
 *                added at the end
 */
void list_add(list_t *list, void *content);

/**
 * It removes the list head and
 * returns the node there located.
 *
 * @param list the list
 */
list_node_t *list_remove_head(list_t *list);

/**
 * It removes the list tail and
 * returns the node there located.
 *
 * @param list the list
 *
 * @return the removed tail node
 */
list_node_t *list_remove_tail(list_t *list);

/**
 * It releases the memory allocated
 * to the list.
 *
 * @param list the list to be freed
 */
void list_free(list_t *list);

#endif // OS_PROJECT_LIST_H
