#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

/* Process Log Related Function Definitions */

/**
 * It initializes the process logging subsystem.
 */
void process_log_init() {
    /* It initializes the process log list */
    process_log_list = list_init();
}

/**
 * It creates a process log and signal the
 * process view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void process_log(const char* process_name, const int remaining, const int pc, const int sid, const int f_op_count) {
    proc_log_info_t* log = (proc_log_info_t *)malloc(sizeof(proc_log_info_t));

    /* It checks if the process log could not be allocated */
    if (!log) {
        printf("Not enough memory to allocate the process log.\n");
        exit(EXIT_FAILURE);
    }

    log->name = strdup(process_name);
    log->remaining = remaining;
    log->pc = pc;
    log->id = sid;
    log->is_proc = 1;
    log->f_op_count = f_op_count;

    /* Register the log into the process log list */
    list_add(process_log_list, log);
}

/**
 * It creates a process log and signal the
 * process view for further view update.
 * In this case, it is being logged that
 * no process is being executed.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void process_np_log() {
    proc_log_info_t* log = (proc_log_info_t *)malloc(sizeof(proc_log_info_t));

    /* It checks if the process log could not be allocated */
    if (!log) {
        printf("Not enough memory to allocate the process log.\n");
        exit(EXIT_FAILURE);
    }

    log->is_proc = 0;

    /* Register the log into the process log list */
    list_add(process_log_list, log);
}

/* Disk Log Related Function Definitions */

/**
 * It initializes the disk logging subsystem.
 */
void disk_log_init() {
    disk_general_log = (disk_log_t*)malloc(sizeof(disk_log_t));

    /* It checks if the disk log could not be allocated */
    if (!disk_general_log) {
        printf("Not enough memory to allocate the disk log.\n");
        exit(EXIT_FAILURE);
    }

    disk_general_log->r_req_count = 0;
    disk_general_log->w_req_count = 0;

    /* It initializes the disk log list */
    disk_log_list = list_init();
}

/**
 * It creates a disk log and signal the
 * disk log view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void disk_log(const char* process_name, const int proc_id,
              const int track, const int read, const int turnaround) {
    disk_log_info_t* log = (disk_log_info_t *)malloc(sizeof(disk_log_info_t));

    /* It checks if the disk log could not be allocated */
    if (!log) {
        printf("Not enough memory to allocate a disk log info.\n");
        exit(EXIT_FAILURE);
    }

    log->proc_name = strdup(process_name);
    log->is_read = read;
    log->track = track;
    log->proc_id = proc_id;
    log->turnaround = turnaround;

    /* It updates the disk general log information */
    if (read)
        disk_general_log->r_req_count++;
    else disk_general_log->w_req_count++;

    /* Register the log into the disk log list */
    list_add(disk_log_list, log);
}

/* I/O Log Related Function Definitions */

/**
 * It initializes the I/O logging subsystem.
 */
void io_log_init() {
    io_general_log = (io_log_t *)malloc(sizeof(io_log_t));

    /* It checks if the I/O log could not be allocated. */
    if (!io_general_log) {
        printf("Not enough memory to allocate the I/O log.\n");
        exit(EXIT_FAILURE);
    }

    io_general_log->r_bytes = 0;
    io_general_log->w_bytes = 0;
    io_general_log->p_time = 0;

    /* It initializes the log list */
    io_log_list = list_init();
}

/**
 * It creates an I/O file system log and signal the
 * I/O view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void io_fs_log(const char* process_name, const int inumber, const io_log_fs_flag_t opt) {
    io_log_info_t* log = (io_log_info_t *)malloc(sizeof(io_log_info_t));

    /* It checks if the I/O log could not be allocated */
    if (!log) {
        printf("Not enough memory to allocate an I/O log.\n");
        exit(EXIT_FAILURE);
    }

    log->fs_log = (io_log_fs_t *)malloc(sizeof(io_log_fs_t));

    /* It checks if the I/O file system log could not be allocated */
    if (!log->fs_log) {
        printf("Not enough memory to allocate an I/O file system log.\n");
        exit(EXIT_FAILURE);
    }

    log->type = IO_LOG_FILE_SYSTEM;
    log->fs_log->proc_name = strdup(process_name);
    log->fs_log->inumber = inumber;
    log->fs_log->opt = opt;

    /* It updates the I/O general log information */
    if (log->fs_log->opt & IO_LOG_FS_READ)
        io_general_log->r_bytes += (1 + (rand() & 65535));
    else if (log->fs_log->opt & IO_LOG_FS_WRITE)
        io_general_log->w_bytes += (1 + (rand() & 65535));

    /* Register the log into the I/O log list */
    list_add(io_log_list, log);
}

/**
 * It creates an I/O disk log and signal the
 * I/O view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void io_disk_log(const char* process_name, const int read) {
    io_log_info_t* log = (io_log_info_t *)malloc(sizeof(io_log_info_t));

    /* It checks if the I/O log could not be allocated */
    if (!log) {
        printf("Not enough memory to allocate an I/O log.\n");
        exit(EXIT_FAILURE);
    }

    log->disk_req_log = (io_log_disk_req_t *)malloc(sizeof(io_log_disk_req_t));

    /* It checks if the I/O disk log could not be allocated */
    if (!log->disk_req_log) {
        printf("Not enough memory to allocate an I/O disk log.\n");
        exit(EXIT_FAILURE);
    }

    log->type = IO_LOG_DISK_REQUEST;
    log->disk_req_log->proc_name = strdup(process_name);
    log->disk_req_log->read = read;

    /* Register the log into the I/O log list */
    list_add(io_log_list, log);
}

/**
 * It creates an I/O print log and signal the
 * I/O view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void io_print_log(const char* process_name, const int duration) {
    io_log_info_t* log = (io_log_info_t *)malloc(sizeof(io_log_info_t));

    /* It checks if the I/O log could not be allocated */
    if (!log) {
        printf("Not enough memory to allocate an I/O log.\n");
        exit(EXIT_FAILURE);
    }

    log->print_log = (io_log_print_t *)malloc(sizeof(io_log_print_t));

    /* It checks if the I/O print log could not be allocated */
    if (!log->print_log) {
        printf("Not enough memory to allocate an I/O print log.\n");
        exit(EXIT_FAILURE);
    }

    log->type = IO_LOG_PRINT;
    log->print_log->proc_name = strdup(process_name);
    log->print_log->duration = duration;

    /* It updates the I/O general log information */
    io_general_log->p_time += duration;

    /* Register the log into the I/O log list */
    list_add(io_log_list, log);
}

/* Resource Acquisition/Release Log Related Function Definitions */

/**
 * It initializes the resource acquisition/release
 * logging subsystem.
 */
void res_acq_log_init() {
    /* It initializes the resource acquisition/release log list */
    res_acq_log_list = list_init();
}

/**
 * It creates an resource acquisition/release
 * log and signal the resource acquisition view
 * for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void res_acq_log(const char* process_name, const char* sem_name, const int acq, const int blocked) {
    res_acq_log_t* log = (res_acq_log_t *)malloc(sizeof(res_acq_log_t));

    /* It checks if the resource acquisition/release could not be allocated */
    if (!log) {
        printf("Not enough memory to allocate an resource acquisition/release log.\n");
        exit(EXIT_FAILURE);
    }

    log->proc_name = process_name == NULL ? NULL : strdup(process_name);
    log->sem_name = strdup(sem_name);
    log->acq = acq;
    log->blocked = blocked;

    /* Register the log into the resource acquisition/release list */
    list_add(res_acq_log_list, log);
}
