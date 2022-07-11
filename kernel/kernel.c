#include <stdio.h>
#include <string.h>

#define BUF_LEN_PARSE (256)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define DEBUG 1
#define OS_EVAL_DEBUG 0

#include "kernel.h"

/* Internal Function Prototypes */

/**
 * It creates the process read from the file
 * specified in the file path, and add this
 * file to the process control block (PCB).
 *
 * @param filepath the file path containing
 *                 the process specifications
 */
void process_create(const char* filepath);

/**
 * It finishes the specified process removing
 * it from the PCB and from schedulable process
 * queue. Further, the memory allocated for the
 * process is freed.
 *
 * @param proc the process to be finished
 */
void process_finish(process_t* proc);

/**
 * It parses the synthetic program written in a file.
 * Further, if the parser has been successful then
 * a pointer to the created process is returned.
 * Otherwise, NULL is returned.
 *
 * @param filepath the filepath
 */
process_t* parse_synthetic_program(const char* filepath);

/**
 * It reads the semaphores specified in the
 * synthetic program. Further, the semaphores
 * read are registered as well.
 *
 * @param proc the process that requested
 *             the semaphores
 * @param sem_line the semaphore line specified
 *                 in the synthetic program that
 *                 contains the requested semaphores.
 */
void read_semaphores(process_t* proc, char* sem_line);

/**
 * It reads the code specified in the synthetic
 * program and returns an array containing the code.
 *
 * @param proc the process containing that code
 * @param buf the buf used to read the line
 *            containing the code. The buffer must be
 *            big enough to contain the instruction line.
 * @param fp the file containing the process code
 * @param code_len a pointer to an integer used to
 *                 set the value for contain the code
 *                 length.
 *
 * @return an array of instructions read from a file
 *         that specifies the process.
 */
instr_t* read_code(process_t* proc, char* buf, FILE *fp, int *code_len);

/**
 * It returns the value 1 if the specified processes
 * can be matched. Otherwise, returns the value 0.
 *
 * @param fid the first process
 * @param sid the second process
 *
 * @return the value 1 if the processes has been
 *         matched; otherwise, returns 0.
 */
static int process_comparator(void *p1, void *p2);

/**
 * It causes the process who invoked this
 * function to sleep, that is, to be blocked.
 */
void sleep();

/**
 * It causes the specified process to be
 * wake up, that is, to be unblocked.
 *
 * @param proc the process to be unblocked.
 */
void wakeup(process_t* proc);

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

#if DEBUG
    printf("Kernel initialized.\n");
#endif // DEBUG

    kernel->proc_table = list_init();
    kernel->next_proc_id = 1; /* 0 is for the kernel */

#if DEBUG
    printf("Process table initialized.\n");
#endif // DEBUG

    segment_table_init(&kernel->seg_table);

#if DEBUG
    printf("Segment table initialized.\n");
#endif // DEBUG

    scheduler_init(&kernel->scheduler);

#if DEBUG
    printf("Scheduler initialized.\n");
#endif // DEBUG

    semaphore_table_init(&kernel->sem_table);

#if DEBUG
    printf("Semaphore table initialized.\n");
#endif // DEBUG
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
            mem_req_load((memory_request_t *)arg, &kernel->seg_table);
            break;
        }
        case SEMAPHORE_P: {
            semaphore_P((semaphore_t *)arg, kernel->scheduler.scheduled_proc, sleep);
            break;
        }
        case SEMAPHORE_V: {
            semaphore_V((semaphore_t *)arg, wakeup);
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
#if OS_EVAL_DEBUG
        printf("Process %s will execute for %d u.t.\n", proc->name, instr->value);
#endif // OS_EVAL_DEBUG
        proc->remaining = proc->remaining - instr->value;
#if OS_EVAL_DEBUG
        printf("Process %s has finished its executing.\n", proc->name);
#endif
        break;
    }
    case SEM_P: {
#if OS_EVAL_DEBUG
        printf("Process %s has requested for semaphore %s.\n", proc->name, instr->sem);
#endif // OS_EVAL_DEBUG
        sysCall(SEMAPHORE_P, semaphore_find(&kernel->sem_table, instr->sem));
        /* It checks if the process has not been blocked */
        /* after a semaphore request */
        if (proc->state != BLOCKED)
            proc->remaining = MAX(0, proc->remaining - 200);
        break;
    }
    case SEM_V: {
#if OS_EVAL_DEBUG
        printf("Process %s has released semaphore %s.\n", proc->name, instr->sem);
#endif // OS_EVAL_DEBUG
        sysCall(SEMAPHORE_V, semaphore_find(&kernel->sem_table, instr->sem));
        proc->remaining = MAX(0, proc->remaining - 200);
        break;
    }
    }
}

