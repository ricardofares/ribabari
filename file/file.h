#ifndef OS_PROJECT_FILE_H
#define OS_PROJECT_FILE_H

#include <time.h>

#include "../process/process.h"

/**
 * It calculates the inode number from
 * the disk block number.
 *
 * There is a simplification that has been
 * made for simulate the file system. In this
 * case the simplification is that the inode
 * number may be calculated using the disk
 * block number. In this case it is being
 * supposed that the inode with that calculated
 * number is in the specified block number.
 */
#define INODE_NUMBER(x) ((x) * 948 + 5)

/**
 * It calculates the disk block number from
 * the inode number.
 */
#define INVERSE_INODE_NUMBER(x) (((x) - 5) / 948)

#include "../utils/list.h"
#include "../process/process.h"

/* File Description Definitions */

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
     * file has been last accessed.
     */
    time_t last_accessed;

    /**
     * It contains the time when this
     * file has been last modified.
     */
    time_t last_modified;

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

/* File Table Structure Definitions */

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

typedef struct FileSystemOperationRequest {
    /**
     * It represents the process which
     * has requested a file system operation.
     */
    process_t* proc;

    /**
     * The current track in which the disk
     * is doing a read/write operation.
     */
    int track;

    /**
     * It indicates that the operation that
     * the disk is doing in the track is a
     * read operation since this value is 1.
     * Otherwise, it indicates that the disk
     * is doing a write operation in that track.
     */
    int read;
} fs_op_request_t;

/* Inode Function Prototypes */

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
inode_t* inode_create(int id);

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
int inode_cmp(void* i1, void* i2);

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
inode_t* find_inode(file_table_t* file_table, int inumber);

/* File Request Function Prototypes */

/**
 * It requests that the file system to handle the
 * content provided from a disk read operation.
 * Further, if the file read is not active, then an
 * inode is created for this file.
 *
 * @param file_table the file table
 * @param process the process which has requested
 *                a disk read operation
 * @param block the block in which the file is
 *              stored
 */
void fs_read_request(file_table_t* file_table, process_t* process, int block);

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
void fs_write_request(file_table_t* file_table, process_t* process, int block);

#endif // OS_PROJECT_FILE_H
