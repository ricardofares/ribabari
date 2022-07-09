#ifndef OS_PROJECT_MEMORY_H
#define OS_PROJECT_MEMORY_H

#define INSTRUCTIONS_PER_PAGE (256)

#include "../utils/list.h"
#include "../process/process.h"
#include "../semaphore/semaphore.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct Page {
    instr_t code[INSTRUCTIONS_PER_PAGE];
    int is_used;
} page_t;

typedef struct memory_node_t {
    int is_used;
    int begin;
    int size;

    page_t* page_table;
} memory_node_t;

typedef struct Segment {
    int id;

    memory_node_t* memory;
} segment_t;

typedef struct Segment_Table {
    list_t* seg_list;
    int seg_qtd;
} segment_table_t;


#endif // OS_PROJECT_MEMORY_H
