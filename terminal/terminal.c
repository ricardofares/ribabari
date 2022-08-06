#include "terminal.h"
#include "../kernel/kernel.h"
#include <curses.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

sem_t log_mutex;
sem_t mem_mutex;
sem_t disk_mutex;
sem_t refresh_sem;
sem_t io_mutex;

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t refresh_mutex = PTHREAD_MUTEX_INITIALIZER;

list_t* process_log_list;
list_t* disk_log_list;
list_t* io_log_list;

disk_log_t* disk_general_log;

log_window_t* process_log;
log_window_t* memory_log;
log_window_t* disk_log;
log_window_t* io_log;
menu_window_t* menu_window;

void process_log_init() { process_log_list = list_init(); }

void disk_log_init() {
    disk_general_log = (disk_log_t *)malloc(sizeof(disk_log_t));

    /* It check if the disk log could not be allocated */
    if (!disk_general_log) {
        printf("Not enough memory to allocate the disk log.\n");
        exit(EXIT_FAILURE);
    }

    disk_general_log->r_req_count = 0;
    disk_general_log->w_req_count = 0;

    /* It initializes the disk log list */
    disk_log_list = list_init();
}

void io_log_init() { io_log_list = list_init(); }

void main_menu_functions(int x) {
    coordinates_t whatever = {
        .begin_x = -1,
        .begin_y = -1,
    };
    char* input;
    switch (x) {
        case CREATE:
            input = get_input_from_window("What is the name of the file?",
                                          whatever, 40);
            if (access(input, F_OK)) {
                print_with_window(
                    "The file could not be executed or it does not exist.",
                    "WARNING.", -1, -1);
                return;
            }
            sysCall(PROCESS_CREATE, (void*)input);
            break;
        default:
            break;
    }
}

void end_screen(void) {
    nocbreak();
    echo();
    curs_set(1);
    keypad(stdscr, FALSE);
    endwin();
}

void* refresh_logs(void* _) {
    while (1) {
        //        sem_wait(&refresh_sem);
        pthread_mutex_lock(&refresh_mutex);

        refresh();
        box(disk_log->title_window, 0, 0);
        box(disk_log->main_window, 0, 0);
        wrefresh(disk_log->main_window);

        box(process_log->title_window, 0, 0);
        box(process_log->main_window, 0, 0);
        wrefresh(process_log->main_window);

        box(memory_log->title_window, 0, 0);
        box(memory_log->main_window, 0, 0);
        wrefresh(memory_log->main_window);

        wattrset(menu_window->title_win, COLOR_PAIR(3));
        box(menu_window->title_win, 0, 0);
        box(menu_window->main_win, 0, 0);
        wrefresh(menu_window->main_win);

        box(io_log->title_window, 0, 0);
        box(io_log->main_window, 0, 0);
        wrefresh(io_log->main_window);
        pthread_mutex_unlock(&refresh_mutex);

        usleep(SECOND_IN_US / 10);
    }
}
#define SI(S, V) {S, V},
int begin_terminal() {
    atexit(end_screen);

    /* Initialize curses */
    init_screen();

    //    print_welcome();

    pthread_t log_thread;
    pthread_t memory_thread;
    pthread_t disk_thread;
    pthread_t io_thread;

    menu_choice_t choices[] = {MAIN_MENU(SI)};
    menu_t* main_menu = create_menu(ARRAY_SIZE(choices), choices, "Main Menu");

    process_log = init_process_log();
    pthread_create(&log_thread, NULL, refresh_process_log, NULL);

    memory_log = init_memory_log();
    pthread_create(&memory_thread, NULL, refresh_memory_log, NULL);

    disk_log = init_disk_log();
    pthread_create(&disk_thread, NULL, refresh_disk_log, NULL);

    io_log = init_io_log();
    pthread_create(&log_thread, NULL, refresh_io_log, NULL);

    start_menu_and_loop(main_menu, (void (*)(int))NULL);
    unpost_menu(main_menu->curses_menu);

    delete_menu(main_menu);
    end_screen();

    return 0;
}

