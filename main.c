#include <stdio.h>
#include <pthread.h>

#include "kernel/kernel.h"

kernel_t* kernel;

typedef long double d64;
d64 lastTick = 0.0;
d64 currentTick = 0.0;

_Noreturn void cpu() {
    while (1) {
        /* It checks if there is no kernel set */
        if (!kernel) {
            printf("No kernel has been found.\n");
            exit(0);
        }

        /* It checks if there is no scheduled proc */
        if (!kernel->scheduler->scheduled_proc) {
            printf("No process is running, then some will be scheduled.\n");
            schedule_process(kernel->scheduler, NONE);

            if (kernel->scheduler->scheduled_proc)
                printf("Process %s has been scheduled.\n", kernel->scheduler->scheduled_proc->name);
            else printf("No process has been found to be scheduled.\n");
        }
        /* There is some process running */
        else {
            do {
                if ((currentTick - lastTick) >= 1000.0 * 1000.0 * 50.0) {
                    kernel->scheduler->scheduled_proc->remaining--;
                    printf("Process %s is running and has remaining %d u.t.\n",
                           kernel->scheduler->scheduled_proc->name, kernel->scheduler->scheduled_proc->remaining);
                    lastTick = currentTick;
                }

                currentTick++;
            } while (kernel->scheduler->scheduled_proc->remaining > 0);

            /* Interrupt the current process to schedule another one, since the latter */
            /* has completed its quantum time (or time slice) */
            sysCall(PROCESS_INTERRUPT, (void *) QUANTUM_COMPLETED);
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


