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
    char* proc_name = strdup(process->name);

    io_log_info_t* io_log = (io_log_info_t*)malloc(sizeof(io_log_info_t));
    io_log->type = IO_LOG_PRINT;
    io_log->print_log = (io_log_print_t *)malloc(sizeof(io_log_print_t));
    io_log->print_log->proc_name = proc_name;
    io_log->print_log->duration = duration;
    list_add(io_log_list, io_log);
    sem_post(&io_mutex);

    process->remaining -= duration;
    interruptControl(PRINT_FINISH, process);
}
