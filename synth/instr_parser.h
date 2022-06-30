#ifndef OS_PROJECT_INSTR_PARSER_H
#define OS_PROJECT_INSTR_PARSER_H

/**
 * A synthetic program instruction. Every instruction
 * is unary or binary and, when it is binary the left
 * operator is a string and the second one is an integer.
 * Further, when it is unary only the left operator
 * holds the instruction information.
 */
typedef struct Instruction {
    char left_op[8];
    int right_op;
    int unary; /* act as a boolean */
} instr_t;

/* Instruction Prototypes */

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
instr_t *instr_init(char *left_op, int right_op, int unary);

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
void instr_parse(instr_t *instr, char *line);

#endif // OS_PROJECT_INSTR_PARSER_H
