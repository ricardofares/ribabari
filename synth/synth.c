#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "synth.h"

/* Synthetic Program Utility Functions */

/**
 * It removes all inner whitespaces
 * contained in the specified string.
 *
 * @param s the string to have its inner
 *          whitespaces removed.
 *
 * @return the length of the string without
 *         the whitespaces.
 */
int rem_inner_ws(char *s);

/* Synthetic Program Functions */

synth_prog_t *synth_prog_create() {
    synth_prog_t *prog = (synth_prog_t *)malloc(sizeof(synth_prog_t));

    /* It checks if the synthetic program has not been allocated */
    if (!prog) {
        printf("Not enough memory to allocate a synthetic program.\n");
        return NULL;
    }

    prog->name = NULL;
    prog->segment_id = -1;
    prog->segment_size = -1;
    prog->sched_priority = -1;
    prog->semaphores = NULL;
    prog->code = (instr_t *)calloc(0, sizeof(instr_t));
    prog->codelen = 0;
    return prog;
}

void synth_prog_populate(synth_prog_t *prog, FILE *f) {
    const int buflen = 256;
    char buf[buflen];
    int codelen;

    /* Name populating */
    fgets(buf, buflen, f);
    prog->name = (char *) malloc(sizeof(char) * strlen(buf));
    strcpy(prog->name, buf);

    /* Segment id populating */
    fgets(buf, buflen, f);
    prog->segment_id = atoi(buf);

    /* Program priority populating */
    fgets(buf, buflen, f);
    prog->sched_priority = atoi(buf);

    /* Segment size populating */
    fgets(buf, buflen, f);
    prog->segment_size = atoi(buf);

    /* Semaphores populating */
    fgets(buf, buflen, f);
    prog->semaphores = (char *) malloc(sizeof(char) * strlen(buf));
    strcpy(prog->semaphores, buf);
    rem_inner_ws(prog->semaphores);

    /* Jump the blank line */
    fgets(buf, buflen, f);

    codelen = 0;
    while (fgets(buf, buflen, f)) {
        /* Jump new lines */
        if (buf[0] == '\n')
            continue;
        buf[strlen(buf) - 1] = '\0';
        codelen++;
        prog->code = (instr_t *)realloc(prog->code, sizeof(instr_t) * codelen);
        instr_parse(&prog->code[codelen - 1], buf);
    }

    prog->codelen = codelen;
}

/**
 * It removes all inner whitespaces
 * contained in the specified string.
 *
 * @param s the string to have its inner
 *          whitespaces removed.
 *
 * @return the length of the string without
 *         the whitespaces.
 */
int rem_inner_ws(char *s) {
    const size_t slen = strlen(s);
    int i = 0;
    int j = 0;

    while (i < slen) {
        if (s[i] != ' ') {
            s[j] = s[i];
            j++;
        }
        i++;
    }

    s[j] = '\0';
    return j;
}