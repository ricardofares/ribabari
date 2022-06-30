#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instr_parser.h"

/**
 * It initializes an instruction with the
 * specified left operator, right operator (if any)
 * and whether the instruction is unary.
 *
 * @param left_op the left operator
 * @param right_op the right operator (if any)
 * @param unary whether the instruction is unary
 *
 * @return a pointer to the instruction or NULL
 *         if the system does not have enough
 *         memory to allocate an instruction
 */
instr_t *instr_init(char *left_op, int right_op, int unary) {
    instr_t *instr = (instr_t *)malloc(sizeof(instr_t));

    /* It checks if the instruction has not been allocated */
    if (!instr) {
        printf("Not enough memory to allocate an instruction!\n");
        return NULL;
    }

    strcpy(instr->left_op, left_op);
    instr->right_op = right_op;
    instr->unary = unary;

    return instr;
}

/**
 * It parses the specified line in the pre-conditioned
 * format ([left_op] {right_op}) in which the left operator
 * is always specified whereas right operator may not be
 * specified.
 *
 * @param instr the instruction to receive the parsed
 *              arguments
 * @param line the line to be parsed
 */
void instr_parse(instr_t *instr, char *line) {
    /* It checks if the instruction is a unary instruction */
    if (line[0] == 'P' || line[0] == 'V') {
        strcpy(instr->left_op, line);
        instr->right_op = -1;
        instr->unary = 1;
        return;
    }

    strcpy(instr->left_op, strtok(line, " "));
    instr->right_op = atoi(strtok(NULL, " "));
    instr->unary = 0;
}