/* Internal Function Definitions */

/**
 * It creates the process read from the file
 * specified in the file path, and add this
 * file to the process control block (PCB).
 *
 * @param filepath the file path containing
 *                 the process specifications
 */
void process_create(const char* filepath) {
    process_t* proc = parse_synthetic_program(filepath);

    /* Add the process into the PCB */
    list_add(kernel->proc_table, proc);

    /* Add the process into the scheduling queue */
    proc->state = READY;
    if (proc->priority == 1)
        list_add(kernel->scheduler.high_queue->queue, proc);
    else list_add(kernel->scheduler.low_queue->queue, proc);
#if DEBUG
    printf("Process %s (%d) added into the process table.\n", proc->name, proc->id);
#endif // DEBUG
}

/**
 * It finishes the specified process removing
 * it from the PCB and from schedulable process
 * queue. Further, the memory allocated for the
 * process is freed.
 *
 * @param proc the process to be finished
 */
void process_finish(process_t* proc) {
    if (proc) {
        /* Remove the node from the PCB */
        list_node_t* pcb_proc_node = list_search(kernel->proc_table, proc, process_comparator);
        list_remove_node(kernel->proc_table, pcb_proc_node);

        /* If the process is running, then interrupt it */
        if (process_comparator(kernel->scheduler.scheduled_proc, proc))
            sysCall(PROCESS_INTERRUPT, (void *) NONE);

        segment_free(&kernel->seg_table, proc->seg_id);

        /* Remove the node from the scheduler queues */
        list_node_t* sched_proc_node;

        if ((sched_proc_node = list_search(kernel->scheduler.high_queue->queue, proc, process_comparator)))
            list_remove_node(kernel->scheduler.high_queue->queue, sched_proc_node);
        else if ((sched_proc_node = list_search(kernel->scheduler.low_queue->queue, proc, process_comparator)))
            list_remove_node(kernel->scheduler.low_queue->queue, sched_proc_node);
        else if ((sched_proc_node = list_search(kernel->scheduler.blocked_queue->queue, proc, process_comparator)))
            list_remove_node(kernel->scheduler.blocked_queue->queue, sched_proc_node);

#if DEBUG
        printf("Process %s has been finished.\n", proc->name);
#endif // DEBUG

        /* It frees the process allocated memory */
        free(proc->name);
        free(proc->semaphores);
        free(proc);
    }
}

/**
 * It causes the process who invoked this
 * function to sleep, that is, to be blocked.
 */
