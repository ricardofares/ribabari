#include <stdio.h>
#include <stdlib.h>

#include "../constants.h"
#include "../terminal/log.h"

#include "file.h"

#if OS_DEBUG || OS_FS_DEBUG
#define LOG_FS(fmt) printf(fmt)
#define LOG_FS_A(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define LOG_FS(fmt)
#define LOG_FS_A(fmt, ...)
#endif // OS_DEBUG || OS_FS_DEBUG

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
    inode->mode = F_PERM_OWNER_R | F_PERM_OWNER_W | F_PERM_OWNER_X | F_PERM_OTHER_R | F_PERM_OTHER_W;
    inode->last_accessed = -1;
    inode->last_modified = -1;
    inode->size = (100 * (1 + (rand() % 10))) + (rand() % RAND_MAX);

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
    FOREACH(file_table->ilist->inode_list, inode_t*) {
        if (it->id == inumber)
            return it;
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
        /* A open request is made, an inode for the file is returned */
        inode = fs_open_request(file_table, process->name, inumber);

        /* Update the inode last modified field */
        time(&inode->last_modified);

        LOG_FS_A("An inode %d has been put as an active file by process %s (%d) from a read operation.\n", inumber, process->name, process->id);
    }

    /* It checks if the process has not before opened this file */
    if (!has_opened_file(process, inumber)) {
        list_add(process->o_files, inumber);

        /* Increase the counter of the amount of process */
        /* has this file opened */
        inode->o_count++;

        LOG_FS_A("Process %s is in its first time being activating the inode %d from a read operation.\n", process->name, inumber);
    }

    /* Update the inode last accessed field */
    time(&inode->last_accessed);

    LOG_FS_A("Process %s is reading from the inode %d.\n", process->name, inumber);
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
        /* A open request is made, an inode for the file is returned */
        inode = fs_open_request(file_table, process->name, inumber);

        time(&inode->last_accessed);

        LOG_FS_A("An inode %d has been put as an active file by process %s (%d) from a write operation.\n", inumber, process->name, process->id);
    }

    /* It checks if the process has not before opened this file */
    if (!has_opened_file(process, inumber)) {
        list_add(process->o_files, inumber);

        /* Increase the counter of the amount of process */
        /* has this file opened */
        inode->o_count++;

        LOG_FS_A("Process %s is in its first time being activating the inode %d from a write operation.\n", process->name, inumber);
    }

    /* Update both access time fields */
    time(&inode->last_modified);

    LOG_FS_A("Process %s is writing at the inode %d.\n", process->name, inumber);
}

/**
 * It opens a file specified by the inode's number.
 *
 * @param file_table the file table
 * @param owner the file's owner
 * @param inumber the inode's number
 */
inode_t *fs_open_request(file_table_t *file_table, const char *owner, int inumber) {
    inode_t *inode = inode_create(inumber);

    /* Add the inode into the active inode list */
    list_add(file_table->ilist->inode_list, inode);

    io_fs_log(owner, inumber, IO_LOG_FS_F_OPEN);
    sem_post(&io_mutex);

    return inode;
}

/**
 * It closes a file specified by the inode's number.
 *
 * @param file_table the file table
 * @param process the process in which has
 *                requested the close operation
 * @param inumber the inode's number
 */
void fs_close_request(file_table_t* file_table, process_t* process, int inumber) {
    FOREACH(file_table->ilist->inode_list, inode_t*) {
        if (it->id == inumber) {
            it->o_count--;

            /* It checks if that process that has been just finished */
            /* it was the last process using this file, then the file */
            /* will be closed. */
            if (it->o_count == 0) {
                /* Remove the current inode from the open file table */
                list_remove_node(file_table->ilist->inode_list, curr_node);

                io_fs_log(process->name, inumber, IO_LOG_FS_F_CLOSE);
                sem_post(&io_mutex);
            }

            break;
        }
    }
}