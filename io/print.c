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
    io_log_info_t* new_io_log = (io_log_info_t*)malloc(sizeof(io_log_info_t));

    char* proc_name = strdup(process->name);
    (*new_io_log) = (io_log_info_t) {
        .proc_id = process->id,
        .proc_name = proc_name,
        .duration = duration,
    };
    list_add(io_log_list, (void*)new_io_log);
    sem_post(&io_mutex);

    process->remaining -= duration;
    interruptControl(PRINT_FINISH, process);
}
