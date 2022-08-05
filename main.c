#ifndef OS_CPU_LOADED
#define OS_CPU_LOADED
#include "cpu/cpu.h"
#endif // OS_CPU_LOADED

#include "terminal/terminal.h"

#define HOLD_MAIN_THREAD while(1);

int main() {
    process_log_init();
    disk_log_init();

    disk_init();
    cpu_init();
    kernel_init();

    begin_terminal();

    /* HOLD_MAIN_THREAD; */
    return 0;
}

