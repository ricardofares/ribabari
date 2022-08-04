#ifndef OS_PROJECT_KERNEL_H
#define OS_PROJECT_KERNEL_H

#include "../memory/memory.h"
#include "../sched/sched.h"
#include "../process/instruction.h"
#include "../disk/disk.h"

typedef enum KernelFunction {
    /* Process Related Functions */
    PROCESS_INTERRUPT = 1,
    PROCESS_CREATE = 2,
    PROCESS_FINISH = 3,

    /* DiskScheduler Related Functions */
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
    segment_table_t seg_table;

    /* Scheduler Information */
    scheduler_t scheduler;

    /* Disk Scheduler Information */
    disk_scheduler_t disk_scheduler;

    /* Semaphore Table Information */
    semaphore_table_t sem_table;

    int pc; /* Program Counter */
} kernel_t;

/* Kernel Variables */

/**
 * A pointer to the kernel structure. This
 * variable must be initialized and will be
 * when the kernel_init function is called.
 */
extern kernel_t* kernel;

/* Kernel Function Prototypes */

/**
 * It initializes the kernel and its
 * underlying structures.
 */
void kernel_init();

/**
 * It makes a interrupt control specifying what must
 * be signalized to the operating system.
 *
 * @param func the signalization flag
 * @param arg a generic argument
 */
void interruptControl(kernel_function_t func, void *arg);

/**
 * It makes a system call specifying by a kernel
 * function flag what the operating system must
 * to do.
 *
 * @param func the kernel function
 * @param arg a generic argument
 */
void sysCall(kernel_function_t func, void *arg);

/**
 * It evaluates the instruction that has been
 * provided by the specified process' code
 * segment.
 *
 * @param proc the process executing the instruction
 * @param instr the instruction to be evaluated
 */
void eval(process_t* proc, instr_t* instr);

#endif // OS_PROJECT_KERNEL_H

