#include <stdio.h>

#include <stdlib.h>

#define MEMORY_SIZE (1024 * 1024 * 1024)
#define KERNEL_HIGH_ADDR (MEMORY_SIZE)
#define KERNEL_LOW_ADDR (KERNEL_HIGH_ADDR - sizeof(kernel_t))

typedef struct bcp_t {
    int begin_mem;
    int size;
    int id;
} bcp_t;

typedef struct process_table_t {
    bcp_t* bcps;
} process_table_t;

typedef struct seg_table_t {

} seg_table_t;

typedef struct scheduler_t {

} scheduler_t;

typedef struct kernel_t {
    seg_table_t* seg_table;
    process_table_t* process_table;
    scheduler_t* scheduler;
    int pc;
} kernel_t;

typedef struct memory_t {
    void* memory;
} memory_t;

int main() {
	void* memory = malloc(MEMORY_SIZE);

	kernel_t* kernel = (kernel_t*)((char*) memory + KERNEL_LOW_ADDR);

    (* kernel) = (kernel_t) {
        .pc = 0,
        .seg_table = memory,
        .process_table = memory,
        .scheduler = memory,
    };

    printf("%d\n", kernel->pc);

    free(memory);
	return 0;
}

