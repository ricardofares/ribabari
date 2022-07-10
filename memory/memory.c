#include "memory.h"

#define TRUE 1
#define FALSE 0
#define KILOBYTE 1024
#define PAGE_SIZE (KILOBYTE * 4)

segment_table_t* init_segment_table() {
    segment_table_t* new_seg_table
        = (segment_table_t*)malloc(sizeof(segment_table_t));

    if (!new_seg_table) {
        printf("Couldn't allocate memory for segment table.\n");
        exit(EXIT_FAILURE);
    }

    (*new_seg_table) = (segment_table_t) {
        .seg_list = list_init(),
        .seg_qtd = 0,
    };

    if (!new_seg_table->seg_list) {
        printf("Couldn't allocate memory for the list of segments.\n");
        free(new_seg_table);
        exit(EXIT_FAILURE);
    }

    return new_seg_table;
}

memory_node_t* init_seg_memory(int size, int used, int begin) {
    memory_node_t* new_memory = (memory_node_t*)malloc(sizeof(memory_node_t));

    if (!new_memory) {
        printf("Couldn't allocate memory for the struct memory.\n");
        exit(EXIT_FAILURE);
    }

    int organized_size = KILOBYTE * size;
    int real_size = size * KILOBYTE;

    // If the size of the memory is divisable by 4096 (size of one page)
    // the quantity of pages is the result of this division
    // otherwise, adds one to the result of the division
    int page_qtd = (real_size % PAGE_SIZE) == 0 ? real_size / PAGE_SIZE
                                                : real_size / PAGE_SIZE + 1;

    (*new_memory) = (memory_node_t) {
        .is_used = used,
        .begin = begin,
        .size = real_size,
        .page_qtd = page_qtd,
        .page_table = (page_t*)malloc(sizeof(page_t) * page_qtd),
    };

    if (!new_memory->page_table) {
        printf("Couldn't allocate memory for a page table.\n");
        exit(EXIT_FAILURE);
    }

    return new_memory;
}

segment_t* init_segment(int seg_id, int size, int used, int begin) {
    segment_t* new_seg = (segment_t*)malloc(sizeof(segment_t));

    if (!new_seg) {
        printf("Couldn't allocate memory for segment.\n");
        exit(EXIT_FAILURE);
    }

    new_seg->id = seg_id;
    new_seg->memory = init_seg_memory(size, used, begin);

    return new_seg;
}

void add_segment(segment_table_t* seg_table, segment_t* seg) {
    list_add(seg_table->seg_list, seg, sizeof(segment_t *));
    seg_table->seg_qtd += 1;
}

segment_t* get_segment(segment_table_t* seg_table, int seg_id) {
    list_node_t* found_node = get_node_from_seg_table(seg_table, seg_id);

    // found_node is NULL when the segment wasn't found by the id given.
    if (found_node == NULL) {
        return NULL;
    }

    if (found_node->content == NULL) {
        printf("Node of segment list without a segment, something is wrong!");
        exit(EXIT_FAILURE);
    }

    return ((segment_t*)found_node->content);
}

list_node_t* get_node_from_seg_table(segment_table_t* seg_table, int seg_id) {
    list_node_t* found_node
        = list_search(seg_table->seg_list, (void*)&seg_id, seg_equal);

    // found_node is NULL when the segment wasn't found by the id given.
    return found_node;
}

void delete_segment(segment_table_t* seg_table, int seg_id) {
    list_node_t* node_to_delete = get_node_from_seg_table(seg_table, seg_id);

    if (node_to_delete == NULL) {
        return;
    }

    free(((segment_t*) node_to_delete->content)->memory->page_table);
    free(((segment_t*) node_to_delete->content)->memory);
    free(node_to_delete->content);

    list_remove_node(seg_table->seg_list, node_to_delete);
    free(node_to_delete);
}

int seg_equal(void* seg_id, void* num) {
    if (*((int*)seg_id) == *((int*)num)) {
        return TRUE;
    }

    return FALSE;
}

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
 */
void mem_req_init(memory_request_t* req, int pid, int sid, int size, instr_t* code) {
    req->pid = pid;
    req->sid = sid;
    req->size = size;
    req->code = code;
}