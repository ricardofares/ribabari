#ifndef OS_PROJECT_TERMINAL_H
#define OS_PROJECT_TERMINAL_H

#include "../tools/math.h"
#include "../memory/memory.h"

#include "log.h"

#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>

#define ENTER_KEY 10
#define BOX_OFFSET 2
#define TITLE_OFFSET 2
#define BOX_SIZE 2

#undef WIN_CREATE
#define WIN_CREATE(winn) win_##winn##_create()

#undef FWIN_CREATE
#define FWIN_CREATE(winn) win_##winn##_t *WIN_CREATE(winn)

#undef WIN_REFRESH
#define WIN_REFRESH(win) refresh_##win##_log()

#undef FWIN_REFRESH
#define FWIN_REFRESH(win) _Noreturn void *WIN_REFRESH(win)

#undef WIN_REFRESH_TITLE
#define WIN_REFRESH_TITLE(win) refresh_##win##_window_title()

#undef FWIN_REFRESH_TITLE
#define FWIN_REFRESH_TITLE(win) void WIN_REFRESH_TITLE(win)

#undef WIN_MEM_BUFFER_RS_SIZE
#define WIN_MEM_BUFFER_RS_SIZE (64)

#undef WIN_DISK_BUFFER_RS_SIZE
#define WIN_DISK_BUFFER_RS_SIZE (64)

#undef INFINITE_LOOP
#define INFINITE_LOOP while (1)

static const int EXIT = -1;
static const long SECOND_IN_US = 1000000L;

typedef struct {
    WINDOW *main_window;
    WINDOW *text_window;
    WINDOW *title_window;
} win_t;

typedef struct {
    /**
     * Window structure used to display the
     * memory view window.
     */
    win_t win;

    /* Additional Information */
    /**
     * This buffer is used to display information
     * at the right side of the memory window title.
     * Also, this buffer is used to provide alignment
     * with the right displayed content with relation
     * to the memory view title.
     */
    char *buffer_rs;
} win_memory_t;

typedef struct {
    /**
     * Window structure used to display the
     * disk view window.
     */
    win_t win;

    /* Additional Information */
    /**
     * This buffer is used to display information
     * at the right side of the disk window title.
     * Also, this buffer is used to provide alignment
     * with the right displayed content with relation
     * to the disk view title.
     */
    char *buffer_rs;
} win_disk_t;

typedef enum main_menu_choices {
    CREATE,
    TOGGLE_RESOURCE_VIEW
} main_menu_choices;

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
    win_t* menu_window;
} menu_t;

typedef struct {
    int begin_y;
    int begin_x;
} coordinates_t;


#define MAIN_MENU(M)                                                           \
    M("Process Create", CREATE)                                                \
    M("Toggle Resource Acquisition View", TOGGLE_RESOURCE_VIEW)                \
    M("Exit", EXIT)                                                            \

void menu_loop(menu_t* main_menu, void (*func)(int));

void init_screen();
void end_screen(void);

void start_menu_and_loop(menu_t* menu);
menu_t* create_menu(int choices_count,
                    menu_choice_t choices[static choices_count],
                    const char* title);
int get_type_from_string(menu_t* menu, const char* buffer);

win_t* init_menu_window(const menu_t* menu);
int begin_terminal();
void delete_menu(menu_t* menu);
void free_menu_window(win_t* window);

char* get_input_from_window(char* title, coordinates_t coordinates,
                            int buffer_size);
void print_with_window(char* string, char* title, int y, int x);

FWIN_REFRESH(process);
FWIN_REFRESH(memory);
FWIN_REFRESH(disk);
FWIN_REFRESH(io);
FWIN_REFRESH(res_acq);
_Noreturn void* refresh_logs();

win_t* init_io_log();
win_t* init_process_log();
FWIN_CREATE(memory);
FWIN_CREATE(disk);
win_t* init_res_acq_log();

FWIN_REFRESH_TITLE(disk);
FWIN_REFRESH_TITLE(memory);
FWIN_REFRESH_TITLE(res_acq);

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

/**
 * It returns the count of blocked processes
 * waiting for a semaphore to be available to
 * acquire. Note, this count is not for a
 * specific semaphore, but for all of them.
 *
 * @return the count of blocked processes
 *         waiting for a semaphore to be
 *         available to acquire.
 */
static int sem_blocked_count();

#endif // OS_PROJECT_TERMINAL_H

