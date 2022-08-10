#include "../constants.h"
#include "process.h"

#include <string.h>

#define BUF_LEN_PARSE (256)

#ifndef OS_SCHED_KERNEL
#define OS_SCHED_KERNEL
#include "../kernel/kernel.h"
#endif // OS_SCHED_KERNEL

/* Internal Process Function Prototypes */

/**
 * It reads the code specified in the synthetic
 * program and returns an array containing the code.
 *
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
static instr_t* read_code(char* buf, FILE *fp, int *code_len);


/**
 * It parses the synthetic program written in a file.
 * Further, if the parser has been successful then
 * a pointer to the created process is returned.
 * Otherwise, NULL is returned.
 *
 * @param filepath the filepath
 */
static process_t* parse_synthetic_program(FILE* fp, char* buf);

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
static void read_semaphores(process_t* proc, char* sem_line);

/* Process Function Definition */

/**
 * It creates the process read from the file
 * specified in the file path, and add this
 * file to the process control block (PCB).
 *
 * @param filepath the file path containing
 *                 the process specifications
 */
void process_create(const char* filepath) {
    FILE *fp;
    process_t* proc;
    instr_t *code;
    char buf[BUF_LEN_PARSE];

    /* It checks if the file could not be opened */
    if (!(fp = fopen(filepath, "r"))) {
        printf("A synthetic program located at %s"
               " could not be parsed because the"
               " file could not be opened.\n",
               filepath);
        exit(EXIT_FAILURE);
    }

    proc = parse_synthetic_program(fp, buf);
    code = read_code(buf, fp, &proc->code_len);

    memory_request_t memory_request;
    mem_req_init(&memory_request, proc, code);

    sysCall(MEM_LOAD_REQ, (void *) &memory_request);

#if OS_PROCESS_DEBUG
    printf("Process %s (%d) added into the process table.\n", proc->name, proc->id);
#endif // OS_PROCESS_DEBUG
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
        list_node_t* pcb_proc_node = list_search(kernel->proc_table, proc, proc_cmp);
        list_remove_node(kernel->proc_table, pcb_proc_node);

        /* If the process is running, then interrupt it */
        if (proc_cmp(kernel->scheduler.scheduled_proc, proc))
            sysCall(PROCESS_INTERRUPT, (void *) NONE);

        segment_free(&kernel->seg_table, proc->seg_id);

        /* Remove the node from the scheduler queues */
        list_node_t* sched_proc_node;

        if ((sched_proc_node = list_search(kernel->scheduler.high_queue->queue, proc, proc_cmp)))
            list_remove_node(kernel->scheduler.high_queue->queue, sched_proc_node);
        else if ((sched_proc_node = list_search(kernel->scheduler.low_queue->queue, proc, proc_cmp)))
            list_remove_node(kernel->scheduler.low_queue->queue, sched_proc_node);
        else if ((sched_proc_node = list_search(kernel->scheduler.blocked_queue->queue, proc, proc_cmp)))
            list_remove_node(kernel->scheduler.blocked_queue->queue, sched_proc_node);

#if OS_PROCESS_DEBUG
        printf("Process %s has been finished.\n", proc->name);
#endif // OS_PROCESS_DEBUG

        /* It frees the process allocated memory */
        free(proc->name);
        free(proc->semaphores);
        free(proc);
    }
}

/**
 * It returns 1 if both process has the same id.
 * Otherwise, 0 is returned.
 *
 * @param p1 the first process
 * @param p2 the second process
 *
 * @return 1 if both process has the same id;
 *         otherwise, 0 is returned.
 */
int proc_cmp(void* p1, void *p2) {
    return ((process_t *) p1)->id == ((process_t *) p2)->id;
}


/* Internal Process Function Definition */

/**
 * It parses the synthetic program written in a file.
 * Further, if the parser has been successful then
 * a pointer to the created process is returned.
 * Otherwise, NULL is returned.
 *
 * @param filepath the filepath
 */
static process_t* parse_synthetic_program(FILE* fp, char* buf) {
    process_t *proc = (process_t *)malloc(sizeof(process_t));

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
    proc->o_files = list_init();

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
    return proc;
}

/**
 * It reads the code specified in the synthetic
 * program and returns an array containing the code.
 *
 * @param buf the buf used to read the line
 *            containing the code. The buffer must be
 *            big enough to contain the instruction line.
 * @param fp the file containing the process code
 *
 * @return an array of instructions read from a file
 *         that specifies the process.
 */
static instr_t* read_code(char* buf, FILE *fp, int *code_len) {
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
static void read_semaphores(process_t* proc, char* sem_line) {
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
#if OS_PROCESS_DEBUG
    printf("Process %s requested %d semaphores.\n", proc->name, sem_count);
    printf("Semaphores are: \n");
    for (i = 0; i < sem_count; i++)
        printf(" - %s\n", proc->semaphores[i]);
#endif
}

/* Process File System Related Function Prototypes */

/**
 * It returns 1 if this process has already opened
 * the inode with the specified inumber. Otherwise,
 * 0 is returned.
 *
 * @param process the process to be checked
 * @param inumber the inode number
 *
 * @return 1 if this process has already opened
 *            the inode with the specified inumber.
 *            Otherwise, 0 is returned.
 */
int has_opened_file(process_t* process, int inumber) {
    FOREACH(process->o_files, int) {
        if (it == inumber)
            return 1;
    }

    return 0;
}