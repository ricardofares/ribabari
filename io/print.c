#include "print.h"

#ifndef OS_SCHED_KERNEL
#define OS_SCHED_KERNEL
#include "../kernel/kernel.h"
#endif // OS_SCHED_KERNEL

/**
 * It requests a print operation.
 *
 * @param process the process which request
 *                the print operation.
 * @param duration the print duration
 */
void print_request(process_t* process, int duration) {
    io_print_log(process->name, duration);
    sem_post(&io_mutex);

    process->remaining -= duration;
    interruptControl(PRINT_FINISH, process);
}
