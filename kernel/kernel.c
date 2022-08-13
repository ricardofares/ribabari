#include <stdio.h>

#include "../constants.h"
#include "../utils/math.h"
#include "kernel.h"

#if OS_DEBUG || OS_KERNEL_DEBUG
#define LOG_KERNEL(fmt) printf(fmt)
#define LOG_KERNEL_A(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define LOG_KERNEL(fmt)
#define LOG_KERNEL_A(fmt, ...)
#endif // OS_DEBUG || OS_KERNEL_DEBUG

#if OS_DEBUG || OS_KERNEL_DEBUG || OS_KERNEL_EVAL_DEBUG
#define LOG_KERNEL_EVAL(fmt) printf(fmt)
#define LOG_KERNEL_EVAL_A(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define LOG_KERNEL_EVAL(fmt)
#define LOG_KERNEL_EVAL_A(fmt, ...)
#endif // OS_DEBUG || OS_KERNEL_DEBUG || OS_KERNEL_EVAL_DEBUG

/* Internal Function Prototypes */

/**
 * It causes the process who invoked this
 * function to sleep, that is, to be blocked.
 */
static void sleep();

/**
 * It causes the specified process to be
 * wake up, that is, to be unblocked.
 *
 * @param proc the process to be unblocked.
 */
static void wakeup(process_t* proc);

/* Kernel Function Definitions */

/**
 * It initializes the kernel and its
 * underlying structures.
 */
void kernel_init() {
    kernel = (kernel_t *)malloc(sizeof(kernel_t));

    /* It checks if the kernel could not be allocated */
    if (!kernel) {
        printf("Not enough memory to allocate the kernel.\n");
        exit(0);
    }

    kernel->proc_table = list_init();
    kernel->next_proc_id = 1; /* 0 is for the kernel */
    LOG_KERNEL("Process table initialized.\n");

    segment_table_init(&kernel->seg_table);
    LOG_KERNEL("Segment table initialized.\n");

    scheduler_init(&kernel->scheduler);
    LOG_KERNEL("Scheduler initialized.\n");

    disk_scheduler_init(&kernel->disk_scheduler);
    LOG_KERNEL("Disk Scheduler initialized.\n");

    semaphore_table_init(&kernel->sem_table);
    LOG_KERNEL("Semaphore table initialized.\n");

    file_table_init(&kernel->file_table);
    LOG_KERNEL("File table initialized.\n");
    LOG_KERNEL("Kernel initialized.\n");
}

/**
 * It makes a system call specifying by a kernel
 * function flag what the operating system must
 * to do.
 *
 * @param func the kernel function
 * @param arg a generic argument
 */
void sysCall(kernel_function_t func, void *arg) {
    switch (func) {
        case PROCESS_INTERRUPT: {
            schedule_process(&kernel->scheduler, (scheduler_flag_t)arg);
            break;
        }
        case PROCESS_CREATE: {
            process_create((char *)arg);
            break;
        }
        case PROCESS_FINISH: {
            process_finish((process_t *)arg);
            break;
        }
        case MEM_LOAD_REQ: {
            /* It requests the memory a portion of itself */
            mem_req_load((memory_request_t *)arg, &kernel->seg_table);

            /* It signalizes the kernel that the memory requested has been allocated */
            interruptControl(MEM_LOAD_FINISH, (memory_request_t *)arg);
            break;
        }
        case SEMAPHORE_P: {
            semaphore_P((semaphore_t *)arg, kernel->scheduler.scheduled_proc, sleep);
            break;
        }
        case SEMAPHORE_V: {
            semaphore_V((semaphore_t *)arg, kernel->scheduler.scheduled_proc, wakeup);
            break;
        }
        case DISK_READ_REQUEST: {
            const int track = (int)arg;
            process_t* curr_proc = kernel->scheduler.scheduled_proc;

            /* It schedules a next process and put the current one into the blocked queue */
            schedule_process(&kernel->scheduler, IO_REQUESTED);

            /* It requests a disk read operation for the specified track */
            disk_request(curr_proc, &kernel->disk_scheduler, track, 1);

            fs_op_request_t fs_req;
            fs_req.proc = curr_proc;
            fs_req.track = track;
            fs_req.read = 1;

            /* It requests a file system request for handling the file */
            /* that is being read at the specified track in the disk */
            sysCall(FS_REQUEST, (void *)(&fs_req));
            break;
        }
        case DISK_WRITE_REQUEST: {
            const int track = (int)arg;
            process_t* curr_proc = kernel->scheduler.scheduled_proc;

            /* It schedules a next process and put the current one into the blocked queue */
            schedule_process(&kernel->scheduler, IO_REQUESTED);

            /* It requests a disk write operation for the specified track */
            disk_request(curr_proc, &kernel->disk_scheduler, track, 0);

            fs_op_request_t fs_req;
            fs_req.proc = curr_proc;
            fs_req.track = track;
            fs_req.read = 0;

            /* It requests a file system request for handling the file */
            /* that is being written at the specified track in the disk */
            sysCall(FS_REQUEST, (void *)(&fs_req));
            break;
        }
        case PRINT_REQUEST: {
            process_t* curr_proc = kernel->scheduler.scheduled_proc;

            /* It schedules a next process and put the current one into the blocked queue */
            schedule_process(&kernel->scheduler, IO_REQUESTED);

            print_request(curr_proc, (int)arg);
            break;
        }
        case FS_REQUEST: {
            fs_op_request_t* fs_req = (fs_op_request_t *)arg;

            if (fs_req->read)
                fs_read_request(&kernel->file_table, fs_req->proc, DISK_BLOCK(fs_req->track));
            else fs_write_request(&kernel->file_table, fs_req->proc, DISK_BLOCK(fs_req->track));
            break;
        }
    }
}

