#ifndef OS_CPU_LOADED
#define OS_CPU_LOADED
#include "cpu/cpu.h"
#endif // OS_CPU_LOADED

#define HOLD_MAIN_THREAD while(1);

int main() {
    cpu_init();
    kernel_init();

    sysCall(PROCESS_CREATE, "/home/ricardo/CLionProjects/so-project/test.txt");
    sysCall(PROCESS_CREATE, "/home/ricardo/CLionProjects/so-project/test2.txt");

    HOLD_MAIN_THREAD;
    return 0;
}


