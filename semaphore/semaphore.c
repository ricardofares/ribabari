#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semaphore.h"

/* Semaphore Function Definitions */

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
void semaphore_init(semaphore_t* sem, const char* name, const int S) {
    sem->name = strdup(name);
    sem->S = S;
    sem->waiters = list_init();
    sem_init(&sem->mutex, 0, 1);
}

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
void semaphore_register(semaphore_table_t* sem_table, const char* name) {
    int i;

    /* It checks if a semaphore with the specified name */
    /* has been already registered. */
    for (i = 0; i < sem_table->len; i++)
        if (strcmp(sem_table->table[i].name, name) == 0)
            return;

    sem_table->table = (semaphore_t *)
            realloc(sem_table->table, sizeof(semaphore_t) * (sem_table->len + 1));

    /* It checks if the request for reallocation of semaphore table */
    /* has been failed */
    if (!sem_table->table) {
        printf("Not enough memory to reallocate the semaphore table.\n");
        exit(0);
    }

    semaphore_init(&sem_table->table[sem_table->len], name, 1);
    sem_table->len++;
}

/**
 * It returns a semaphore with the specified name if
 * there is a semaphore in the semaphore table with
 * that name. Otherwise, NULL is returned.
 *
 * @param sem_table the semaphore table
 * @param name the semaphore name
 */
semaphore_t* semaphore_find(semaphore_table_t* sem_table, const char* name) {
    int i;
    for (i = 0; i < sem_table->len; i++)
        if (strcmp(sem_table->table[i].name, name) == 0)
            return &sem_table->table[i];
    return NULL;
}

/**
 * It request access to the specified semaphore.
 * If no resources is available then the current
 * process requesting the semaphore is blocked and
 * put into the semaphore's waiting list.
 *
 * @param sem a pointer to the semaphore
 * @param proc the process requesting access
 *             to this semaphore
 */
void semaphore_P(semaphore_t* sem) {
    sem_wait(&sem->mutex);
    sem->S--;
    if (sem->S < 0) {
    }
}

/* Semaphore Table Function Definitions */

/**
 * It initializes the semaphore table specified
 * by a pointer to a semaphore table.
 *
 * @param sem_table a pointer to the semaphore table
 */
void semaphore_table_init(semaphore_table_t* sem_table) {
    sem_table->table = (semaphore_t *)malloc(0);
    sem_table->len = 0;

    /* It checks if the semaphore table could not be allocated */
    if (!sem_table->table) {
        printf("Not enough memory to allocate the semaphore table.\n");
        exit(0);
    }
}