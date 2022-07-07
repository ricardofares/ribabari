#ifndef OS_PROJECT_SEMAPHORE_H
#define OS_PROJECT_SEMAPHORE_H

#include "../utils/list.h"

typedef struct Semaphore {
    /**
     * It represents the current
     * semaphore value in which
     * may or not be positive.
     */
    int S;

    /**
     * It contains the list of processes
     * waiting for this semaphore to be
     * available. This list is not NULL
     * since this semaphore is initialized.
     */
    list_t* waiters;
} semaphore_t;

/**
 * It initializes the specified semaphore
 * with the specified initial value. Further,
 * it is supposed that the specified semaphore
 * is non NULL.
 *
 * @param sem the semaphore to be initialized
 * @param S the initial semaphore value
 */
void semaphore_init(semaphore_t* sem, int S);

#endif // OS_PROJECT_SEMAPHORE_H
