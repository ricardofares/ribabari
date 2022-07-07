#include "semaphore.h"

/**
 * It initializes the specified semaphore
 * with the specified initial value. Further,
 * it is supposed that the specified semaphore
 * is non NULL.
 *
 * @param sem the semaphore to be initialized
 * @param S the initial semaphore value
 */
void semaphore_init(semaphore_t* sem, const int S) {
    sem->S = S;
    sem->waiters = list_init();
}
