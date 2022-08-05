#include <stdio.h>
#include <stdlib.h>

#include "file.h"

#ifndef OS_SCHED_KERNEL
#define OS_SCHED_KERNEL
#include "../kernel/kernel.h"
#endif // OS_SCHED_KERNEL

/* Inode Function Definitions */

/**
 * It returns a pointer to an inode.
 * If there is not enough memory to
 * allocate an inode, then NULL is
 * returned.
 *
 * @return a pointer to an inode, however,
 *         if there is not enough memory
 *         to allocate an inode, then NULL
 *         is returned.
 */
inode_t* inode_create() {
    inode_t* inode = (inode_t *)malloc(sizeof(inode_t));

    /* It check if the inode could not be allocated. */
    if (!inode) {
        printf("Not enough memory to allocate an inode.\n");
        exit(EXIT_FAILURE);
    }

    inode->id = kernel->file_table.next_inode_id++;
    inode->o_count = 0;
    inode->acc_perm_flag = FILE_READ_PERM | FILE_EXECUTE_PERM;

    return inode;
}

/* Ilist Function Prototypes */

/**
 * It returns a pointer to an ilist.
 * If there is not enough memory to
 * allocate an ilist, then the
 * application is exited.
 *
 * @return a pointer to an ilist, however,
 *         if there is not enough memory
 *         to allocate an ilist, then the
 *         application is exited.
 */
ilist_t* ilist_create() {
    ilist_t* ilist = (ilist_t *)malloc(sizeof(ilist_t));

    /* It checks if the ilist could not be allocated. */
    if (!ilist) {
        printf("Not enough memory to allocate an ilist.\n");
        exit(EXIT_FAILURE);
    }

    ilist->inode_list = list_init();

    return ilist;
}

/* File Table Function Definitions */

/**
 * It initializes the specified file table.
 *
 * @param file_table a pointer to a file table
 */
void file_table_init(file_table_t* file_table) {
    file_table->ilist = ilist_create();
    file_table->next_inode_id = 0;
}
