#include <math.h>

#include "../tools/math.h"
#include "memory.h"

/* Segment Internal Function Definitions */

/**
 * It creates a segment and update the segment
 * information with that specified in the
 * memory request.
 *
 * @param req the memory request
 *
 * @return a pointer to a segment
 */
static segment_t* segment_create(memory_request_t* req) {
    segment_t* seg = (segment_t *)malloc(sizeof(segment_t));

    /* It checks if the segment could not be allocated */
    if (!seg) {
        printf("Not enough memory to allocate a segment.\n");
        exit(EXIT_FAILURE);
    }

    seg->id = req->proc->seg_id;
    seg->size = req->proc->seg_size * KILOBYTE; /* size measure in bytes */
    seg->page_count = (int) ceil((double) req->proc->code_len / INSTRUCTIONS_PER_PAGE);
    seg->page_table = (page_t *)malloc(sizeof(page_t) * seg->page_count);
    seg->page_qtd = seg->size / PAGE_SIZE;

    return seg;
}

/**
 * It populates the segment's page with the
 * program code. Further, it is supposed that
 * the segment is already created.
 *
 * @param code the program code
 * @param code_len the program code length
 */
static void segment_populate(segment_t* seg, instr_t* code, int code_len) {
    register int i;
    for (i = 0; i < code_len; i++)
        seg->page_table[PAGE_NUMBER(i)].used = 0;
    for (i = 0; i < code_len; i++)
        seg->page_table[PAGE_NUMBER(i)].code[PAGE_OFFSET(i)] = code[i];
}

/**
 * It adds the specified segment into the segment table.
 *
 * @param seg_table the segment table
 * @param seg the segment to be added
 */
static void segment_add(segment_table_t* seg_table, segment_t* seg) {
    list_add(seg_table->seg_list, seg);
    seg_table->seg_list_size++;
}

/**
 * It swaps the necessary pages that are candidates for
 * being unloaded from the main memory using the second
 * chance algorithm to be able to allocate pages provided
 * by a segment.
 *
 * @param seg_table the segment table
 * @param seg the segment containing the pages to
 *            to be loaded into the memory
 * @return the amount of memory freed by unused pages.
 */
static int memory_page_swap(segment_table_t* seg_table, segment_t* seg) {
    int i;
    int freed = 0;

    FOREACH(seg_table->seg_list, segment_t*) {
        for (i = 0; i < it->page_count; i++) {
            page_t* page = it->page_table + i;

            /* If the page is being referenced, then the */
            /* used bit is cleared (giving to it a second chance) */
            if (page->used)
                page->used = 0;
            else freed += PAGE_SIZE;

            if (freed >= seg->size)
                break;
        }
    }

    return freed;
}

/* Segment Table Function Definitions */

/**
 * It initializes the segment table.
 *
 * @param seg_table the segment table to be initialized.
 */
void segment_table_init(segment_table_t* seg_table) {
    seg_table->seg_list = list_init();
    seg_table->seg_list_size = 0;
    seg_table->remaining = 1024 * 1024 * 1024;
}

/* Segment Function Definitions */

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
segment_t* segment_find(segment_table_t* seg_table, int sid) {
    FOREACH(seg_table->seg_list, segment_t*) {
        if (it->id == sid)
            return it;
    }

    return NULL;
}

/**
 * It frees a memory segment allocated in the specified
 * segment table. Further, if there is no segment in
 * the table with that id, the application is automatically
 * terminated.
 *
 * @param seg_table the segment table
 * @param sid the segment id
 */
void segment_free(segment_table_t* seg_table, int sid) {
    list_node_t* seg_node;
    for (seg_node = seg_table->seg_list->head; seg_node != NULL;
         seg_node = seg_node->next)
        if (((segment_t *)seg_node->content)->id == sid)
            break;

    /* It checks if the segment could not be found */
    if (!seg_node) {
        printf("A segment release operation has been invoked"
               " for an unregistered segment id.\n");
        exit(EXIT_FAILURE);
    }

    /* Remove the segment node from the segment table list */
    list_remove_node(seg_table->seg_list, seg_node);

    segment_t* seg = (segment_t *)seg_node->content;

    /* It sums the freed memory to the amount of */
    /* addressable memory space */
    seg_table->remaining = MIN(MAX_MEM_SIZE, seg_table->remaining + seg->size);

    free(seg->page_table);
    free(seg);
    free(seg_node);
}

/* Memory Request Function Definitions */

/**
 * It initializes the memory request.
 *
 * @param req a pointer to a memory request
 * @param the process which requested the memory
 * @param code the program code
 */
void mem_req_init(memory_request_t* req, process_t* proc, instr_t* code) {
    req->proc = proc;
    req->code = code;
}

/**
 * It allocates a block of memory used by a process
 * via a memory request.
 *
 * @param req the memory request
 * @param seg_table the segment table
 */
void mem_req_load(memory_request_t* req, segment_table_t* seg_table) {
    segment_t* seg = segment_create(req);

    const int new_remaining = seg_table->remaining - seg->size;

    /* Update the remaining addressable memory space */
    seg_table->remaining = MAX(0, new_remaining);

    /* If checks if the request memory segment could not */
    /* be allocated into the memory space because this */
    /* does not have enough. Therefore, a page swapping */
    /* must occur */
    if (new_remaining < 0) {
        /* Invoke the page swapping */
        seg_table->remaining += memory_page_swap(seg_table, seg);
    }

    /* Populate the segment's page with the program code */
    segment_populate(seg, req->code, req->proc->code_len);
    segment_add(seg_table, seg);
}

