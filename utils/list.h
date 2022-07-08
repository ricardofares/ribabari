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
list_node_t *list_search(list_t *list, void *content, int (*matcher)(void *, void *));

/**
 * It returns 1 if the list is empty.
 * Otherwise, it returns 0.
 *
 * @return 1 if the list is empty;
 *         otherwise, it returns 0.
 */
int list_empty(list_t *list);

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
 * It removes the node from the list.
 *
 * @param list the list to have its
 *             node removed
 * @param node the node to be removed
 */
void list_remove_node(list_t *list, list_node_t *node);

/**
 * It releases the memory allocated
 * to the list.
 *
 * @param list the list to be freed
 */
void list_free(list_t *list);

#endif // OS_PROJECT_LIST_H