menu_t* create_menu(int choices_count,
                    menu_choice_t choices[static choices_count],
                    const char* title) {

    menu_t* menu = malloc(sizeof(menu_t));
    (*menu) = (menu_t) {
        .height = choices_count + 4,
        //        .width = get_biggest_item(choices, choices_count, title) + 5,
        .width = COLS / 2,
        .choices_count = choices_count,
        .choices = choices,
        .title = title,
    };

    menu->items = (ITEM**)calloc(menu->choices_count + 1, sizeof(ITEM*));

    for (int i = 0; i < menu->choices_count; i++) {
        menu->items[i] = new_item(menu->choices[i].text, "");
    }

    menu->curses_menu = new_menu(menu->items);

    menu_window = init_menu_window(menu);
    menu->menu_window = menu_window;
    set_menu_fore(menu->curses_menu, COLOR_PAIR(1) | A_BOLD | A_UNDERLINE);
    set_menu_back(menu->curses_menu, COLOR_PAIR(1));
    set_menu_mark(menu->curses_menu, "* ");

    set_menu_win(menu->curses_menu, menu->menu_window->main_win);
    set_menu_sub(menu->curses_menu, menu->menu_window->items_win);

    return menu;
}

void title_print(WINDOW* title_win, int win_width, const char* title) {
    int titleMiddleY = 1;

    wattron(title_win, A_BOLD | COLOR_PAIR(1));
    mvwaddstr(title_win, titleMiddleY, win_width, title);
    wattroff(title_win, A_BOLD | COLOR_PAIR(1));
}

menu_window_t* init_menu_window(const menu_t* menu) {
    WINDOW* mainWin = newwin(menu->height, menu->width, 0, 0);

    const int titleHeight = 3;
    WINDOW* title_win = derwin(mainWin, titleHeight, menu->width, 0, 0);

    /* Create Boxes around windows */
    wattrset(title_win, COLOR_PAIR(3));
    wattrset(mainWin, COLOR_PAIR(3));
    box(title_win, 0, 0);
    box(mainWin, 0, 0);

    int titleOffset = 3;
    WINDOW* choiceWin = derwin(mainWin, menu->height - titleOffset,
                               menu->width - BOX_SIZE, titleOffset, BOX_SIZE);

    const int titleMiddleY = 1;

    // Print title with colors and bold
    title_print(title_win, (menu->width - strlen(menu->title)) / 2,
                menu->title);

    menu_window_t* newMenuWindow
        = (menu_window_t*)malloc(sizeof(menu_window_t));
    (*newMenuWindow) = (menu_window_t) {
        .main_win = mainWin,
        .title_win = title_win,
        .items_win = choiceWin,
        .height = menu->height,
        .width = menu->width,
        .begin_y = (LINES - menu->height) / 2,
        .begin_x = (COLS - menu->width) / 2,
    };

    return newMenuWindow;
}

void init_screen() {
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    // TITLE COLOR PAIR
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);

    // LOG COLOR PAIR
    init_pair(2, COLOR_CYAN, COLOR_BLACK);

    // BORDER COLOR PAIR
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    // BORDER COLOR PAIR
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
}

void print_item_index(menu_t* menu, const ITEM* item) {
    int y_local = menu->menu_window->begin_y + menu->menu_window->height;
    int x_local = menu->menu_window->begin_x;
    int win_size = menu->menu_window->width;
    move(x_local, 0);
    clrtoeol();

    char buffer[30];
    int buffer_size = sprintf(buffer, "Item %d was chosen.", item_index(item));
    mvprintw(y_local, x_local + (win_size - buffer_size) / 2, "%s", buffer);
}

void menu_loop(menu_t* menu, void (*func)(int)) {
    MENU* curses_menu = menu->curses_menu;
    WINDOW* main_window = menu->menu_window->main_win;
    int c;

    while ((c = wgetch(main_window))) {
        switch (c) {
            case 'j':
            case KEY_DOWN:
                menu_driver(curses_menu, REQ_DOWN_ITEM);
                break;
            case 'k':
            case KEY_UP:
                menu_driver(curses_menu, REQ_UP_ITEM);
                break;
            case ENTER_KEY:
                {
                    ITEM* cur = current_item(curses_menu);
                    int type = get_type_from_string(menu, item_name(cur));

                    if (type == EXIT) {
                        return;
                    }

                    if (func) {
                        wrefresh(menu->menu_window->main_win);
                        func(type);
                    }

                    break;
                }
            default:
                break;
        }
        c = 0;
    }
}

