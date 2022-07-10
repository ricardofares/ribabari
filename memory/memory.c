#include <math.h>

#include "memory.h"

#define KILOBYTE (1024)

/* Segment Internal Function Definitions */

/**
 * It returns 1 if the segments have been matched.
 * Otherwise, it will return 0.
 *
 * @param seg1 the first segment
 * @param seg2 the second segment
 *
 * @return 1 if the segments have been matched;
 *         otherwise will return 0.
 */
static int seg_cmp(void* seg1, void* seg2) {
    return ((segment_t *)seg1)->id == ((segment_t *)seg2)->id;
}

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

    seg->id = req->sid;
    seg->size = req->size * KILOBYTE; /* size measure in bytes */
    seg->page_count = (int) ceil((double) req->code_len / INSTRUCTIONS_PER_PAGE);\
    seg->page_table = (page_t *)malloc(sizeof(page_t) * seg->page_count);

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

/* Segment Table Function Definitions */

/**
 * It initializes the segment table.
 *
 * @param seg_table the segment table to be initialized.
 */
void segment_table_init(segment_table_t* seg_table) {
    seg_table->seg_list = list_init();
    seg_table->seg_list_size = 0;
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
    list_node_t* seg_node;
    for (seg_node = seg_table->seg_list->head; seg_node != NULL;
         seg_node = seg_node->next)
        if (((segment_t *)seg_node->content)->id == sid)
            return seg_node->content;
    return NULL;
}

/* Memory Request Function Definitions */

/**
 * It initializes the memory request. Further,
 * if pid, sid or size is negative then the
 * program is automatically terminated.
 *
 * @param req a pointer to a memory request
 * @param pid the process id
 * @param sid the segment id
 * @param size the segment size
 * @param code the program code
 * @param code_len the program code length
 */
void mem_req_init(memory_request_t* req, int pid, int sid, int size, instr_t* code, int code_len) {
    req->pid = pid;
    req->sid = sid;
    req->size = size;
    req->code = code;
    req->code_len = code_len;
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

    /* Populate the segment's page with the program code */
    segment_populate(seg, req->code, req->code_len);
    segment_add(seg_table, seg);
}