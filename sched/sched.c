#include <stdio.h>
#include <stdlib.h>

#define SCHED_HIGH_QUEUE_QUANTUM (1000)
#define SCHED_LOW_QUEUE_QUANTUM (2000)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#include "sched.h"

/* Scheduler Internal Function Prototypes */

static int proc_cmp(void* p1, void *p2) {
    return ((process_t *) p1)->id == ((process_t *) p2)->id;
}

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

/* Scheduler Function Definitions */

/**
 * It initializes the scheduler.
 *
 * @param scheduler a pointer to the scheduler
 *                 that will be initialized
 */
void scheduler_init(scheduler_t* scheduler) {
    scheduler->scheduled_proc = NULL;
    scheduler->high_queue = create_queue(SCHED_HIGH_QUEUE_QUANTUM);
    scheduler->low_queue = create_queue(SCHED_LOW_QUEUE_QUANTUM);
    scheduler->blocked_queue = create_queue(-1);
}

/**
 * It schedules the next process
 * to take the CPU.
 *
 * @param scheduler a pointer to the scheduler
 *                  that will schedule a process
 * @param flags the flags to indicate to the
 *              scheduler what caused the
 *              process scheduling.
 */
void schedule_process(scheduler_t* scheduler, scheduler_flag_t flags) {
    process_t* curr_scheduled = scheduler->scheduled_proc;
    process_t* new_scheduled = NULL;

    if (!list_empty(scheduler->high_queue->queue)) {
        new_scheduled = (process_t *)list_remove_head(scheduler->high_queue->queue)->content;
        if (new_scheduled)
            new_scheduled->remaining += scheduler->high_queue->quantum;
    } else if (!list_empty(scheduler->low_queue->queue)) {
        new_scheduled = (process_t *)list_remove_head(scheduler->low_queue->queue)->content;
        if (new_scheduled)
            new_scheduled->remaining += scheduler->low_queue->quantum;
    }

    /* It checks if there was a process in the CPU */
    if (curr_scheduled) {
        /* Did the process request I/O? or Has requested a resource (and blocked)? */
        if ((flags & IO_REQUESTED) || (flags & SEMAPHORE_BLOCKED)) {
            list_add(scheduler->blocked_queue->queue, curr_scheduled);
            curr_scheduled->state = BLOCKED;
        }
        /* Did the process complete its quantum time? */
        else if ((flags & QUANTUM_COMPLETED)) {
            list_add(scheduler->high_queue->queue, curr_scheduled);
            curr_scheduled->state = READY;
        }
    }

    if (new_scheduled)
        new_scheduled->state = RUNNING;

    scheduler->scheduled_proc = new_scheduled;
}

/**
 * It wakes up the specified process.
 *
 * @param scheduler the scheduler
 * @param proc the process
 */
void schedule_wake_process(scheduler_t* scheduler, process_t* proc) {
    list_node_t* proc_node;

    if (!(proc_node = list_search(scheduler->blocked_queue->queue, proc, proc_cmp)))
        return;

    list_remove_node(scheduler->blocked_queue->queue, proc_node);
    list_add(scheduler->high_queue->queue, proc);
}

/* Scheduler Internal Function Definitions */

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