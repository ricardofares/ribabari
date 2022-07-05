#ifndef OS_PROJECT_SCHED_H
#define OS_PROJECT_SCHED_H

#include "../process/process.h"

/* Scheduler Function Prototypes */

void scheduler_init();

/**
 * It schedules the next process that
 * will take the CPU.
 */
void schedule_process();

/* Scheduler Structure */

typedef struct SchedulerQueue {
    int quantum; /* or time slice */
} scheduler_queue_t;

typedef struct Scheduler {
    process_t* scheduled_proc;
} scheduler_t;

#ifndef OS_SCHED_KERNEL_LOADED
#define OS_SCHED_KERNEL_LOADED
    #include "../kernel/kernel.h"
#endif // OS_SCHED_KERNEL_LOADED

#endif // OS_PROJECT_SCHED_H
