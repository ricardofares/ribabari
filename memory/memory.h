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

typedef struct memory_node_t {
    int is_used;
    int begin;
    int size;

    int page_qtd;
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

/**
 * It represents a structure used
 * to indicate the process' memory
 * needs for the operating system
 * allocate to it.
 */
typedef struct {
    /**
     * The process id. It is guaranteed
     * unique for each process.
     */
    int pid;

    /**
     * The segment id.
     */
    int sid;

    /**
     * The segment size.
     */
    int size;

    /**
     * The program code that will be
     * allocated in the segment requested
     * by this process.
     */
    instr_t* code;
} memory_request_t;

/* Memory Function Prototypes */

/**
 * It initializes the memory request.
 *
 * @param req a pointer to a memory request
 * @param pid the process id
 * @param sid the segment id
 * @param size the segment size
 * @param code the program code
 */
void mem_req_init(memory_request_t* req, int pid, int sid, int size, instr_t* code);

void mem_req_load(memory_request_t* req);

segment_table_t* init_segment_table();
memory_node_t* init_seg_memory(int size, int used, int begin);
segment_t* init_segment(int seg_id, int size, int used, int begin);
void add_segment(segment_table_t* seg_table, segment_t* seg);
segment_t* get_segment(segment_table_t* seg_table, int seg_id);
list_node_t* get_node_from_seg_table(segment_table_t* seg_table, int seg_id);
void delete_segment(segment_table_t* seg_table, int seg_id);
int seg_equal(void* seg_id, void* num);

#endif // OS_PROJECT_MEMORY_H

