#ifndef OS_PROJECT_MEMORY_H
#define OS_PROJECT_MEMORY_H

#include "../utils/list.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum OP_TYPE {
    READ,
    EXEC,
    SEMAPHORE_START,
    SEMAPHORE_END,
} OP_TYPE;

typedef struct semaphore_t {
} semaphore_t;

typedef struct instruct_t {
    OP_TYPE op;
    int value;
    semaphore_t* semaphore;
} instruct_t;

typedef struct Page {
    instruct_t instructs[256];
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
