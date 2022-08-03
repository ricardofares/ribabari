#ifndef OS_PROJECT_DISK_H
#define OS_PROJECT_DISK_H

#include "../utils/list.h"

/**
 * It represents an integer constant
 * holding the amount of track the
 * hard disk contains.
 */
#define DISK_TRACK_LIMIT (200)

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

#endif // OS_PROJECT_DISK_H
