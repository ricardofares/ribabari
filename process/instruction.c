#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instruction.h"

/* [Internal] Instruction Function Prototypes */

/**
 * It parses a semaphore request or release instruction, that
 * instruction's format are P(sem_name) or V(sem_name). Further,
 * if the semaphore used in that instruction has not been previously
 * registered then the application is automatically terminated.
 *
 * @param instr a pointer to an instruction
 * @param line the instruction line
 * @param sem_table a pointer to a semaphore table
 */
void instr_semaphore_parse(instr_t* instr, const char* line, semaphore_table_t* sem_table) {
    char* sem_name = strdup(line);
    semaphore_t* sem;

    sem_name = sem_name + 2; /* jump P( or V( */
    sem_name[strlen(sem_name) - 2] = '\0';

    sem = semaphore_find(sem_table, sem_name);

    /* It checks if the instruction is trying to request or release */
    /* a semaphore that has not been previously registered. */
    if (!sem) {
        printf("A semaphore with the name %s has not been registered.\n", sem_name);
        exit(0);
    }

    instr->op = line[0] == 'P' ? SEM_P : SEM_V;
    instr->sem = sem_name;
}

/* Instruction Function Definitions  */

/**
 * It parses the instruction specified in the
 * instruction line. The pointer to an instruction
 * will received all parsed information. Further, if
 * the specified instruction is a request or a release
 * for a semaphore then a pointer to a semaphore table
 * is needed for handling this cases.
 *
 * @param instr a pointer to an instruction
 * @param line the instruction line
 * @param sem_table a pointer to a semaphore table
 */
void instr_parse(instr_t* instr, const char* line, semaphore_table_t* sem_table) {
    /* It checks if the instruction is unary, that is, P(sem_name) or V(sem_name) */
    if (line[0] == 'P' || line[0] == 'V')
        instr_semaphore_parse(instr, line, sem_table);
    /* Binary instruction */
    else {
        char* dupline = strdup(line);
        char* left_op = strtok(dupline, " ");
        int right_op = atoi(strtok(NULL, " "));

        if (strcmp(left_op, "exec") == 0)
            instr->op = EXEC;
        else if (strcmp(left_op, "read") == 0)
            instr->op = READ;
        else if (strcmp(left_op, "write") == 0)
            instr->op = WRITE;
        else if (strcmp(left_op, "print") == 0)
            instr->op = PRINT;

        instr->value = right_op;
        instr->sem = NULL;
    }
}