void sleep() {
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
void wakeup(process_t* proc) {
    schedule_wake_process(&kernel->scheduler, proc);
}

/**
 * It parses the synthetic program written in a file.
 * Further, if the parser has been successful then
 * a pointer to the created process is returned.
 * Otherwise, NULL is returned.
 *
 * @param filepath the filepath
 */
process_t* parse_synthetic_program(const char* filepath) {
    FILE *fp;
    process_t *proc;
    char buf[BUF_LEN_PARSE];

    /* It checks if the file could not be opened */
    if (!(fp = fopen(filepath, "r"))) {
        printf("A synthetic program located at %s"
               " could not be parsed because the"
               " file could not be opened.\n",
               filepath);
        exit(0);
    }

    proc = (process_t *)malloc(sizeof(process_t));

    /* It checks if the process could not be allocated */
    if (!proc) {
        printf("A memory block could not be"
               " allocated for a process.\n");
        exit(0);
    }

    /* Non-dependent file information */
    proc->id = kernel->next_proc_id++;
    proc->pc = 0;
    proc->state = NEW;
    proc->remaining = 0;

    /* Dependent file information */
    fgets(buf, BUF_LEN_PARSE, fp);
    buf[strlen(buf) - 1] = '\0';
    proc->name = strdup(buf);

    fgets(buf, BUF_LEN_PARSE, fp);
    proc->seg_id = atoi(buf);

    fgets(buf, BUF_LEN_PARSE, fp);
    proc->priority = atoi(buf);

    fgets(buf, BUF_LEN_PARSE, fp);
    proc->seg_size = atoi(buf);

    fgets(buf, BUF_LEN_PARSE, fp);
    buf[strlen(buf) - 1] = '\0';
    /* It checks if there are semaphores to be read */
    if (strcmp(buf, "\n") != 0)
        read_semaphores(proc, buf);

    /* Jump the new line */
    fgets(buf, BUF_LEN_PARSE, fp);

    int code_len = 0;
    instr_t *code = read_code(proc, buf, fp, &code_len);
    proc->code_len = code_len;

    memory_request_t memory_request;
    mem_req_init(&memory_request, proc->id, proc->seg_id, proc->seg_size, code, code_len);

    sysCall(MEM_LOAD_REQ, (void *) &memory_request);
    return proc;
}

/**
 * It reads the semaphores specified in the
 * synthetic program. Further, the semaphores
 * read are registered as well.
 *
 * @param proc the process that requested
 *             the semaphores
 * @param sem_line the semaphore line specified
 *                 in the synthetic program that
 *                 contains the requested semaphores.
 */
void read_semaphores(process_t* proc, char* sem_line) {
    const size_t len = strlen(sem_line);
    int sem_count = 1;
    int i;

    /* Count the amount of semaphores requested */
    for (i = 0; i < len; i++)
        if (sem_line[i] == ' ') sem_count++;

    proc->semaphores = (char **)malloc(sizeof(char *) * sem_count);

    /* It checks if the array of semaphores could not be allocated */
    if (!proc->semaphores) {
        printf("Semaphores could not be allocated.\n");
        exit(0);
    }

    char* tok = strtok(sem_line, " ");

    i = 0;
    do {
        proc->semaphores[i] = strdup(tok);
        semaphore_register(&kernel->sem_table, proc->semaphores[i]);
        i++;
    } while ((tok = strtok(NULL, " ")));
#if DEBUG
    printf("Process %s requested %d semaphores.\n", proc->name, sem_count);
    printf("Semaphores are: \n");
    for (i = 0; i < sem_count; i++)
        printf(" - %s\n", proc->semaphores[i]);
#endif
}

/**
 * It reads the code specified in the synthetic
 * program and returns an array containing the code.
 *
 * @param proc the process containing that code
 * @param buf the buf used to read the line
 *            containing the code. The buffer must be
 *            big enough to contain the instruction line.
 * @param fp the file containing the process code
 *
 * @return an array of instructions read from a file
 *         that specifies the process.
 */
instr_t* read_code(process_t* proc, char* buf, FILE *fp, int *code_len) {
    instr_t* code;
    long int code_section;
    int i;

    code_section = ftell(fp);
    (*code_len) = 0;

    /* It calculates the code length */
    while (fgets(buf, BUF_LEN_PARSE, fp))
        (*code_len)++;
    fseek(fp, code_section, SEEK_SET);

    code = (instr_t *)malloc(sizeof(instr_t) * (*code_len));

    /* It checks if the code could not be allocated */
    if (!code) {
        printf("Not enough memory to allocate the code.\n");
        exit(0);
    }

    /* It parses the program code */
    i = 0;
    while (fgets(buf, BUF_LEN_PARSE, fp))
        instr_parse(&code[i++], buf, &kernel->sem_table);

    return code;
}

/**
 * It returns the value 1 if the specified processes
 * can be matched. Otherwise, returns the value 0.
 *
 * @param fid the first process
 * @param sid the second process
 *
 * @return the value 1 if the processes has been
 *         matched; otherwise, returns 0.
 */
static int process_comparator(void *p1, void *p2) {
    return ((process_t *)p1)->id == ((process_t *) p2)->id;
}
