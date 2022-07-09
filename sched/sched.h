#ifndef OS_PROJECT_SCHED_H
#define OS_PROJECT_SCHED_H

#include "../utils/list.h"
#include "../process/process.h"

/* Scheduler Structures Definitions */

typedef enum SchedulerFlags {
    NONE = 0x0,
    IO_REQUESTED = 0x1,
    QUANTUM_COMPLETED = 0x2,
    SEMAPHORE_BLOCKED = 0x4
} scheduler_flag_t;

typedef struct SchedulerQueue {
    list_t* queue;
    int quantum; /* or time slice */
} scheduler_queue_t;

typedef struct Scheduler {
    process_t* scheduled_proc;

    scheduler_queue_t* high_queue; /* High priority queue */
    scheduler_queue_t* low_queue; /* Low priority queue */
    scheduler_queue_t* blocked_queue; /* Blocked queue */
} scheduler_t;

/* Scheduler Function Prototypes */

/**
 * It initializes the scheduler.
 *
 * @param scheduler a pointer to the scheduler
 *                 that will be initialized
 */
void scheduler_init(scheduler_t* scheduler);

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
void schedule_process(scheduler_t* scheduler, scheduler_flag_t flags);

#endif // OS_PROJECT_SCHED_H
