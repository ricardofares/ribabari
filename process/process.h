#ifndef OS_PROJECT_PROCESS_H
#define OS_PROJECT_PROCESS_H

#define FETCH_INSTR_ADDR(x) ((x)->pc++)

typedef enum ProcessState {
    NEW,
    BLOCKED,
    READY,
    RUNNING,
    TERMINATED
} process_state_t;

typedef struct Process {
    /* General Information */
    char *name;
    int id;
    int pc; /* Program Counter */
    int code_len;

    /* Memory Information */
    int seg_id;
    int seg_size;

    /* Scheduling Information */
    int priority;
    int remaining;
    process_state_t state;

    /* Resource Acquisition Information */
    char **semaphores;
} process_t;

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
int proc_cmp(void* p1, void *p2);

#endif // OS_PROJECT_PROCESS_H
