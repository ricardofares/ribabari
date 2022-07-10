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
        if (!kernel->scheduler.scheduled_proc) {
            printf("No process is running, then some will be scheduled.\n");
            schedule_process(&kernel->scheduler, NONE);

            if (kernel->scheduler.scheduled_proc)
                printf("Process %s has been scheduled.\n", kernel->scheduler.scheduled_proc->name);
            else printf("No process has been found to be scheduled.\n");
        }
        /* There is some process running */
        else {
            do {
                if ((currentTick - lastTick) >= 1000.0 * 1000.0 * 50.0) {
                    /* It calculates the physical memory address from the virtual memory address */
                    const int pc = FETCH_INSTR_ADDR(kernel->scheduler.scheduled_proc);
                    const int page_number = PAGE_NUMBER(pc);
                    const int page_offset = PAGE_OFFSET(pc);

                    /* Fetch the instruction from the page stored in the memory */
                    segment_t* seg = segment_find(&kernel->seg_table, kernel->scheduler.scheduled_proc->seg_id);
                    instr_t instr = seg->page_table[page_number].code[page_offset];

                    printf("Process %s, remaining: %d, pc: %d, sid: %d.\n",
                           kernel->scheduler.scheduled_proc->name,
                           kernel->scheduler.scheduled_proc->remaining,
                           pc,
                           seg->id);

                    /* Evaluate the current instruction to be executed by the process */
                    eval(kernel->scheduler.scheduled_proc, &instr);

                    lastTick = currentTick;
                }

                currentTick++;
            } while (
                    kernel->scheduler.scheduled_proc != NULL &&
                    kernel->scheduler.scheduled_proc->remaining > 0 &&
                    kernel->scheduler.scheduled_proc->pc < kernel->scheduler.scheduled_proc->code_len);

            if (kernel->scheduler.scheduled_proc == NULL)
                continue;

            /* The process has finished its execution, then it must be terminated */
            if (kernel->scheduler.scheduled_proc->pc >= kernel->scheduler.scheduled_proc->code_len)
                sysCall(PROCESS_FINISH, kernel->scheduler.scheduled_proc);
            else
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

    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test_client.txt");
    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test_server.txt");
//    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test3.txt");
//    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test4.txt");
//    sysCall(PROCESS_CREATE, "C:\\Users\\user\\CLionProjects\\so-project\\test5.txt");

     pthread_join(cpu_id, NULL);

//    process_t* proc = kernel->proc_table->head->content;
//    segment_t* seg = segment_find(&kernel->seg_table, proc->seg_id);
//    page_t page = seg->page_table[0];
//
//    printf("Process Name: %s\n", proc->name);
//    printf("Segment Id: %d (Matched: %d)\n", seg->id, seg->id == proc->seg_id);
//    printf("Process Semaphores: %s %s\n", proc->semaphores[0], proc->semaphores[1]);
//
//    for (int i = 0; i < proc->code_len; i++) {
//        instr_t instr = page.code[i];
//
//        if (!instr.sem)
//            printf("%s %d\n", op_to_name(instr.op), instr.value);
//        else printf("%s(%d)\n", op_to_name(instr.op), instr.sem);
//    }
//
//    semaphore_t* sem = semaphore_find(&kernel->sem_table, "s");
//    printf("Sem not null: %d\n", sem != NULL);
//
//    printf("\n\n\n\n");
//
//    proc = kernel->proc_table->head->next->content;
//    seg = segment_find(&kernel->seg_table, proc->seg_id);
//    page = seg->page_table[0];
//
//    printf("Process Name: %s\n", proc->name);
//    printf("Segment Id: %d (Matched: %d)\n", seg->id, seg->id == proc->seg_id);
//
//    for (int i = 0; i < proc->code_len; i++) {
//        instr_t instr = page.code[i];
//
//        if (!instr.sem)
//            printf("%s %d\n", op_to_name(instr.op), instr.value);
//        else printf("%s(%s)\n", op_to_name(instr.op), instr.sem->name);
//    }


    return 0;
}


