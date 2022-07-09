#ifndef OS_PROJECT_SEMAPHORE_H
#define OS_PROJECT_SEMAPHORE_H

#include <semaphore.h>
#include "../utils/list.h"

typedef struct Semaphore {
    /**
     * It represents the semaphore
     * name. Further, it is used
     * as identifier.
     */
    char *name;

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

    /**
     * It represents the system binary
     * semaphore used to ensure mutual
     * exclusion when handling the
     * "virtual" semaphore.
     */
    sem_t mutex;
} semaphore_t;

/**
 * It initializes the specified semaphore
 * with the specified initial value. Further,
 * it is supposed that the specified semaphore
 * is non NULL.
 *
 * @param sem the semaphore to be initialized
 * @param name the semaphore name
 * @param S the initial semaphore value
 */
void semaphore_init(semaphore_t* sem, const char* name, int S);

#endif // OS_PROJECT_SEMAPHORE_H
