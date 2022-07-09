#ifndef OS_PROJECT_SEMAPHORE_H
#define OS_PROJECT_SEMAPHORE_H

#include <semaphore.h>

#include "../utils/list.h"
#include "../process/process.h"

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

typedef struct SemaphoreTable {
    semaphore_t* table;
    int len;
} semaphore_table_t;

/* Semaphore Function Prototypes */

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

/**
 * It register the semaphore with the specified
 * semaphore name, however, if the semaphore
 * already exists, then nothing is done.
 *
 * @param sem_table the semaphore table in which
 *                  the semaphore will be registered
 * @param name the semaphore name to be
 *             registered
 */
void semaphore_register(semaphore_table_t* sem_table, const char* name);

/**
 * It returns a semaphore with the specified name if
 * there is a semaphore in the semaphore table with
 * that name. Otherwise, NULL is returned.
 *
 * @param sem_table the semaphore table
 * @param name the semaphore name
 */
semaphore_t* semaphore_find(semaphore_table_t* sem_table, const char* name);

/**
 * It request an access to the specified semaphore.
 * If no resources is available then the current
 * process requesting the semaphore is blocked and
 * put into the semaphore's waiting list.
 *
 * @param sem a pointer to the semaphore
 * @param proc the process requesting access
 *             to this semaphore
 * @param sleep a function whose invoke causes
 *              the specified process to sleep
 */
void semaphore_P(semaphore_t* sem, process_t* proc, void (*sleep)(process_t*));

/**
 * It releases an access to the specified semaphore.
 * If there are pending processes waiting for some
 * process to release its access, the least recently
 * requested process will be woken up.
 *
 * @param sem a pointer to the semaphore
 * @param wakeup a function whose invoke causes
 *               a process to wake up
 */
void semaphore_V(semaphore_t* sem, void (*wakeup)(process_t*));

/* Semaphore Table Function Prototypes */

/**
 * It initializes the semaphore table specified
 * by a pointer to a semaphore table.
 *
 * @param sem_table a pointer to the semaphore table
 */
void semaphore_table_init(semaphore_table_t* sem_table);

#endif // OS_PROJECT_SEMAPHORE_H