void start_menu_and_loop(menu_t* menu, void (*func)(int)) {
    MENU* curses_menu = menu->curses_menu;
    //    menu_window_t* menu_window = menu->menu_window;
    refresh();

    wattrset(menu->menu_window->items_win, COLOR_PAIR(3));

    post_menu(curses_menu);
    wrefresh(menu_window->main_win);

    pthread_t refresh_thread;
    pthread_create(&refresh_thread, NULL, refresh_logs, NULL);

    menu_loop(menu, main_menu_functions);
}

void print_welcome() {
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(LINES - 3, 1, "Press <ENTER> to see the option selected");
    mvprintw(LINES - 2, 1, "\"j\" to go down and \"k\" to go up.");
    attroff(COLOR_PAIR(2) | A_BOLD);
}

int get_type_from_string(menu_t* menu, const char* buffer) {
    for (int i = 0; i < menu->choices_count; i++) {
        if (!strcmp(menu->choices[i].text, buffer)) {
            return menu->choices[i].value;
        }
    }

    return -1;
}

const char* get_string_from_type(menu_t* menu, int type) {
    static const char* NO_MATCH = "";
    for (int i = 0; i < menu->choices_count; i++) {
        if (menu->choices[i].value == type) {
            return menu->choices[i].text;
        }
    }
    return NO_MATCH;
}

int get_biggest_item(menu_choice_t choices[], int choice_count,
                     const char* title) {
    int max = strlen(title);
    for (int i = 0; i < choice_count; i++) {
        int item_len = strlen(choices[i].text);
        if (item_len > max) {
            max = item_len;
        }
    }

    return max;
}

void delete_menu(menu_t* menu) {
    free_menu(menu->curses_menu);
    for (int i = 0; i < menu->choices_count; i++) {
        free(menu->items[i]);
    }
    free(menu->items);

    free_menu_window(menu->menu_window);
    free(menu->menu_window);
    free(menu);
}

void free_menu_window(menu_window_t* menu_window) {
    delwin(menu_window->items_win);
    delwin(menu_window->title_win);
    delwin(menu_window->main_win);
}

void input_refresh(io_window_t* input_win) {
    //    refresh();
    wrefresh(input_win->main_window);
    wrefresh(input_win->text_window);
    wrefresh(input_win->title_window);
    //    refresh();
}

void del_io_window(io_window_t* input_win) {
    delwin(input_win->text_window);
    delwin(input_win->title_window);

    wclear(input_win->main_window);
    wrefresh(input_win->main_window);

    delwin(input_win->main_window);
}

char* get_input_from_window(char* title, coordinates_t coordinates,
                            int buffer_size) {
    pthread_mutex_lock(&refresh_mutex);
    io_window_t io_win;
    echo();

    const int title_len = strlen(title);

    if (coordinates.begin_y < 0) {
        coordinates.begin_y = LINES / 2 - 1;
        coordinates.begin_x = (COLS - buffer_size + BOX_SIZE) / 2;
    }
    io_win.main_window = newwin(5, buffer_size + BOX_SIZE, coordinates.begin_y,
                                coordinates.begin_x);

    const int title_window_height = 3;
    io_win.title_window = derwin(io_win.main_window, title_window_height,
                                 buffer_size + BOX_SIZE, 0, 0);

    title_print(io_win.title_window,
                (buffer_size + BOX_SIZE - strlen(title)) / 2, title);

    wattrset(io_win.title_window, COLOR_PAIR(3));
    wattrset(io_win.main_window, COLOR_PAIR(3));
    box(io_win.title_window, 0, 0);
    box(io_win.main_window, 0, 0);

    io_win.text_window = derwin(io_win.main_window, 1, buffer_size + 1, 3, 1);

    input_refresh(&io_win);

    char* buffer = (char*)malloc(buffer_size);
    wmove(io_win.text_window, 0, 0);
    wattrset(io_win.text_window, COLOR_PAIR(2));
    wgetnstr(io_win.text_window, buffer, buffer_size);

    input_refresh(&io_win);
    del_io_window(&io_win);

    noecho();
    pthread_mutex_unlock(&refresh_mutex);
    return buffer;
}

