#ifndef OS_PROJECT_MEMORY_H
#define OS_PROJECT_MEMORY_H

#include <stdlib.h>

typedef struct Page {
    int offset;
} page_t;

typedef struct Segment {
    int id;
    int pos;
    int size;

    page_t* page_table;
} segment_t;

#endif // OS_PROJECT_MEMORY_H