/**
 * It makes a interrupt control specifying what must
 * be signalized to the operating system.
 *
 * @param func the signalization flag
 * @param arg a generic argument
 */
void interruptControl(kernel_function_t func, void *arg) {
    switch (func) {
        case MEM_LOAD_FINISH: {
            memory_request_t* req = (memory_request_t *)arg;
            process_t* proc = req->proc;

            /* Add the process into the PCB */
            list_add(kernel->proc_table, proc);

            /* Add the process into the scheduling queue */
            proc->state = READY;
            if (proc->priority == 1)
                list_add(kernel->scheduler.high_queue->queue, proc);
            else list_add(kernel->scheduler.low_queue->queue, proc);
            break;
        }
        /* When the process comes back from an I/O request it is put at the low queue */
        case DISK_FINISH: {
            schedule_unblock_process(&kernel->scheduler, (process_t *)arg, LOW_QUEUE);
            break;
        }
        /* When the process comes back from an I/O request it is put at the low queue */
        case PRINT_FINISH: {
            schedule_unblock_process(&kernel->scheduler, (process_t *)arg, LOW_QUEUE);
            break;
        }
    }
}

/**
 * It evaluates the instruction that has been
 * provided by the specified process' code
 * segment.
 *
 * @param proc the process executing the instruction
 * @param instr the instruction to be evaluated
 */
void eval(process_t* proc, instr_t* instr) {
    switch (instr->op) {
    case EXEC: {
        LOG_KERNEL_EVAL_A("Process %s will execute for %d u.t.\n", proc->name, instr->value);

        proc->remaining = proc->remaining - instr->value;

        LOG_KERNEL_EVAL_A("Process %s has finished its executing.\n", proc->name);
        break;
    }
    case SEM_P: {
        LOG_KERNEL_EVAL_A("Process %s has requested for semaphore %s.\n", proc->name, instr->sem);

        sysCall(SEMAPHORE_P, semaphore_find(&kernel->sem_table, instr->sem));
        /* It checks if the process has not been blocked */
        /* after a semaphore request */
        if (proc->state != BLOCKED)
            proc->remaining = MAX(0, proc->remaining - 200);
        break;
    }
    case SEM_V: {
        LOG_KERNEL_EVAL_A("Process %s has released semaphore %s.\n", proc->name, instr->sem);

        sysCall(SEMAPHORE_V, semaphore_find(&kernel->sem_table, instr->sem));
        proc->remaining = MAX(0, proc->remaining - 200);
        break;
    }
    case READ: {
        LOG_KERNEL_EVAL_A("Process %s has requested a read operation at track %d.\n", proc->name, instr->value);

        sysCall(DISK_READ_REQUEST, instr->value);
        break;
    }
    case WRITE: {
        LOG_KERNEL_EVAL_A("Process %s has requested a write operation at track %d.\n", proc->name, instr->value);

        sysCall(DISK_WRITE_REQUEST, instr->value);
        break;
    }
    case PRINT: {
        LOG_KERNEL_EVAL_A("Process %s has requested a print operation for %d u.t.\n", proc->name, instr->value);

        sysCall(PRINT_REQUEST, instr->value);
        break;
    }
    }
}

/* Internal Function Definitions */

/**
 * It causes the process who invoked this
 * function to sleep, that is, to be blocked.
 */
static void sleep() {
    /* It causes the current scheduled process to be blocked */
    /* in which that has been caused by a semaphore request */
    sysCall(PROCESS_INTERRUPT, (void *) SEMAPHORE_BLOCKED);
}

/**
 * It causes the specified process to be
 * wake up, that is, to be unblocked.
 *
 * @param proc the process to be unblocked.
 */
static void wakeup(process_t* proc) {
    schedule_unblock_process(&kernel->scheduler, proc, HIGH_QUEUE);
}
