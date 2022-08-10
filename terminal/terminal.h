#ifndef OS_PROJECT_TERMINAL_H
#define OS_PROJECT_TERMINAL_H
#include "../memory/memory.h"
#include "../utils/list.h"
#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define ENTER_KEY 10
#define BOX_OFFSET 2
#define TITLE_OFFSET 2

#define BOX_SIZE 2

static const int EXIT = -1;
static const long SECOND_IN_US = 1000000L;

extern list_t* process_log_list;
extern list_t* disk_log_list;
extern list_t* io_log_list;

extern sem_t log_mutex;
extern sem_t mem_mutex;
extern sem_t disk_mutex;
extern sem_t refresh_sem;
extern sem_t io_mutex;

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

extern disk_log_t* disk_general_log;

typedef struct {
    int is_proc;

    char* name;
    int remaining;
    int id;
    int pc;

    /**
     * It stores the amount of open
     * files being used by this process.
     */
    int f_op_count;
} proc_log_info_t;

typedef enum IoLogType {
    IO_LOG_PRINT,
    IO_LOG_FILE_SYSTEM,
    IO_LOG_DISK_REQUEST
} io_log_type_t;

typedef struct IoLogPrint {
    char* proc_name;
    int duration;
} io_log_print_t;

typedef struct IoLogFileSystem {
    char* proc_name;
    int inumber;
    int read;
} io_log_fs_t;

typedef struct IoLogDiskRequest {
    char* proc_name;
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

typedef struct {
    char *proc_name;
    int track;
    int is_read;
    int proc_id;
    int turnaround;
} disk_log_info_t;

typedef enum main_menu_choices {
    CREATE,
} main_menu_choices;

typedef struct menu_window_t {
    WINDOW* main_win;
    WINDOW* title_win;
    WINDOW* items_win;
} menu_window_t;

typedef struct menu_choice_t {
    const char* text;
    int value;
} menu_choice_t;

typedef struct menu_t {
    menu_choice_t* choices;
    int choices_count;
    const char* title;
    int height;
    int width;
    MENU* curses_menu;
    ITEM** items;
    menu_window_t* menu_window;
} menu_t;

typedef struct {
    int begin_y;
    int begin_x;
} coordinates_t;

typedef struct {
    WINDOW* main_window;
    WINDOW* text_window;
    WINDOW* title_window;
} io_window_t;

typedef struct {
    WINDOW* main_window;
    WINDOW* text_window;
    WINDOW* title_window;
} log_window_t;

#define MAIN_MENU(M)                                                           \
    M("Process create", CREATE)                                                \
    M("Exit", EXIT)

#define ONE_GIGABIT 1073741824
void menu_loop(menu_t* main_menu, void (*func)(int));

void init_screen();
void end_screen(void);

void start_menu_and_loop(menu_t* menu);
menu_t* create_menu(int choices_count,
                    menu_choice_t choices[static choices_count],
                    const char* title);
int get_type_from_string(menu_t* menu, const char* buffer);

menu_window_t* init_menu_window(const menu_t* menu);
int begin_terminal();
void delete_menu(menu_t* menu);
void free_menu_window(menu_window_t* window);

char* get_input_from_window(char* title, coordinates_t coordinates,
                            int buffer_size);
void print_with_window(char* string, char* title, int y, int x);

void process_log_init();
void disk_log_init();
void io_log_init();

_Noreturn void* refresh_process_log();
_Noreturn void* refresh_memory_log();
_Noreturn void* refresh_disk_log();
_Noreturn void* refresh_io_log();
_Noreturn void* refresh_logs();

log_window_t* init_io_log();
log_window_t* init_process_log();
log_window_t* init_memory_log();
log_window_t* init_disk_log();

void refresh_disk_title_window();
void refresh_memory_title_window();

/* Internal Terminal Function Prototypes */

/**
 * It returns a pointer to a process that has the
 * specified segment id. Otherwise, if there is not
 * exists such a process, then NULL is returned.
 *
 * @param sid the segment id
 *
 * @return a pointer to a process that has the
 *         specified segment id; otherwise, if
 *         there is not exists such a process,
 *         then NULL is returned.
 */
static process_t* get_process_sid(int sid);

/**
 * It returns the maximum segment size
 * allocated in the memory table.
 *
 * @return the maximum segment size
 *         allocated in the memory table
 */
static int max_seg_size();

/**
 * It calculates the page in the segment that
 * is in use by the process. The in use is in
 * the meaning that the process is executing
 * an instruction in that page currently.
 *
 * @param segment the segment
 *
 * @return the page index (a non-negative value);
 *         otherwise, returns -1 indicating that
 *         no page in that segment is being used.
 */
static int page_inuse_index(const segment_t* segment);

#endif // OS_PROJECT_TERMINAL_H

