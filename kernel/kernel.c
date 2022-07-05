#include <stdio.h>
#include <string.h>

#define DEBUG 1
#include "kernel.h"

/* Internal Function Prototypes */

void process_created(const char* filename);

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
}

void sysCall(kernel_function_t func, void *arg) {
}

/* Internal Function Definitions */

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

    return proc;
}