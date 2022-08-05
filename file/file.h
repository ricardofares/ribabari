#ifndef OS_PROJECT_FILE_H
#define OS_PROJECT_FILE_H

#include "../utils/list.h"

typedef enum FilePermission {
    FILE_READ_PERM = 1,
    FILE_WRITE_PERM = 2,
    FILE_EXECUTE_PERM = 4
} file_perm_t;

typedef struct FileDescriptor {
    /**
     * It represents the inode id.
     */
    int id;

    /**
     * If it is true, then this inode
     * is representing a directory.
     * Otherwise, it is representing
     * a file.
     */
    int dir;

    /**
     * It holds an integer number indicating
     * the number of process that have this
     * file open.
     */
    int o_count;

    /**
     * It contains the access permissions
     * read, write, execute etc.
     */
    int acc_perm_flag;

    /**
     * It contains the time when this
     * file has been created.
     */
    long created_at;

    /**
     * It contains the time when this
     * file has been last accessed.
     */
    long last_accessed;

    /**
     * It contains the time when this
     * file has been last modified.
     */
    long last_modified;

    /**
     * It holds the file size measured
     * in bytes.
     */
    long size;
} inode_t;

typedef struct FileDescriptorList {
    /**
     * An inode list.
     */
    list_t* inode_list;
} ilist_t;

typedef struct FileTable {
    /**
     * It stores the current active open files.
     */
    ilist_t* ilist;

    /**
     * It holds the next inode id.
     */
    int next_inode_id;
} file_table_t;

/* Inode Function Prototypes */

/**
 * It returns a pointer to an inode.
 * If there is not enough memory to
 * allocate an inode, then the
 * application is exited.
 *
 * @return a pointer to an inode, however,
 *         if there is not enough memory
 *         to allocate an inode, then the
 *         application is exited.
 */
inode_t* inode_create();

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
ilist_t* ilist_create();

/* File Table Function Prototypes */

/**
 * It initializes the specified file table.
 *
 * @param file_table a pointer to a file table
 */
void file_table_init(file_table_t* file_table);

#endif // OS_PROJECT_FILE_H
