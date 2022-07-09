#ifndef OS_PROJECT_KERNEL_H
#define OS_PROJECT_KERNEL_H

#include "../memory/memory.h"
#include "../sched/sched.h"
#include "../process/instruction.h"

typedef enum KernelFunction {
    /* Process Related Functions */
    PROCESS_INTERRUPT = 1,
    PROCESS_CREATE = 2,
    PROCESS_FINISH = 3,

    /* Disk Related Functions */
    DISK_REQUEST = 4,
    DISK_FINISH = 5,

    /* Memory Related Functions */
    MEM_LOAD_REQ = 6,
    MEM_LOAD_FINISH = 7,

    /* File System Related Functions */
    FS_REQUEST = 8,
    FS_FINISH = 9,

    /* Resource Acquisition Related Functions */
    SEMAPHORE_P = 10,
    SEMAPHORE_V = 11,

    /* Print Related Functions */
    PRINT_REQUEST = 14,
    PRINT_FINISH = 15
} kernel_function_t;

typedef struct Kernel {
    /* Process Table Information (aka PCB) */
    list_t* proc_table;
    int next_proc_id;

    /* Segment Table Information */
    segment_table_t* seg_table;

    scheduler_t* scheduler;

    /* Semaphore Table Information */
    semaphore_table_t sem_table;

    int pc; /* Program Counter */
} kernel_t;

extern kernel_t* kernel;

void kernel_init();

void interruptControl(kernel_function_t func, void *arg);

void sysCall(kernel_function_t func, void *arg);

#endif // OS_PROJECT_KERNEL_H

