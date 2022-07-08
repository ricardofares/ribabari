#include "memory.h"

segment_table_t* init_segment_table() {
    segment_table_t* new_seg_table = (segment_table_t*)malloc(sizeof(segment_table_t));

    if (!new_seg_table) {
        printf("Couldn't allocate memory for segment_table.\n");
        exit(0);
    }

    (*new_seg_table) = (segment_table_t) {
        .seg_list = list_init(),
        .seg_qtd = 0,
    };

    if (!new_seg_table->seg_list) {
        printf("Couldn't allocate memory for the list of segments.\n");
        free(new_seg_table);
        exit(0);
    }

    return new_seg_table;
}

segment_t* init_segment(const int id, const int pos, const int size) {
    segment_t* new_segment = (segment_t*) malloc(sizeof(segment_t));

    return new_segment;
}