void print_with_window(char* string, char* title, int y, int x) {

    io_window_t io_win;

    int string_len = strlen(string);
    int title_len = strlen(title);

    int window_width;
    if (string_len > title_len) {
        window_width = string_len + BOX_SIZE;
    } else {
        window_width = title_len + BOX_SIZE;
    }

    if (y < 0) {
        y = LINES / 2 - 1;
    }

    if (x < 0) {
        x = (COLS - window_width) / 2;
    }

    int window_height = 5;
    io_win.main_window = newwin(window_height, window_width, y, x);

    int title_height = 3;
    io_win.title_window = derwin(io_win.main_window, 3, window_width, 0, 0);
    io_win.text_window = derwin(io_win.main_window, 1, window_width - 2, 3, 1);

    pthread_mutex_lock(&refresh_mutex);
    input_refresh(&io_win);

    wattrset(io_win.title_window, COLOR_PAIR(3));
    wattrset(io_win.main_window, COLOR_PAIR(3));
    wattrset(io_win.text_window, COLOR_PAIR(3));
    box(io_win.title_window, 0, 0);
    box(io_win.main_window, 0, 0);

    title_print(io_win.title_window, (window_width - title_len) / 2, title);

    mvwprintw(io_win.text_window, 0, (window_width - 2 - string_len) / 2, "%s",
              string);
    input_refresh(&io_win);
    getch();

    del_io_window(&io_win);
    pthread_mutex_unlock(&refresh_mutex);
}

