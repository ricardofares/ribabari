#ifndef OS_PROJECT_TERMINAL_H
#define OS_PROJECT_TERMINAL_H

/* The maximum opcode length */
#define OPCODE_LEN (2)

/* The buffer length is defined as the sum */
/* of the maximum opcode length plus the '\0' */
#define OPCODE_BUF_LEN (OPCODE_LEN + 1)

/**
 * It initializes the terminal in which will
 * interacts with the operating system simulator
 * user receiving its messages.
 */
void terminal_init(void);

#endif // OS_PROJECT_TERMINAL_H
