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
 */
void scheduler_init();

/**
 * It schedules the next process
 * to take the CPU.
 *
 * @param flags the flags to indicate to the
 *              scheduler what caused the
 *              process scheduling.
 */
void schedule_process(scheduler_flag_t flags);

#ifndef OS_SCHED_KERNEL_LOADED
#define OS_SCHED_KERNEL_LOADED
    #include "../kernel/kernel.h"
#endif // OS_SCHED_KERNEL_LOADED

#endif // OS_PROJECT_SCHED_H