void* refresh_process_log(void* _) {
    sem_wait(&log_mutex);
    list_node_t* i = process_log_list->head;

    while (1) {
        sem_wait(&log_mutex);
        i = i->next;

        pthread_mutex_lock(&print_mutex);
        for (; i != NULL; i = i->next) {
            proc_log_info_t* log_info = ((proc_log_info_t*)i->content);
            if (!log_info->is_proc) {
                wprintw(process_log->text_window, "No process running.\n");
                break;
            }

            wattron(process_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(process_log->text_window, "PROCESS: ");
            wattroff(process_log->text_window, A_BOLD);

            wattron(process_log->text_window, COLOR_PAIR(3));
            wprintw(process_log->text_window, "%10s, ", log_info->name);

            wattron(process_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(process_log->text_window, "REMAINING: ");
            wattroff(process_log->text_window, A_BOLD);

            wattron(process_log->text_window, COLOR_PAIR(3));
            wprintw(process_log->text_window, "%4dms, ", log_info->remaining);

            wattron(process_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(process_log->text_window, "PC: ");
            wattroff(process_log->text_window, A_BOLD);

            wattron(process_log->text_window, COLOR_PAIR(3));
            wprintw(process_log->text_window, "%4d, ", log_info->pc);

            wattron(process_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(process_log->text_window, "ID: ");
            wattroff(process_log->text_window, A_BOLD);

            wattron(process_log->text_window, COLOR_PAIR(3));
            wprintw(process_log->text_window, "%2d.\n", log_info->id);
        }

        pthread_mutex_unlock(&print_mutex);

        i = process_log_list->tail;
        sem_post(&mem_mutex);
    }
}

void* refresh_memory_log(void* _) {
    list_t* seg_list = kernel->seg_table.seg_list;
    while (1) {
        // Print Memory Remaining the title
        wattron(memory_log->title_window, COLOR_PAIR(1) | A_BOLD);
        mvwprintw(memory_log->title_window, 1, 1, "Memory remaining: %7d bytes",
                  kernel->seg_table.remaining);
        mvwprintw(memory_log->title_window, 1, (COLS / 2) - 17, "Use: %.1lf%%/100%%",
                  (1073741824 - kernel->seg_table.remaining) * 100.0 / 1073741824.0);
        wattron(memory_log->title_window, COLOR_PAIR(3));

        sem_wait(&mem_mutex);
        wmove(memory_log->text_window, 0, 0);
        wclear(memory_log->text_window);

        pthread_mutex_lock(&print_mutex);
        for (list_node_t* i = seg_list->head; i != NULL; i = i->next) {
            const segment_t* seg = ((segment_t*)i->content);
            char* buffer = malloc(100);

            wattron(memory_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(memory_log->text_window, "ID: ");
            wattroff(memory_log->text_window, A_BOLD);

            wattron(memory_log->text_window, COLOR_PAIR(3));
            snprintf(buffer, 100, "%2d, ", seg->id);
            wprintw(memory_log->text_window, "%s", buffer);

            wattron(memory_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(memory_log->text_window, "Segment Size: ");
            wattroff(memory_log->text_window, A_BOLD);

            wattron(memory_log->text_window, COLOR_PAIR(3));
            snprintf(buffer, 100, "%10d bytes, ", seg->size);
            wprintw(memory_log->text_window, "%s", buffer);

            wattron(memory_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(memory_log->text_window, "Pages count: ");
            wattroff(memory_log->text_window, A_BOLD);

            wattron(memory_log->text_window, COLOR_PAIR(3));
            const int p_inuse_index = page_inuse_index(seg);
            if (p_inuse_index == -1)
                snprintf(buffer, 100, "%6d available, no page is being used\n",
                         seg->page_qtd);
            else
                snprintf(buffer, 100, "%6d available, page %d in use\n",
                         seg->page_qtd, p_inuse_index);
            wprintw(memory_log->text_window, "%s", buffer);

            wattroff(memory_log->text_window, COLOR_PAIR(2));
        }
        pthread_mutex_unlock(&print_mutex);
    }
}

log_window_t* init_process_log() {
    const int main_height = LINES / 2;
    const int main_width = COLS / 2;

    WINDOW* main_window = newwin(main_height, main_width, 0, COLS / 2);
    WINDOW* text_window = derwin(
        main_window, main_height - TITLE_OFFSET - BOX_OFFSET,
        main_width - BOX_OFFSET + 1, (BOX_OFFSET / 2) + TITLE_OFFSET, 1);
    WINDOW* title_window = derwin(main_window, 3, COLS / 2, 0, 0);
    scrollok(text_window, 1);
    wattrset(text_window, COLOR_PAIR(3));

    const char title[] = "Process View";
    title_print(title_window, (main_width - (int)strlen(title)) / 2, title);

    wattrset(main_window, COLOR_PAIR(3));
    wattrset(title_window, COLOR_PAIR(3));

    log_window_t* log_window = malloc(sizeof(log_window_t));
    (*log_window) = (log_window_t) {
        .main_window = main_window,
        .text_window = text_window,
        .title_window = title_window,
    };

    return log_window;
}

log_window_t* init_memory_log() {
    const int main_height = LINES / 2;
    const int main_width = COLS / 2;

    WINDOW* main_window = newwin(main_height, main_width, LINES / 2, COLS / 2);
    WINDOW* text_window = derwin(
        main_window, main_height - TITLE_OFFSET - BOX_OFFSET,
        main_width - BOX_OFFSET - 1, (BOX_OFFSET / 2) + TITLE_OFFSET, 1);
    WINDOW* title_window = derwin(main_window, 3, main_width, 0, 0);
    scrollok(text_window, 1);

    char title[] = "Memory View";
    title_print(title_window, (main_width - (int)strlen(title)) / 2, title);

    wattrset(main_window, COLOR_PAIR(3));
    wattrset(title_window, COLOR_PAIR(3));

    log_window_t* log_window = malloc(sizeof(log_window_t));
    (*log_window) = (log_window_t) {
        .main_window = main_window,
        .text_window = text_window,
        .title_window = title_window,
    };

    return log_window;
}

/* Internal Terminal Function Definitions */

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
static int page_inuse_index(const segment_t* segment) {
    int i;

    for (i = segment->page_count - 1; i >= 0; i--)
        if (segment->page_table[i].used)
            goto page_in_use;
    return -1;
page_in_use:
    {
        const process_t* proc = get_process_sid(segment->id);
        return MAX(1, proc->pc / (double)proc->code_len * segment->page_qtd);
    };
}

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
static process_t* get_process_sid(const int sid) {
    const list_t* proc_table = kernel->proc_table;
    list_node_t* curr_node;

    for (curr_node = proc_table->head; curr_node != NULL;
         curr_node = curr_node->next) {
        process_t* proc = (process_t*)curr_node->content;
        if (proc->seg_id == sid)
            return proc;
    }

    return NULL;
}

log_window_t* init_disk_log() {
    const int main_height = LINES / 2;
    const int main_width = COLS / 2;

    const int text_offset = BOX_OFFSET + TITLE_OFFSET;
    WINDOW* main_window = newwin(main_height, main_width, LINES / 2, 0);
    WINDOW* text_window
        = derwin(main_window, main_height - text_offset,
                 main_width - (BOX_OFFSET + 1), BOX_OFFSET + 1, 1);
    WINDOW* title_window = derwin(main_window, 3, main_width, 0, 0);

    scrollok(text_window, 1);
    wattrset(text_window, COLOR_PAIR(2));

    const char title[] = "Disk View";
    title_print(title_window, (main_width - (int)strlen(title)) / 2, title);

    wattrset(main_window, COLOR_PAIR(3));
    wattrset(title_window, COLOR_PAIR(3));

    log_window_t* log_window = malloc(sizeof(log_window_t));
    (*log_window) = (log_window_t) {
        .main_window = main_window,
        .text_window = text_window,
        .title_window = title_window,
    };

    return log_window;
}

void* refresh_disk_log(void* _) {
    sem_wait(&disk_mutex);
    list_node_t* i = disk_log_list->head;

    while (1) {

        char* buffer = (char *)malloc(sizeof(char) * 100);
        pthread_mutex_lock(&print_mutex);
        sprintf(buffer, "DIR: %s    R: %d    W: %d",
                disk_general_log->forward_dir ? "FORWARD" : "BACKWARD",
                disk_general_log->r_req_count,
                disk_general_log->w_req_count);

        wattron(disk_log->title_window, COLOR_PAIR(1) | A_BOLD);
        mvwprintw(disk_log->title_window, 1, (COLS / 2) - strlen(buffer) - 2, buffer);
        wattron(disk_log->title_window, COLOR_PAIR(3) | A_BOLD);
        free(buffer);
        for (; i != NULL; i = i->next) {
            disk_log_info_t* disk_info = ((disk_log_info_t*)i->content);

            wattron(disk_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(disk_log->text_window, "Process ");

            wattron(disk_log->text_window, COLOR_PAIR(3) | A_NORMAL);
            wprintw(disk_log->text_window, "%s ", disk_info->proc_name);

            wattron(disk_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(disk_log->text_window, "is ");

            wattron(disk_log->text_window, COLOR_PAIR(3) | A_NORMAL);
            if (disk_info->is_read) {
                wprintw(disk_log->text_window, "reading ");
            } else {
                wprintw(disk_log->text_window, "writing ");
            }

            wattron(disk_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(disk_log->text_window, "on track ");

            wattron(disk_log->text_window, COLOR_PAIR(3) | A_NORMAL);
            wprintw(disk_log->text_window, "%d", disk_info->track);

            wattron(disk_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(disk_log->text_window, ".\n");
        }
        pthread_mutex_unlock(&print_mutex);

        i = disk_log_list->tail;

        sem_wait(&disk_mutex);
        i = i->next;
    }
}

log_window_t* init_io_log() {
    const int main_height = LINES / 2;
    const int main_width = COLS / 2;

    const int text_offset = BOX_OFFSET + TITLE_OFFSET;
    WINDOW* main_window = newwin(main_height - 6, main_width, 6, 0);
    WINDOW* text_window
        = derwin(main_window, main_height - 6 - text_offset,
                 main_width - (BOX_OFFSET + 1), BOX_OFFSET + 1, 1);
    WINDOW* title_window = derwin(main_window, 3, main_width, 0, 0);

    scrollok(text_window, 1);
    wattrset(text_window, COLOR_PAIR(2));

    const char title[] = "I/O View";
    title_print(title_window, (main_width - (int)strlen(title)) / 2, title);

    wattrset(main_window, COLOR_PAIR(3));
    wattrset(title_window, COLOR_PAIR(3));

    log_window_t* log_window = malloc(sizeof(log_window_t));
    (*log_window) = (log_window_t) {
        .main_window = main_window,
        .text_window = text_window,
        .title_window = title_window,
    };

    return log_window;
}

void* refresh_io_log(void* _) {
    sem_wait(&io_mutex);
    list_node_t* i = io_log_list->head;

    while (1) {
        pthread_mutex_lock(&print_mutex);
        for (; i != NULL; i = i->next) {
            io_log_info_t * io_info = ((io_log_info_t*)i->content);

            wattron(io_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(io_log->text_window, "Process ");

            wattron(io_log->text_window, COLOR_PAIR(3) | A_NORMAL);
            wprintw(io_log->text_window, "%s ", io_info->proc_name);

            wattron(io_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(io_log->text_window, "is printing for ");

            wattron(io_log->text_window, COLOR_PAIR(3) | A_NORMAL);
            wprintw(io_log->text_window, "%d ", io_info->duration);

            wattron(io_log->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(io_log->text_window, "u.t.\n");
        }
        pthread_mutex_unlock(&print_mutex);

        i = io_log_list->tail;

        sem_wait(&io_mutex);
    }
}