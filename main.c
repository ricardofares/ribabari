#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "kernel/kernel.h"

kernel_t* kernel;

void print_list(list_t* list);
int int_matcher(void *a, void *b) {
    return ((int) a) == ((int) b);
}

long long int ns_last_execution;
long long int t;
int a = 1;
int b = 5;

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

        if ((t % b) == 0 && a) {
            ns_last_execution = 0;
            b *= 5;
//            schedule_process(kernel->scheduler, IO_REQUESTED);
//
//            if (kernel->scheduler->scheduled_proc)
//                printf("(I/O REQUEST) Process %s has been scheduled.\n",
//                       kernel->scheduler->scheduled_proc->name);
//            else printf("No process has been scheduled.\n");
            printf("Finishing the %s process...\n", kernel->scheduler->scheduled_proc->name);
            sysCall(PROCESS_FINISH, (void *) kernel->scheduler->scheduled_proc);
            continue;
        }

        /* No process has taken the CPU */
        if (!kernel->scheduler->scheduled_proc) {
            printf("No process to be executed.\n");
            schedule_process(kernel->scheduler, 0);

            if (kernel->scheduler->scheduled_proc)
                printf("(NONE) Process %s has been scheduled.\n",
                       kernel->scheduler->scheduled_proc->name);
            else printf("No process has been scheduled (2).\n");
            continue;
        }

        if (kernel->scheduler->scheduled_proc->remaining > 0) {
            kernel->scheduler->scheduled_proc->remaining--;
            printf("Remaining: %d (%s)\n",
                   kernel->scheduler->scheduled_proc->remaining,
                   kernel->scheduler->scheduled_proc->name);
        }
        else {
            printf("Scheduling...\n");
            schedule_process(kernel->scheduler, QUANTUM_COMPLETED);

            if (kernel->scheduler->scheduled_proc)
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

    pthread_create(&cpu_id, NULL, (void *) cpu, NULL);

    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test.txt");
    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test2.txt");
    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test3.txt");
    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test4.txt");
    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test5.txt");

    pthread_join(cpu_id, NULL);

    return 0;
}

void print_list(list_t* list) {
    list_node_t* curr;

    for (curr = list->head; curr != NULL; curr = curr->next)
        printf("%d -> ", (int) curr->content);
    printf("\n");
    for (curr = list->tail; curr != NULL; curr = curr->prev)
        printf("%d -> ", (int) curr->content);
    printf("\n");

}