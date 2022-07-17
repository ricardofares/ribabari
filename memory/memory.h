#ifndef OS_PROJECT_MEMORY_H
#define OS_PROJECT_MEMORY_H

#define INSTRUCTIONS_PER_PAGE (256)
#define PAGE_NUMBER(x) ((x) / INSTRUCTIONS_PER_PAGE)
#define PAGE_OFFSET(x) ((x) % INSTRUCTIONS_PER_PAGE)

#include "../utils/list.h"
#include "../process/instruction.h"
#include "../process/process.h"
#include "../semaphore/semaphore.h"

#include <stdio.h>
#include <stdlib.h>

#define KILOBYTE (1024)
#define PAGE_SIZE (4 * KILOBYTE)

typedef struct Page {
    instr_t code[INSTRUCTIONS_PER_PAGE];
    int used;
} page_t;

typedef struct Segment {
    /* General Segment Information */
    int id;
    int size;

    /* Page Table Information */
    page_t* page_table;
    int page_count;
    int page_qtd;
} segment_t;

typedef struct Segment_Table {
    /**
     * The list containing the
     * registered segments requested
     * by the processes.
     */
    list_t* seg_list;

    /**
     * It holds the segment
     * list size.
     */
    int seg_list_size;
} segment_table_t;

/**
 * It represents a structure used
 * to indicate the process' memory
 * needs for the operating system
 * allocate to it.
 */
typedef struct {
    /**
     * It represents the process which
     * requested the memory.
     */
    process_t* proc;

    /**
     * The program code that will be
     * allocated in the segment requested
     * by this process.
     */
    instr_t* code;
} memory_request_t;

/* Segment Table Function Prototypes */

/**
 * It initializes the segment table.
 *
 * @param seg_table the segment table to be initialized.
 */
void segment_table_init(segment_table_t* seg_table);

/* Segment Function Prototypes */

/**
 * It returns a pointer to a segment with the specified id.
 * Further, if the segment table does not have any segment
 * with that id, then NULL is returned.
 *
 * @param seg_table the segment table in which the segment
 *                  will be searched
 * @param sid the segment id
 *
 * @return a pointer to a segment or NULL if the segment
 *         could not be found
 */
segment_t* segment_find(segment_table_t* seg_table, int sid);

/**
 * It frees a memory segment allocated in the specified
 * segment table. Further, if there is no segment in
 * the table with that id, the application is automatically
 * terminated.
 *
 * @param seg_table the segment table
 * @param sid the segment id
 */
void segment_free(segment_table_t* seg_table, int sid);

/* Memory Request Function Prototypes */

/**
 * It initializes the memory request.
 *
 * @param req a pointer to a memory request
 * @param the process which requested the memory
 * @param code the program code
 */
void mem_req_init(memory_request_t* req, process_t* proc, instr_t* code);

/**
 * It loads the requested memory into the specified
 * segment table.
 *
 * @param req the memory request
 * @param seg_table the segment table in which the
 *                  memory will be allocated
 */
void mem_req_load(memory_request_t* req, segment_table_t* seg_table);

#endif // OS_PROJECT_MEMORY_H

