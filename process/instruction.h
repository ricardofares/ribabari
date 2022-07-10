#ifndef SO_PROJECT_INSTRUCTION_H
#define SO_PROJECT_INSTRUCTION_H

#include "../semaphore/semaphore.h"

/**
 * It is an enumeration containing the
 * available operator codes.
 */
typedef enum Opcode {
    EXEC,
    READ,
    WRITE,
    PRINT,
    SEM_P,
    SEM_V
} opcode_t;

/**
 * It represents an instruction that contains
 * an operator, a possible value and may contain
 * a semaphore that has been requested.
 *
 * The general instruction's format is:
 *  - For unary operators
 *    <left_op(semaphore_name)>
 *  - For binary operators
 *    <left_op> <value>
 */
typedef struct Instruction {
    opcode_t op;
    int value;
    char* sem;
} instr_t;

/* Instruction Function Prototypes */

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
void instr_parse(instr_t* instr, const char* line, semaphore_table_t* sem_table);

#endif // SO_PROJECT_INSTRUCTION_H
