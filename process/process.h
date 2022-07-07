#ifndef OS_PROJECT_PROCESS_H
#define OS_PROJECT_PROCESS_H

typedef enum Opcode {
    EXEC,
    READ,
    WRITE,
    PRINT,
    SEM_P,
    SEM_V
} opcode_t;

typedef struct Instruction {
    opcode_t op;
    int value;
    char* sem;
} instr_t;

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
    int pc; /* Program Counter */

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

#endif // OS_PROJECT_PROCESS_H
