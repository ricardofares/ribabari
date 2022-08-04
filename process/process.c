#include "process.h"

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