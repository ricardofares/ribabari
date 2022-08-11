#include <pthread.h>
#include <time.h>
#include <math.h>

#include "disk.h"

#ifndef OS_SCHED_KERNEL
#define OS_SCHED_KERNEL
#include "../kernel/kernel.h"
#include "../terminal/terminal.h"
#endif // OS_SCHED_KERNEL

#define MILLISECONDS_100 (50000000L)

/* Internal Disk Function Prototypes */

/**
 * It performs the read/write operation for
 * those pending requests that are over the
 * specified track.
 *
 * @param track the specified track
 */
static void rw_disk_op_resolve(int track);

/* Disk Function Prototypes */

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

            /* It resolves the disk read/write operations for the current track */
            rw_disk_op_resolve(kernel->disk_scheduler.curr_track);

            if (kernel->disk_scheduler.forward_dir) {
                if (kernel->disk_scheduler.curr_track == DISK_TRACK_LIMIT)
                    kernel->disk_scheduler.forward_dir = 0;
                else
                    kernel->disk_scheduler.curr_track++;
            } else {
                if (kernel->disk_scheduler.curr_track == 0)
                    kernel->disk_scheduler.forward_dir = 1;
                else
                    kernel->disk_scheduler.curr_track--;
            }

            /* It updates some miscellaneous information */
            kernel->disk_scheduler.angular_v = (int) (DISk_BASE_ANGULAR_V + 100 * sin(kernel->disk_scheduler.curr_track));

            /* It updates the disk forward direction log */
            disk_general_log->forward_dir = kernel->disk_scheduler.forward_dir;
            disk_general_log->curr_track = kernel->disk_scheduler.curr_track;
            disk_general_log->angular_v = kernel->disk_scheduler.angular_v;
            disk_general_log->pending_requests_size = kernel->disk_scheduler.pending_requests->size;
        }
    }
}

/* Disk Scheduler Function Prototypes */

/**
 * It initializes the disk scheduler.
 *
 * @param disk_scheduler a pointer to the disk
 *                       scheduler that will be
 *                       initialized.
 */
void disk_scheduler_init(disk_scheduler_t* disk_scheduler) {
    disk_scheduler->pending_requests = list_init();
    disk_scheduler->forward_dir = 1;
    disk_scheduler->curr_track = 0;
}

/* Disk Operation Function Prototypes */

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
void disk_request(process_t* process, disk_scheduler_t* disk_scheduler,
                  int track, int read) {
    int time = DISK_OPERATION_TIME;

    /* It is going from the inner track to the outer one */
    if (disk_scheduler->forward_dir) {
        if (track >= disk_scheduler->curr_track)
            time += (track - disk_scheduler->curr_track) * DISK_TRACK_MOVE_TIME;
        else
            time += ((DISK_TRACK_LIMIT - disk_scheduler->curr_track)
                     + (DISK_TRACK_LIMIT - track))
                    * DISK_TRACK_MOVE_TIME;
    }
    /* It is going from the outer track to the inner one */
    else {
        if (track < disk_scheduler->curr_track)
            time += (disk_scheduler->curr_track - track) * DISK_TRACK_MOVE_TIME;
        else
            time += (disk_scheduler->curr_track + track) * DISK_TRACK_MOVE_TIME;
    }

    /* It populates a disk request structure with the necessary */
    /* information for the simulated disk handle it */
    disk_request_t* disk_req = create_disk_request();
    disk_req->process = process;
    disk_req->track = track;
    disk_req->read = read;
    disk_req->turnaround = time;

    io_disk_log(process->name, read);
    sem_post(&io_mutex);

    /* Add the disk request into the pending requests */
    list_add(disk_scheduler->pending_requests, disk_req);

    process->remaining -= time;
}

/**
 * It returns a pointer to a disk request.
 * If there is not enough memory to allocate
 * the disk request, then the application is
 * exited.
 *
 * @return a pointer to a disk request, however,
 *         if there is not enough memory to allocate
 *         the disk request, then the application is
 *         exited.
 */
disk_request_t* create_disk_request() {
    disk_request_t* disk_req = (disk_request_t *)malloc(sizeof(disk_request_t));

    /* It check if the disk request could not be allocated */
    if (!disk_req) {
        printf("Not enough memory to allocate a disk request.\n");
        exit(EXIT_FAILURE);
    }

    return disk_req;
}

/* Internal Disk Function Definitions */

/**
 * It performs the read/write operation for
 * those pending requests that are over the
 * specified track.
 *
 * @param track the specified track
 */
static void rw_disk_op_resolve(int track) {
    list_node_t* curr_node;
    list_node_t* next_node;

    for (curr_node = kernel->disk_scheduler.pending_requests->head;
         curr_node != NULL;) {
        next_node = curr_node->next;
        disk_request_t* disk_req = (disk_request_t *)curr_node->content;

        /* It check if that pending requests is for that track */
        /* If so, then resolve its read/write disk operation */
        if (disk_req->track == track) {
            disk_log(disk_req->process->name, disk_req->process->id, track, disk_req->read, disk_req->turnaround);
            sem_post(&disk_mutex);

            io_fs_log(disk_req->process->name, INODE_NUMBER(DISK_BLOCK(track)),
                      disk_req->read ? IO_LOG_FS_READ : IO_LOG_FS_WRITE);
            sem_post(&io_mutex);

            /* After perform the disk read/write operation */
            /* emit a interrupt signal */
            interruptControl(DISK_FINISH, disk_req->process);

            list_remove_node(kernel->disk_scheduler.pending_requests, curr_node);
        }

        curr_node = next_node;
    }
}
