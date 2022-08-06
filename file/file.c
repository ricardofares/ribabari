#include <stdio.h>
#include <stdlib.h>

#include "../constants.h"
#include "file.h"

/* Inode Function Definitions */

/**
 * It returns a pointer to an inode.
 * If there is not enough memory to
 * allocate an inode, then NULL is
 * returned.
 *
 * @param the inode id
 *
 * @return a pointer to an inode, however,
 *         if there is not enough memory
 *         to allocate an inode, then NULL
 *         is returned.
 */
inode_t* inode_create(int id) {
    inode_t* inode = (inode_t *)malloc(sizeof(inode_t));

    /* It check if the inode could not be allocated. */
    if (!inode) {
        printf("Not enough memory to allocate an inode.\n");
        exit(EXIT_FAILURE);
    }

    inode->id = id;
    inode->o_count = 0;
    inode->acc_perm_flag = FILE_READ_PERM | FILE_EXECUTE_PERM;
    inode->last_accessed = -1;
    inode->last_modified = -1;

    return inode;
}

/**
 * It returns 1 if both inode has the same
 * inode number. Otherwise, 0 is returned.
 *
 * @param i1 the first inode
 * @param i2 the second inode
 *
 * @return 1 if both inode has the same
 *         inode number; otherwise, 0
 *         is returned.
 */
int inode_cmp(void* i1, void* i2) {
    return ((inode_t *) i1)->id == ((inode_t *) i2)->id;
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
}

/**
 * It returns a pointer to an inode if an inode
 * with the specified inode number is active in
 * the specified file table. Otherwise, NULL is
 * returned.
 *
 * @param file_table the file table
 * @param inumber the inode number
 *
 * @return a pointer to an inode if an inode with
 *         the specified inode number is active in
 *         the specified file table; otherwise,
 *         NULL is returned.
 */
inode_t* find_inode(file_table_t* file_table, int inumber) {
    list_node_t* curr_node;

    for (curr_node = file_table->ilist->inode_list->head; curr_node != NULL;
         curr_node = curr_node->next) {
        inode_t* inode = (inode_t *)curr_node->content;
        if (inode->id == inumber)
            return inode;
    }

    return NULL;
}

/* File Request Function Prototypes */

/**
 * It requests a file read operation. Further, if
 * the file is not active, then an inode is created
 * for this file.
 *
 * @param file_table the file table
 * @param process the process which has requested
 *                a file read operation.
 * @param block the block in which the file is
 *              stored
 */
void fs_read_request(file_table_t* file_table, process_t* process, int block) {
    const int inumber = INODE_NUMBER(block);

    inode_t* inode = find_inode(file_table, inumber);

    /* It checks if there is no active inode in the file table with that inumber */
    if (!inode) {
        inode = inode_create(inumber);

        /* Add the inode into the active inode list */
        list_add(file_table->ilist->inode_list, inode);

        /* Update the inode last modified field */
        time(&inode->last_modified);
#if OS_FS_DEBUG
        printf("An inode %d has been put as an active file by process %s (%d) from a read operation.\n", inumber, process->name, process->id);
#endif // OS_FS_DEBUG
    }

    /* It checks if the process has not before opened this file */
    if (!has_opened_file(process, inumber)) {
        list_add(process->o_files, inumber);

        /* Increase the counter of the amount of process */
        /* has this file opened */
        inode->o_count++;

#if OS_FS_DEBUG
        printf("Process %s is in its first time being activating the inode %d from a read operation.", process->name, inumber);
#endif // OS_FS_DEBUG
    }

    /* Update the inode last accessed field */
    time(&inode->last_accessed);
}

/**
 * It requests that the file system to handle the
 * content that is being written to the disk from
 * a disk write operation.
 *
 * @param file_table the file table
 * @param process the process which has requested
 *                a disk write operation
 * @param block the block in which the file is
 *              being written at
 */
void fs_write_request(file_table_t* file_table, process_t* process, int block) {
    const int inumber = INODE_NUMBER(block);

    inode_t* inode = find_inode(file_table, inumber);

    /* It checks if the inode could not be found in the file table */
    if (!inode) {
        inode = inode_create(inumber);

        /* Add the inode into the active inode list */
        list_add(file_table->ilist->inode_list, inode);

        time(&inode->last_accessed);
#if OS_FS_DEBUG
        printf("An inode %d has been put as an active file by process %s (%d) from a write operation.\n", inumber, process->name, process->id);
#endif // OS_FS_DEBUG
    }

    /* It checks if the process has not before opened this file */
    if (!has_opened_file(process, inumber)) {
        list_add(process->o_files, inumber);

        /* Increase the counter of the amount of process */
        /* has this file opened */
        inode->o_count++;

#if OS_FS_DEBUG
        printf("Process %s is in its first time being activating the inode %d from a write operation.", process->name, inumber);
#endif // OS_FS_DEBUG
    }

    /* Update both access time fields */
    time(&inode->last_modified);

#if OS_FS_DEBUG
    printf("Process %s is writing at the inode %d.\n", process->name, inumber);
#endif // OS_FS_DEBUG
}