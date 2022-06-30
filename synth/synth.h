#ifndef OS_PROJECT_SYNTH_READER_H
#define OS_PROJECT_SYNTH_READER_H

#include "instr_parser.h"

/**
 * A synthetic program. This contains all
 * descriptive information about a synthetic
 * program.
 */
typedef struct SynthProg {
    char *name;
    int segment_id;
    int sched_priority;
    long int segment_size; /* kilobytes (kb) */
    char *semaphores;

    /**
     * It contains the program code.
     */
    instr_t *code;
    int codelen;
} synth_prog_t;

/* Synthetic Program Prototypes */

/**
 * It allocates a memory for a synthetic program,
 * however, if the system does not have enough
 * memory to allocate, then NULL is returned.
 *
 * @return a pointer to a synthetic program or
 *         NULL if the system does not have
 *         enough memory
 */
synth_prog_t *synth_prog_create();

/**
 * It populates the synthetic program from a
 * specified file containing the synthetic
 * program description. Further, it is the
 * caller's responsibility to close the file.
 *
 * @param f the file to be read
 */
void synth_prog_populate(synth_prog_t *prog, FILE *f);

#endif // OS_PROJECT_SYNTH_READER_H
