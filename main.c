#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "kernel/kernel.h"

kernel_t* kernel;

long long int ns_last_execution;
long long int t;

void cpu() {
    printf("Test123\n");
    while (1) {
        if (ns_last_execution < 100000000) {
            ns_last_execution++;
            continue;
        } else {
            ns_last_execution = 0;
            t++;
        }

        if (!kernel) {
            printf("No kernel.\n");
            continue;
        }

        if ((t % 5) == 0) {
            ns_last_execution = 0;
            schedule_process(IO_REQUESTED);

            if (kernel->scheduler->scheduled_proc)
                printf("(I/O REQUEST) Process %s has been scheduled.\n",
                       kernel->scheduler->scheduled_proc->name);
            else printf("No process has been scheduled.\n");
            continue;
        }

        /* No process has taken the CPU */
        if (!kernel->scheduler->scheduled_proc) {
            printf("No process to be executed.\n");
            schedule_process(0);

            if (kernel->scheduler->scheduled_proc)
                printf("(NONE) Process %s has been scheduled.\n",
                       kernel->scheduler->scheduled_proc->name);
            else printf("No process has been scheduled (2).\n");
            continue;
        }

        if (kernel->scheduler->scheduled_proc->remaining > 0) {
            kernel->scheduler->scheduled_proc->remaining--;
            printf("Remaining: %d\n",
                   kernel->scheduler->scheduled_proc->remaining);
        }
        else {
            schedule_process(QUANTUM_COMPLETED);
            printf("Process %s has been scheduled.\n",
                   kernel->scheduler->scheduled_proc->name);
        }


    }
}

int main() {
    kernel_init();

    pthread_t cpu_id;
    pthread_attr_t cpu_attr;

    pthread_attr_init(&cpu_attr);
    pthread_attr_setscope(&cpu_attr, PTHREAD_SCOPE_SYSTEM);

    pthread_create(&cpu_id, NULL, cpu, NULL);

    process_t* p1 = (process_t *)malloc(sizeof(process_t));
    process_t* p2 = (process_t *)malloc(sizeof(process_t));
    process_t* p3 = (process_t *)malloc(sizeof(process_t));
    process_t* p4 = (process_t *)malloc(sizeof(process_t));

    p1->name = (char *)malloc(sizeof(char) * 8);
    p2->name = (char *)malloc(sizeof(char) * 8);
    p3->name = (char *)malloc(sizeof(char) * 8);
    p4->name = (char *)malloc(sizeof(char) * 8);

    strcpy(p1->name, "P1");
    strcpy(p2->name, "P2");
    strcpy(p3->name, "P3");
    strcpy(p4->name, "P4");

    printf("Adding processes.\n");
    list_add(kernel->scheduler->high_queue->queue, p1);
    list_add(kernel->scheduler->low_queue->queue, p2);
    list_add(kernel->scheduler->high_queue->queue, p3);
    list_add(kernel->scheduler->high_queue->queue, p4);

    pthread_join(cpu_id, NULL);
    return 0;
}