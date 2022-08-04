#include <pthread.h>
#include <time.h>

#include "disk.h"

#ifndef OS_SCHED_KERNEL
#define OS_SCHED_KERNEL
#include "../kernel/kernel.h"
#endif // OS_SCHED_KERNEL

#define MILLISECONDS_100 (100000000L)

/**
 * It creates the DiskScheduler-Thread that
 * is going to run the Hard DiskScheduler function
 * that is going to simulate the hard
 * disk work along the operating system
 * simulation.
 */
void disk_init() {
    pthread_t disk_id;
    pthread_attr_t disk_attr;

    pthread_attr_init(&disk_attr);
    pthread_attr_setscope(&disk_attr, PTHREAD_SCOPE_SYSTEM);

    pthread_create(&disk_id, NULL, (void*)disk, NULL);
}

/**
 * It represents the function that
 * is going to simulate the hard
 * disk work by moving its arm
 * over the disk surface making
 * reading and writing.
 */
_Noreturn void disk() {
    struct timespec start;
    struct timespec end;

    clock_gettime(CLOCK_REALTIME, &start);
    while (1) {
        clock_gettime(CLOCK_REALTIME, &end);
        const int elapsed = (end.tv_sec - start.tv_sec) * 1000000000L
                            + (end.tv_nsec - start.tv_nsec);

        if (elapsed >= MILLISECONDS_100) {
            start = end;

            if (kernel->disk_scheduler.forward_dir) {
                if (kernel->disk_scheduler.curr_track == DISK_TRACK_LIMIT)
                    kernel->disk_scheduler.forward_dir = 0;
                else kernel->disk_scheduler.curr_track++;
            } else {
                if (kernel->disk_scheduler.curr_track == 0)
                    kernel->disk_scheduler.forward_dir = 1;
                else kernel->disk_scheduler.curr_track--;
            }
        }
    }
}

/**
 * It initializes the disk scheduler.
 *
 * @param disk_scheduler a pointer to the disk
 *                       scheduler that will be
 *                       initialized.
 */
void disk_scheduler_init(disk_scheduler_t *disk_scheduler) {
    disk_scheduler->pending_requests = list_init();
    disk_scheduler->forward_dir = 1;
    disk_scheduler->curr_track = 0;
}

/**
 * It requests a read/write operation from the
 * disk on the specified track.
 *
 * @param process the process which request the
 *                read/write operation.
 * @param disk_scheduler the disk scheduler
 * @param track the requested track
 */
void disk_request(process_t* process, disk_scheduler_t *disk_scheduler, int track) {
    int time = DISK_OPERATION_TIME;

    /* It is going from the inner track to the outer one */
    if (disk_scheduler->forward_dir) {
        if (track >= disk_scheduler->curr_track)
            time += (track - disk_scheduler->curr_track) * DISK_TRACK_MOVE_TIME;
        else time += ((DISK_TRACK_LIMIT - disk_scheduler->curr_track) + (DISK_TRACK_LIMIT - track)) * DISK_TRACK_MOVE_TIME;
    }
    /* It is going from the outer track to the inner one */
    else {
        if (track < disk_scheduler->curr_track)
            time += (disk_scheduler->curr_track - track) * DISK_TRACK_MOVE_TIME;
        else time += (disk_scheduler->curr_track + track) * DISK_TRACK_MOVE_TIME;
    }

    process->remaining -= time;

    interruptControl(DISK_FINISH, process);
}