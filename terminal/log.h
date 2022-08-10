#ifndef OS_PROJECT_LOG_H
#define OS_PROJECT_LOG_H

#include "../utils/list.h"

/* Process Log Related Structures & Function Prototypes */

typedef struct {
    int is_proc;

    /**
     * It stores the proces's name.
     */
    char* name;

    /**
     * It stores the process' time
     * remaining.
     */
    int remaining;

    /**
     * It stores the process' segment id.
     */
    int id;

    /**
     * It stores the current process'
     * program counter.
     */
    int pc;

    /**
     * It stores the amount of open
     * files being used by this process.
     */
    int f_op_count;
} proc_log_info_t;

/**
 * It initializes the process logging subsystem.
 */
void process_log_init();

/**
 * It creates a process log and signal the
 * process view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void process_log(const char* process_name, const int remaining, const int pc, const int sid, const int f_op_count);

/**
 * It creates a process log and signal the
 * process view for further view update.
 * In this case, it is being logged that
 * no process is being executed.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void process_np_log();

/* Disk Log Related Structures & Function Prototypes */

typedef struct DiskLog {
    /**
     * It stores the amount of
     * disk read operation requests.
     */
    int r_req_count;

    /**
     * It stores the amount of
     * disk write operation requests.
     */
    int w_req_count;

    /**
     * It stores if the disk arm is
     * going from the inner track to
     * the outer one or vice-versa.
     */
    int forward_dir;

    /**
     * It stores the current track
     * in which the disk arm is on.
     */
    int curr_track;

    /**
     * It stores the angular velocity.
     */
    int angular_v;

    /**
     * It stores the amount of pending
     * disk read/write operations requests.
     */
    int pending_requests_size;
} disk_log_t;

typedef struct {
    /**
     * The process name.
     */
    char *proc_name;

    /**
     * The process id.
     */
    int proc_id;

    /**
     * The track in which the
     * process is reading/writing.
     */
    int track;

    /**
     * If is 1, then it indicates a
     * disk read operation. Otherwise,
     * if it is 0, then it indicates
     * a disk write operation.
     */
    int is_read;

    /**
     * The turnaround of that disk
     * operation. The turnaround time
     * mark the duration between the
     * disk operation request and
     * disk operation completion.
     */
    int turnaround;
} disk_log_info_t;

/**
 * It initializes the disk logging subsystem.
 */
void disk_log_init();

/**
 * It creates a disk log and signal the
 * disk view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void disk_log(const char* process_name, const int proc_id,
              const int track, const int read, const int turnaround);

/* I/O Log Related Structures & Function Prototypes */

typedef struct {
    /**
     * It stores the amount of bytes that
     * has been written by the disk to the
     * stored files.
     */
    long w_bytes;

    /**
     * It stores the amount of bytes that
     * has been read from the disk from the
     * stored files.
     */
    long r_bytes;

    /**
     * It stores the amount of time the
     * processes has been printing.
     */
    int p_time;
} io_log_t;

typedef enum IoLogType {
    /**
     * It indicates that the log is
     * carrying a print log. That is,
     * information about a process that
     * has requested a printing operation.
     */
    IO_LOG_PRINT,

    /**
     * It indicates that the log is
     * carrying a file system log. That is,
     * information about the file system
     * handling the files that is being
     * accessed, read and written.
     */
    IO_LOG_FILE_SYSTEM,

    /**
     * It indicates that the log is
     * carrying a disk request log. That is,
     * information about a process which
     * has requested a disk operation.
     */
    IO_LOG_DISK_REQUEST
} io_log_type_t;

typedef struct IoLogPrint {
    /**
     * The process name.
     */
    char* proc_name;

    /**
     * The print duration (measured in u.t.,
     * that is unit of time).
     */
    int duration;
} io_log_print_t;

typedef struct IoLogFileSystem {
    /**
     * The name of the process which
     * cause the file system to handle
     * some file.
     */
    char* proc_name;

    /**
     * The inode's number that is
     * being handled by the file system.
     */
    int inumber;

    /**
     * It indicates if the file system
     * is handling that file because
     * the process had requested a
     * disk read or write operation.
     */
    int read;
} io_log_fs_t;

typedef struct IoLogDiskRequest {
    /**
     * The process name which has
     * requested a disk operation.
     */
    char* proc_name;

    /**
     * It indicates if the requested
     * disk operation is a read or
     * write operation.
     */
    int read;
} io_log_disk_req_t;

typedef struct {
    /**
     * It stores the log type.
     */
    io_log_type_t type;

    /**
     * It stores a pointer to a print
     * log. This value is not null since
     * the log type is IO_LOG_PRINT.
     */
    io_log_print_t* print_log;

    /**
     * It stores a pointer to a file
     * system log. This value is not
     * null since the log type is
     * IO_LOG_FILE_SYSTEM.
     */
    io_log_fs_t* fs_log;

    /**
     * It stores a pointer to a disk
     * request log. This value is not
     * null since the log type is
     * IO_LOG_DISK_REQUEST.
     */
    io_log_disk_req_t* disk_req_log;
} io_log_info_t;

/**
 * It initializes the I/O logging subsystem.
 */
void io_log_init();

/**
 * It creates an I/O file system log and signal the
 * I/O view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void io_fs_log(const char* process_name, const int inumber, const int read);

/**
 * It creates an I/O disk log and signal the
 * I/O view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void io_disk_log(const char* process_name, const int read);

/**
 * It creates an I/O print log and signal the
 * I/O view for further view update.
 *
 * Further, the application closes immediately
 * if there is not enough memory to allocate
 * the needed internal structures to perform
 * the log operation.
 */
void io_print_log(const char* process_name, const int duration);

/* Global Variables */

extern list_t* process_log_list;
extern list_t* disk_log_list;
extern list_t* io_log_list;

extern disk_log_t* disk_general_log;
extern io_log_t* io_general_log;

#endif // OS_PROJECT_LOG_H
