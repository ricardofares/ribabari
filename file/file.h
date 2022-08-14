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
    /**
     * File permission flags for
     * controlling the file's
     * owner access.
     */
    F_PERM_OWNER_R = (1 << 8),
    F_PERM_OWNER_W = (1 << 7),
    F_PERM_OWNER_X = (1 << 6),

    /**
     * File permission flags for
     * controlling the file's
     * group access.
     */
    F_PERM_GROUP_R = (1 << 5),
    F_PERM_GROUP_W = (1 << 4),
    F_PERM_GROUP_X = (1 << 3),

    /**
     * File permission flags for
     * controlling the file's
     * other access.
     */
    F_PERM_OTHER_R = (1 << 2),
    F_PERM_OTHER_W = (1 << 1),
    F_PERM_OTHER_X = (1 << 0),
} file_perm_t;

typedef struct {
    /**
     * It represents the inode id.
     */
    int id;

    /**
     * It holds an integer number indicating
     * the number of process that have this
     * file open.
     */
    int o_count;

    /**
     * It contains the file mode that specifies
     * the file's access control for the file's
     * owner, owner's group and others.
     */
    short mode;

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

/**
 * It opens a file specified by the inode's number.
 *
 * @param file_table the file table
 * @param owner the file's owner
 * @param inumber the inode's number
 */
inode_t *fs_open_request(file_table_t *file_table, const char *owner, int inumber);

 /**
  * It closes a file specified by the inode's number.
  *
  * @param file_table the file table
  * @param process the process in which has
  *                requested the close operation
  * @param inumber the inode's number
  */
void fs_close_request(file_table_t* file_table, process_t* process, int inumber);

#endif // OS_PROJECT_FILE_H
