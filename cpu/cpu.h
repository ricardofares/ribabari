#ifndef OS_PROJECT_CPU_H
#define OS_PROJECT_CPU_H

#ifndef OS_CPU_KERNEL_LOADED
#define OS_CPU_KERNEL_LOADED
#include "../kernel/kernel.h"
#endif // OS_CPU_KERNEL_LOADED

/* CPU Function Prototype */

/**
 * It creates the CPU-Thread that
 * is going to run the CPU function
 * that is going to simulate the CPU
 * execution along the simulation.
 */
void cpu_init();

/**
 * It represents the function that
 * is going to simulate the CPU work
 * by executing instructions that are
 * selected by the scheduler.
 */
_Noreturn void cpu();

#endif // OS_PROJECT_CPU_H
