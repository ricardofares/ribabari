#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "terminal.h"

/* Internal Functions Prototypes */

/**
 * It returns 1 if the specified string
 * contains a valid opcode. Otherwise, it
 * returns 0. Further, a valid opcode is
 * that in which contains only digits.
 *
 * @param buf the specified string
 *            possibly containing a
 *            valid opcode.
 *
 * @return 1 if the specified string is
 *         a valid opcode; otherwise, 0.
 */
int __is_valid_opcode(const char *buf);

/**
 * It dispatches the opcode function relative
 * to the specified opcode.
 *
 * @param opcode the opcode to have its
 *               function dispatched
 */
void __dispatch_opcode_func(const int opcode);

/* Terminal Functions */

/**
 * It initializes the terminal in which will
 * interacts with the operating system simulator
 * user receiving its messages.
 */
void terminal_init(void) {
    char buf[OPCODE_BUF_LEN];
    int opcode;

    do {
        printf(""
               " Opções:\n"
               "  1. Criar um processo.\n"
               "  2. Sair.\n"
               );

        scanf(" %[^\n]s", buf);

        /* It checks if the buffer does not contains only digits */
        if (!__is_valid_opcode(buf)) {
            printf("A opção selecionada deve conter somente dígitos.\n");
            printf("Por favor, tente novamente!\n");
            continue;
        }

        opcode = atoi(buf);
        __dispatch_opcode_func(opcode);
    } while(1);
}

/* Internal Functions */

/**
 * It returns 1 if the specified string
 * contains a valid opcode. Otherwise, it
 * returns 0. Further, a valid opcode is
 * that which contains only digits.
 *
 * @param buf the specified string
 *            possibly containing a
 *            valid opcode.
 *
 * @return 1 if the specified string is
 *         a valid opcode; otherwise, 0.
 */
int __is_valid_opcode(const char *buf) {
    const char *cp = buf;

    while (*cp) {
        if (!isdigit(*cp))
            return 0;
        cp++;
    }

    return 1;
}

/**
 * It dispatches the opcode function relative
 * to the specified opcode.
 *
 * @param opcode the opcode to have its
 *               function dispatched
 */
void __dispatch_opcode_func(const int opcode) {
    switch (opcode) {
        case 1: {
            printf("Criando processo.\n");
            break;
        }
        case 2: {
            printf("Até mais.\n");
            exit(0);
            break;
        }
    }
}