#ifndef OS_PROJECT_DISK_H
#define OS_PROJECT_DISK_H

#include "../utils/list.h"
#include "../process/process.h"

#define DISK_BLOCK(track) ((track) * 521 + 8)
#define INVERSE_DISK_BLOCK(block) (((block) - 8) / 521)

/**
 * It represents the disk base
 * angular velocity measured in
 * rpm (rotations per minute).
 */
#define DISk_BASE_ANGULAR_V (7200)

/**
 * It represents an integer constant
 * holding the amount of track the
 * hard disk contains.
 */
#define DISK_TRACK_LIMIT (200)

#define DISK_OPERATION_TIME (5000)
#define DISK_TRACK_MOVE_TIME (100)

typedef struct DiskScheduler {
    /**
     * It represents a linked list
     * containing the pending requests.
     */
    list_t* pending_requests;

    /**
     * If it is true, then the disk arm
     * is going from the inner track to
     * the outer one.
     */
    int forward_dir;

    /**
     * It holds the current track number
     * which the arm is on to.
     */
    int curr_track;

    /**
     * It stores the disk angular velocity.
     */
    int angular_v;
} disk_scheduler_t;

/**
 * It creates the DiskScheduler-Thread that
 * is going to run the Hard Disk function
 * that is going to simulate the hard
 * disk work along the operating system
 * simulation.
 */
void disk_init();

/**
 * It represents the function that
 * is going to simulate the hard
 * disk work by moving its arm
 * over the disk surface making
 * reading and writing.
 */
_Noreturn void disk();

/**
 * It initializes the disk scheduler.
 *
 * @param disk_scheduler a pointer to the disk
 *                       scheduler that will be
 *                       initialized.
 */
void disk_scheduler_init(disk_scheduler_t *disk_scheduler);

/**
 * It requests a read/write operation from the
 * disk on the specified track.
 *
 * @param process the process which request the
 *                read/write operation.
 * @param disk_scheduler the disk scheduler
 * @param track the requested track
 * @param read if is 1 indicates that the disk request
 *             is a read operation; otherwise, it indicates
 *             a write operation.
 */
void disk_request(process_t* process, disk_scheduler_t *disk_scheduler, int track, int read);

#endif // OS_PROJECT_DISK_H
