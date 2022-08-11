#include <time.h>

#ifndef OS_CPU_LOADED
#define OS_CPU_LOADED
#include "cpu/cpu.h"
#endif // OS_CPU_LOADED

int main() {
    srand(time(NULL));

    process_log_init();
    disk_log_init();
    io_log_init();
    res_acq_log_init();

    disk_init();
    cpu_init();
    kernel_init();

    begin_terminal();
    return 0;
}

