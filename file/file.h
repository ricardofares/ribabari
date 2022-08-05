#ifndef OS_PROJECT_FILE_H
#define OS_PROJECT_FILE_H

typedef struct Inode {
    /**
     * If it is true, then this inode
     * is representing a directory.
     * Otherwise, it is representing
     * a file.
     */
    int dir;
} inode_t;

typedef struct Ilist {
    inode_t *inode_list;
} ilist_t;

#endif // OS_PROJECT_FILE_H
