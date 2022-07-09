#ifndef OS_PROJECT_MEMORY_H
#define OS_PROJECT_MEMORY_H

#define INSTRUCTIONS_PER_PAGE (256)

#include "../utils/list.h"
#include "../process/instruction.h"
#include "../semaphore/semaphore.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct Page {
    instr_t code[INSTRUCTIONS_PER_PAGE];
    int is_used;
} page_t;

typedef struct Segment {
    int id;

    int is_used;
    int begin;
    int size;

    int page_qtd;
    page_t* page_table;

} segment_t;

typedef struct Segment_Table {
    list_t* seg_list;
    int seg_qtd;
} segment_table_t;


segment_table_t* init_segment_table();
segment_t* init_segment(int seg_id, int size, int used, int begin);
void add_segment(segment_table_t* seg_table, segment_t* seg);
segment_t* get_segment(segment_table_t* seg_table, int seg_id);
list_node_t* get_node_from_seg_table(segment_table_t* seg_table, int seg_id);
void delete_segment(segment_table_t* seg_table, int seg_id);
int seg_equal(void* seg_id, void* num);

#endif // OS_PROJECT_MEMORY_H

