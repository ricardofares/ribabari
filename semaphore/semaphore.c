#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../tools/constants.h"
#include "semaphore.h"

#ifndef OS_LOG_SYSTEM_LOADED
#define OS_LOG_SYSTEM_LOADED
#include "../terminal/log.h"
#endif // OS_LOG_SYSTEM_LOADED

#if OS_DEBUG || OS_SEM_DEBUG
#define LOG_SEM(fmt) printf(fmt)
#define LOG_SEM_A(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define LOG_SEM(fmt)
#define LOG_SEM_A(fmt, ...)
#endif // OS_DEBUG || OS_SEM_DEBUG

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
void semaphore_init(semaphore_t *sem, const char *name, const int S) {
    sem->name    = strdup(name);
    sem->S       = S;
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
void semaphore_register(semaphore_table_t *sem_table, const char *name) {
    semaphore_t *sem;
    semaphore_t *n_table;
    register int t;

    /* It checks if a semaphore with the specified name */
    /* has been already registered. */
    for (t = 0; t < sem_table->len; t++)
    {
      if (0 == strcmp(sem_table->table[t].name, name))
      {
        LOG_SEM_A("Semaphore %s has already been registered.\n", name);
        return;
      }
    }

    n_table = (semaphore_t *)
            realloc(sem_table->table, sizeof(semaphore_t) * (sem_table->len + 1));

    /* It checks if the request for reallocation of semaphore table */
    /* has been failed */
    if (!n_table) {
        printf("Not enough memory to reallocate the semaphore table.\n");
        exit(EXIT_FAILURE);
    }

    sem_table->table = n_table;
    sem_table->len   = sem_table->len + 1;
    sem              = &sem_table->table[sem_table->len-1];

    semaphore_init(sem, name, 1);

    LOG_SEM_A("Semaphore %s has been registered.\n", sem->name);
}

/**
 * It returns a semaphore with the specified name if
 * there is a semaphore in the semaphore table with
 * that name. Otherwise, NULL is returned.
 *
 * @param sem_table the semaphore table
 * @param name the semaphore name
 */
semaphore_t *semaphore_find(semaphore_table_t *sem_table, const char *name) {
    semaphore_t *sem;
    for (sem = sem_table->table; sem < &sem_table->table[sem_table->len]; sem++)
      if (0 == strcmp(sem->name, name))
        return sem;
    LOG_SEM_A("Semaphore %s could not be found.\n", name);
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
void semaphore_P(semaphore_t *sem, process_t *proc, void (*sleep)(void)) {
    int blocked = 0;

    sem_wait(&sem->mutex);
    sem->S--;
    if (sem->S < 0) {
        list_add(sem->waiters, proc);
        sleep();

        blocked = 1;
    }

    res_acq_log(proc->name, sem->name, 1, blocked);
    sem_post(&res_acq_mutex);

    sem_post(&sem->mutex);
}

/**
 * It releases an access to the specified semaphore.
 * If there are pending processes waiting for some
 * process to release its access, the least recently
 * requested process will be woken up.
 *
 * @param sem a pointer to the semaphore
 * @param process the process which is releasing
 *                the semaphore
 * @param wakeup a function whose invoke causes
 *               a process to wake up
 */
void semaphore_V(semaphore_t *sem, process_t *process, void (*wakeup)(process_t *)) {
    sem_wait(&sem->mutex);
    sem->S++;
    if (sem->S <= 0) {
        process_t* proc = list_remove_head(sem->waiters)->content;
        wakeup(proc);
    }

    res_acq_log(process->name, sem->name, 0, 0);
    sem_post(&res_acq_mutex);

    sem_post(&sem->mutex);
}

/* Semaphore Table Function Definitions */

/**
 * It initializes the semaphore table specified
 * by a pointer to a semaphore table.
 *
 * @param sem_table a pointer to the semaphore table
 */
void semaphore_table_init(semaphore_table_t *sem_table) {
  semaphore_t *n_table;

  n_table = (semaphore_t *)malloc(0);

  /* It checks if the semaphore table could not be allocated */
  if (!n_table) {
      printf("Not enough memory to allocate the semaphore table.\n");
      exit(EXIT_FAILURE);
  }

  sem_table->table = n_table;
  sem_table->len   = 0;
}
