#include <stdio.h>
#include <string.h>

#define DEBUG 1
#include "kernel.h"

/* Internal Function Prototypes */

void process_create(const char* filename);

/**
 * It registers the semaphore with the specified
 * semaphore name, however, if the semaphore
 * already exists, then nothing is done.
 *
 * @param sem_name the semaphore name to be
 *                 registered
 */
void register_semaphore(const char* sem_name);

/* Kernel Function Definitions */

void kernel_init() {
    kernel = (kernel_t *)malloc(sizeof(kernel_t));

    /* It checks if the kernel could not be allocated */
    if (!kernel) {
        printf("Not enough memory to allocate the kernel.\n");
        exit(0);
    }

#if DEBUG
    printf("Kernel initialized.\n");
#endif // DEBUG

    kernel->scheduler = (scheduler_t *)malloc(sizeof(scheduler_t));

    /* It checks if the scheduler could not be allocated */
    if (!kernel->scheduler) {
        printf("Not enough memory to allocate the scheduler.\n");
        exit(0);
    }

    scheduler_init();

#if DEBUG
    printf("Scheduler initialized.\n");
#endif // DEBUG

    kernel->sem_table = (semaphore_t *)malloc(0);

    /* It checks if the semaphore table could not be allocated */
    if (!kernel->sem_table) {
        printf("Not enough memory to allocate the semaphore table.\n");
        exit(0);
    }

    kernel->sem_table_len = 0;

#if DEBUG
    printf("Semaphore table initialized.\n");
#endif // DEBUG
}

void sysCall(kernel_function_t func, void *arg) {
    switch (func) {
        case PROCESS_CREATE: {
            process_create((char *)arg);
            break;
        }
    }
}

/* Internal Function Definitions */

void read_semaphores(process_t* proc, char* sem_line) {
    const size_t len = strlen(sem_line);
    int sem_count = 1;
    int i;

    /* Count the amount of semaphores requested */
    for (i = 0; i < len; i++)
        if (sem_line[i] == ' ') sem_count++;

    proc->semaphores = (char **)malloc(sizeof(char *) * sem_count);

    /* It checks if the array of semaphores could not be allocated */
    if (!proc->semaphores) {
        printf("Semaphores could not be allocated.\n");
        exit(0);
    }

    char* tok = strtok(sem_line, " ");

    i = 0;
    do {
        proc->semaphores[i] = strdup(tok);
        register_semaphore(proc->semaphores[i]);
        i++;
    } while ((tok = strtok(NULL, " ")));
#if DEBUG
    printf("Process %s requested %d semaphores.\n", proc->name, sem_count);
    printf("Semaphores are: \n");
    for (i = 0; i < sem_count; i++)
        printf(" - %s\n", proc->semaphores[i]);
#endif
}

/**
 * It parses the synthetic program written in a file.
 * Further, if the parser has been successful then
 * a pointer to the created process is returned.
 * Otherwise, NULL is returned.
 *
 * @param filepath the filepath
 */
process_t* parse_synthetic_program(const char* filepath) {
    FILE *fp;
    process_t *proc;
    const int buflen = 256;
    char buf[buflen];

    /* It checks if the file could not be opened */
    if (!(fp = fopen(filepath, "r"))) {
        printf("A synthetic program located at %s"
               " could not be parsed because the"
               " file could not be opened.\n",
               filepath);
        return NULL;
    }

    proc = (process_t *)malloc(sizeof(process_t));

    /* It checks if the process could not be allocated */
    if (!proc) {
        printf("A memory block could not be"
               " allocated for a process.\n");
        return NULL;
    }

    /* Non-dependent file information */
    proc->pc = 0;
    proc->state = NEW;
    proc->remaining = 0;

    /* Dependent file information */
    fgets(buf, buflen, fp);
    proc->name = strdup(buf);

    fgets(buf, buflen, fp);
    proc->seg_id = atoi(buf);

    fgets(buf, buflen, fp);
    proc->priority = atoi(buf);

    fgets(buf, buflen, fp);
    proc->seg_size = atoi(buf);

    fgets(buf, buflen, fp);
    /* It checks if there are semaphores to be read */
    if (strcmp(buf, "\n") != 0)
        read_semaphores(proc, buf);

    return proc;
}

void process_create(const char* filename) {
    parse_synthetic_program(filename);
}

/**
 * It registers the semaphore with the specified
 * semaphore name, however, if the semaphore
 * already exists, then nothing is done.
 *
 * @param sem_name the semaphore name to be
 *                 registered
 */
void register_semaphore(const char* sem_name) {
    int i;

    /* It checks if a semaphore with the specified */
    /* name has been already registered. */
    for (i = 0; i < kernel->sem_table_len; i++)
        if (strcmp(kernel->sem_table[i].name, sem_name) == 0)
            return;

    kernel->sem_table = (semaphore_t *)realloc(
            kernel->sem_table, sizeof(semaphore_t) * (kernel->sem_table_len + 1));
    semaphore_init(&kernel->sem_table[kernel->sem_table_len], sem_name, 1);
    kernel->sem_table_len++;

#if DEBUG
    printf("Semaphore %s has been registered.\n", sem_name);
#endif // DEBUG
}