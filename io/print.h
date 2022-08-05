#ifndef OS_PROJECT_PRINT_H
#define OS_PROJECT_PRINT_H

#include "../process/process.h"
#include "../terminal/terminal.h"

/**
 * It requests a print operation.
 *
 * @param process the process which request
 *                the print operation.
 * @param duration the print duration
 */
void print_request(process_t* process, int duration);

#endif // OS_PROJECT_PRINT_H
