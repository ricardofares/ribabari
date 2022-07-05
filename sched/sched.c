#include <stdio.h>
#include <stdlib.h>

#define SCHED_HIGH_QUEUE_QUANTUM (10)
#define SCHED_LOW_QUEUE_QUANTUM (2000)

#include "sched.h"

/* Internal Function Prototypes */

/**
 * It creates a scheduler queue containing
 * the specified quantum-time (or time slice)
 * that each process in the queue may use to
 * execute its instructions.
 *
 * @param quantum the quantum-time (or time slice)
 *
 * @return a pointer to a scheduler queue or NULL
 *         if the queue could not be allocated.
 */
scheduler_queue_t* create_queue(int quantum);

/* Function Definitions */

/**
 * It initializes the scheduler.
 */
void scheduler_init() {
    kernel->scheduler->scheduled_proc = NULL;
    kernel->scheduler->high_queue = create_queue(SCHED_HIGH_QUEUE_QUANTUM);
    kernel->scheduler->low_queue = create_queue(SCHED_LOW_QUEUE_QUANTUM);
    kernel->scheduler->blocked_queue = create_queue(-1);
}

/**
 * It schedules the next process
 * to take the CPU.
 *
 * @param flags the flags to indicate to the
 *              scheduler what caused the
 *              process scheduling.
 */
void schedule_process(scheduler_flag_t flags) {
    scheduler_t* sched = kernel->scheduler;
    process_t* curr_scheduled = sched->scheduled_proc;
    process_t* new_scheduled = NULL;

    if (!list_empty(sched->high_queue->queue)) {
        new_scheduled = (process_t *)list_remove_head(sched->high_queue->queue)->content;
        if (new_scheduled)
            new_scheduled->remaining = sched->high_queue->quantum;
    } else if (!list_empty(sched->low_queue->queue)) {
        new_scheduled = (process_t *)list_remove_head(sched->low_queue->queue)->content;
        if (new_scheduled)
            new_scheduled->remaining = sched->low_queue->quantum;
    }

    /* It checks if there was a process in the CPU */
    if (curr_scheduled) {
        /* Did the process request I/O? or Has requested a resource (and blocked)? */
        if ((flags & IO_REQUESTED) || (flags & SEMAPHORE_BLOCKED)) {
            list_add(sched->blocked_queue->queue, curr_scheduled);
            curr_scheduled->state = BLOCKED;
        }
        /* Did the process complete its quantum time? */
        else if ((flags & QUANTUM_COMPLETED)) {
            list_add(sched->high_queue->queue, curr_scheduled);
            curr_scheduled->state = READY;
        }
    }

    if (new_scheduled)
        new_scheduled->state = RUNNING;

    sched->scheduled_proc = new_scheduled;
}

/* Internal Function Definitions */

/**
 * It creates a scheduler queue containing
 * the specified quantum-time (or time slice)
 * that each process in the queue may use to
 * execute its instructions.
 *
 * @param quantum the quantum-time (or time slice)
 *
 * @return a pointer to a scheduler queue or NULL
 *         if the queue could not be allocated.
 */
scheduler_queue_t* create_queue(int quantum) {
    scheduler_queue_t* queue = (scheduler_queue_t *)
            malloc(sizeof(scheduler_queue_t));

    /* It checks if the scheduler queue could not be allocated */
    if (!queue) {
        printf("Not enough memory to allocate the scheduler queue!\n");
        exit(0);
    }

    queue->queue = list_init();
    queue->quantum = quantum;
    return queue;
}