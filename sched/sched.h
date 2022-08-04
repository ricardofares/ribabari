#ifndef OS_PROJECT_SCHED_H
#define OS_PROJECT_SCHED_H

#include "../utils/list.h"
#include "../process/process.h"

/* Scheduler Structures Definitions */

/**
 * It is an enumeration that contains
 * the available flags to indicate what
 * causes the current process interruption.
 */
typedef enum SchedulerFlags {
    NONE = 0x0,

    /**
     * An I/O request caused the current
     * process to be interrupted. Further,
     * that process will be blocked for
     * further completion.
     */
    IO_REQUESTED = 0x1,

    /**
     * The time (quantum time) provided to
     * the process for completing its
     * activities has been expired.
     */
    QUANTUM_COMPLETED = 0x2,

    /**
     * A request for a semaphore has
     * blocked the process.
     */
    SEMAPHORE_BLOCKED = 0x4
} scheduler_flag_t;

typedef enum SchedulerQueueFlag {
    LOW_QUEUE = 0,
    HIGH_QUEUE = 1
} scheduler_queue_flag_t;

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

/**
 * It unblocks the specified process and
 * put it in the specified queue.
 *
 * @param scheduler the scheduler
 * @param proc the process
 * @param queue_flag the queue to put the process
 */
void schedule_unblock_process(scheduler_t* scheduler, process_t* proc, scheduler_queue_flag_t queue_flag);

#endif // OS_PROJECT_SCHED_H
