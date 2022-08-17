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
sem_t res_acq_mutex;

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t refresh_mutex = PTHREAD_MUTEX_INITIALIZER;

list_t* process_log_list;
list_t* disk_log_list;
list_t* io_log_list;
list_t* res_acq_log_list;

disk_log_t* disk_general_log;
io_log_t* io_general_log;

win_t* process_window;
win_t* memory_window;
win_t* disk_window;
win_t* io_window;
win_t* menu_window;
win_t* res_acq_window;

int r_view = 0;

void main_menu_functions(int menu_function) {
    coordinates_t whatever = {
        .begin_x = -1,
        .begin_y = -1,
    };

    switch (menu_function) {
        case CREATE: {
            char* input = get_input_from_window("What is the name of the file?", whatever,
                                                40);

            if (access(input, F_OK)) {
                print_with_window(
                        "The file could not be executed or it does not exist.",
                        "WARNING.", -1, -1);
                return;
            }

            /* It performs a system call tto create the process */
            sysCall(PROCESS_CREATE, (void *)input);

            free(input);
            break;
        }
        case TOGGLE_RESOURCE_VIEW: {
            r_view = !r_view;
            break;
        }
        default: {
            printf("Menu function %d unrecognized.\n", menu_function);
            break;
        }
    }
}

void end_screen(void) {
    nocbreak();
    echo();
    curs_set(1);
    keypad(stdscr, FALSE);
    endwin();
}

_Noreturn void* refresh_logs() {
    while (1) {
        pthread_mutex_lock(&refresh_mutex);

        refresh();

	wattrset(disk_window->title_window, COLOR_PAIR(3));
        box(disk_window->title_window, 0, 0);
        box(disk_window->main_window, 0, 0);
        wrefresh(disk_window->main_window);

        box(process_window->title_window, 0, 0);
        box(process_window->main_window, 0, 0);
        wrefresh(process_window->main_window);

        if (r_view == 0) {
	    wattrset(memory_window->title_window, COLOR_PAIR(3));
            box(memory_window->title_window, 0, 0);
            box(memory_window->main_window, 0, 0);
            wrefresh(memory_window->main_window);
            wrefresh(memory_window->title_window);
        } else {
	    wattrset(res_acq_window->title_window, COLOR_PAIR(3));
            box(res_acq_window->title_window, 0, 0);
            box(res_acq_window->main_window, 0, 0);
            wrefresh(res_acq_window->main_window);

            pthread_mutex_lock(&print_mutex);
            refresh_res_acq_title_window();
            pthread_mutex_unlock(&print_mutex);
        }

        wattrset(menu_window->title_window, COLOR_PAIR(3));
        box(menu_window->title_window, 0, 0);
        box(menu_window->main_window, 0, 0);
        wrefresh(menu_window->main_window);

        pthread_mutex_lock(&print_mutex);
        refresh_disk_title_window();
        pthread_mutex_unlock(&print_mutex);

        wattrset(io_window->title_window, COLOR_PAIR(3));
        box(io_window->title_window, 0, 0);
        box(io_window->main_window, 0, 0);
        wrefresh(io_window->main_window);
        pthread_mutex_unlock(&refresh_mutex);

        usleep(SECOND_IN_US / 6);
    }
}
#define SI(S, V) {S, V},
int begin_terminal() {
    atexit(end_screen);

    /* Initialize curses */
    init_screen();

    pthread_t log_thread;
    pthread_t memory_thread;
    pthread_t disk_thread;
    pthread_t io_thread;
    pthread_t res_acq_thread;

    menu_choice_t choices[] = {MAIN_MENU(SI)};
    menu_t* main_menu = create_menu(ARRAY_SIZE(choices), choices, "Main Menu");

    process_window = init_process_log();
    pthread_create(&log_thread, NULL, refresh_process_log, NULL);

    memory_window = init_memory_log();
    pthread_create(&memory_thread, NULL, refresh_memory_log, NULL);

    disk_window = init_disk_log();
    pthread_create(&disk_thread, NULL, refresh_disk_log, NULL);

    io_window = init_io_log();
    pthread_create(&io_thread, NULL, refresh_io_log, NULL);

    res_acq_window = init_res_acq_log();
    pthread_create(&res_acq_thread, NULL, refresh_res_acq_log, NULL);

    start_menu_and_loop(main_menu);
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

    set_menu_win(menu->curses_menu, menu->menu_window->main_window);
    set_menu_sub(menu->curses_menu, menu->menu_window->text_window);

    return menu;
}

void title_print(WINDOW* title_win, int win_width, const char* title) {
    int titleMiddleY = 1;

    wattron(title_win, A_BOLD | COLOR_PAIR(1));
    mvwaddstr(title_win, titleMiddleY, win_width, title);
    wattroff(title_win, A_BOLD | COLOR_PAIR(1));
}

win_t* init_menu_window(const menu_t* menu) {
    WINDOW* main_win = newwin(menu->height, menu->width, 0, 0);

    const int title_height = 3;
    WINDOW* title_win = derwin(main_win, title_height, menu->width, 0, 0);

    /* Create Boxes around windows */
    wattrset(title_win, COLOR_PAIR(3));
    wattrset(main_win, COLOR_PAIR(3));
    box(title_win, 0, 0);
    box(main_win, 0, 0);

    int title_offset = 3;
    WINDOW* choice_win = derwin(main_win, menu->height - title_offset,
                                menu->width - BOX_SIZE, title_offset, BOX_SIZE);

    // Print title with colors and bold
    title_print(title_win, (menu->width - (int)strlen(menu->title)) / 2,
                menu->title);

    win_t* new_menu_window
        = (win_t*)malloc(sizeof(win_t));
    (*new_menu_window) = (win_t) {
        .main_window = main_win,
        .title_window = title_win,
        .text_window = choice_win,
    };

    // For using the UP/DOWN key
    keypad(new_menu_window->main_window, TRUE);

    return new_menu_window;
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

void menu_loop(menu_t* menu, void (*func)(int)) {
    MENU* curses_menu = menu->curses_menu;
    WINDOW* main_window = menu->menu_window->main_window;
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
                        wrefresh(menu->menu_window->main_window);
                        func(type);
                    }

                    break;
                }
            default:
                break;
        }
    }
}

void start_menu_and_loop(menu_t* menu) {
    MENU* curses_menu = menu->curses_menu;
    //    win_t* menu_window = menu->menu_window;
    refresh();

    wattrset(menu->menu_window->text_window, COLOR_PAIR(3));

    post_menu(curses_menu);
    wrefresh(menu_window->main_window);

    pthread_t refresh_thread;
    pthread_create(&refresh_thread, NULL, refresh_logs, NULL);

    menu_loop(menu, main_menu_functions);
}

int get_type_from_string(menu_t* menu, const char* buffer) {
    for (int i = 0; i < menu->choices_count; i++) {
        if (!strcmp(menu->choices[i].text, buffer)) {
            return menu->choices[i].value;
        }
    }

    return -1;
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

void free_menu_window(win_t* window) {
    delwin(window->text_window);
    delwin(window->title_window);
    delwin(window->main_window);
}

void input_refresh(win_t* input_win) {
    //    refresh();
    wrefresh(input_win->main_window);
    wrefresh(input_win->text_window);
    wrefresh(input_win->title_window);
    //    refresh();
}

void del_io_window(win_t* input_win) {
    delwin(input_win->text_window);
    delwin(input_win->title_window);

    wclear(input_win->main_window);
    wrefresh(input_win->main_window);

    delwin(input_win->main_window);
}

char* get_input_from_window(char* title, coordinates_t coordinates,
                            int buffer_size) {
    pthread_mutex_lock(&refresh_mutex);
    win_t io_win;
    echo();

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
                (buffer_size + BOX_SIZE - (int)strlen(title)) / 2, title);

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

    win_t io_win;

    int string_len = (int)strlen(string);
    int title_len = (int)strlen(title);

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

_Noreturn void* refresh_process_log() {
    sem_wait(&log_mutex);
    list_node_t* i = process_log_list->head;

    while (1) {
        sem_wait(&log_mutex);
        i = i->next;

        pthread_mutex_lock(&print_mutex);
        for (; i != NULL; i = i->next) {
            proc_log_info_t* log_info = ((proc_log_info_t*)i->content);
            if (!log_info->is_proc) {
                wprintw(process_window->text_window, "No process running.\n");
                break;
            }

            wattrset(process_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "PROCESS: ");

            wattrset(process_window->text_window, COLOR_PAIR(3) | COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "%10s, ", log_info->name);

            wattrset(process_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "REMAINING: ");

            wattrset(process_window->text_window, COLOR_PAIR(3) | COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "%4dms, ",
                    log_info->remaining);

            wattrset(process_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "PC: ");

            wattrset(process_window->text_window, COLOR_PAIR(3) | COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "%4d, ", log_info->pc);

            wattrset(process_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "ID: ");

            wattrset(process_window->text_window, COLOR_PAIR(3) | COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "%2d, ", log_info->id);

            wattrset(process_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "FILES: ");

            wattrset(process_window->text_window, COLOR_PAIR(3) | COLOR_PAIR(1) | A_BOLD);
            wprintw(process_window->text_window, "%2d.\n", log_info->f_op_count);
        }

        pthread_mutex_unlock(&print_mutex);

        i = process_log_list->tail;
        sem_post(&mem_mutex);
    }
}

void refresh_memory_title_window() {
    // Print Memory Remaining the title

    char *usage_buffer = malloc(sizeof(char) * 64);
    int usage_buffer_size = sprintf(usage_buffer, "LSS: %d Kbytes Use: %.1lf%%",
                                    max_seg_size() >> 10,
                                    (ONE_GIGABIT - kernel->seg_table.remaining) * 100.0 / ONE_GIGABIT);

    wclear(memory_window->title_window);
    wattron(memory_window->title_window, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(memory_window->title_window, 1, 1, "Remaining: %d Kbytes",
              kernel->seg_table.remaining >> 10);
    mvwprintw(memory_window->title_window, 1,
              (COLS / 2) - usage_buffer_size - (BOX_SIZE / 2), "%s", usage_buffer);
    wattron(memory_window->title_window, COLOR_PAIR(3));

    const char title[] = "Memory View";

    wattron(memory_window->title_window, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(memory_window->title_window, 1,
              (COLS / 2 - (int)strlen(title)) / 2, title);
    wattron(memory_window->title_window, COLOR_PAIR(3) | A_BOLD);
}

_Noreturn void* refresh_memory_log() {
    list_t* seg_list = kernel->seg_table.seg_list;
    while (1) {

        sem_wait(&mem_mutex);
        wmove(memory_window->text_window, 0, 0);
        wclear(memory_window->text_window);

        pthread_mutex_lock(&print_mutex);
        refresh_memory_title_window();
        for (list_node_t* i = seg_list->head; i != NULL; i = i->next) {
            const segment_t* seg = ((segment_t*)i->content);
            char* buffer = malloc(100);

            wattrset(memory_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(memory_window->text_window, "ID: ");

            wattrset(memory_window->text_window, COLOR_PAIR(3) | COLOR_PAIR(1) | A_BOLD);
            snprintf(buffer, 100, "%2d, ", seg->id);
            wprintw(memory_window->text_window, "%s", buffer);

            wattrset(memory_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(memory_window->text_window, "SegSize: ");

            wattrset(memory_window->text_window, COLOR_PAIR(3) | COLOR_PAIR(1) | A_BOLD);
            snprintf(buffer, 100, "%7d kb, ", seg->size / 1024);
            wprintw(memory_window->text_window, "%s", buffer);

            wattrset(memory_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(memory_window->text_window, "Pages: ");

            wattrset(memory_window->text_window, COLOR_PAIR(3) | COLOR_PAIR(1) | A_BOLD);
            const int p_inuse_index = page_inuse_index(seg);
            if (p_inuse_index == -1)
                snprintf(buffer, 100, "%d available.\n", seg->page_qtd);
            else
                snprintf(buffer, 100, "using %d of %d.\n", p_inuse_index,
                         seg->page_qtd);
            wprintw(memory_window->text_window, "%s", buffer);

            // wattroff(memory_window->text_window, COLOR_PAIR(2));

            refresh_memory_title_window();
        }
        pthread_mutex_unlock(&print_mutex);
    }
}

win_t* init_process_log() {
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

    win_t* log_window = malloc(sizeof(win_t));
    (*log_window) = (win_t) {
        .main_window = main_window,
        .text_window = text_window,
        .title_window = title_window,
    };

    return log_window;
}

win_t* init_memory_log() {
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

    win_t* log_window = malloc(sizeof(win_t));
    (*log_window) = (win_t) {
        .main_window = main_window,
        .text_window = text_window,
        .title_window = title_window,
    };

    return log_window;
}

win_t* init_disk_log() {
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

    win_t* log_window = malloc(sizeof(win_t));
    (*log_window) = (win_t) {
        .main_window = main_window,
        .text_window = text_window,
        .title_window = title_window,
    };

    return log_window;
}

win_t* init_res_acq_log() {
    const int main_height = LINES / 2;
    const int main_width = COLS / 2;

    WINDOW* main_window = newwin(main_height, main_width, LINES / 2, COLS / 2);
    WINDOW* text_window = derwin(
            main_window, main_height - TITLE_OFFSET - BOX_OFFSET,
            main_width - BOX_OFFSET - 1, (BOX_OFFSET / 2) + TITLE_OFFSET, 1);
    WINDOW* title_window = derwin(main_window, 3, main_width, 0, 0);
    scrollok(text_window, 1);
    wattrset(text_window, COLOR_PAIR(2));

    const char title[] = "Resource Acquisition View";
    title_print(title_window, (main_width - (int)strlen(title)) / 2, title);

    wattrset(main_window, COLOR_PAIR(3));
    wattrset(title_window, COLOR_PAIR(3));

    win_t* log_window = malloc(sizeof(win_t));
    (*log_window) = (win_t) {
            .main_window = main_window,
            .text_window = text_window,
            .title_window = title_window,
    };

    return log_window;
}

void refresh_disk_title_window() {
    //    pthread_mutex_lock(&print_mutex);
    char* disk_direction = (char*)malloc(sizeof(char) * 32);
    char* disk_track = (char*)malloc(sizeof(char) * 32);
    char* disk_velocity = (char*)malloc(sizeof(char) * 32);
    char* disk_quantity = (char*)malloc(sizeof(char) * 32);
    sprintf(disk_direction, "DIR: %s ",
            disk_general_log->forward_dir ? "F" : "B");
    int disk_velocity_size
        = sprintf(disk_velocity, "AngV: %d rpm", disk_general_log->angular_v);
    sprintf(disk_track, "Track: %d Requests: %d", disk_general_log->curr_track, disk_general_log->pending_requests_size);
    int disk_qtd_size
        = sprintf(disk_quantity, "R: %d W: %d",
                  disk_general_log->r_req_count, disk_general_log->w_req_count);

    const char title[] = "Disk View";
    wclear(disk_window->title_window);

    wattron(disk_window->title_window, COLOR_PAIR(1) | A_BOLD);
    wmove(disk_window->title_window, 1, 1);
    wprintw(disk_window->title_window, "%s", disk_direction);
    wprintw(disk_window->title_window, "%s", disk_track);
    mvwprintw(disk_window->title_window, 1, (COLS / 2) - disk_qtd_size - 1,
              "%s", disk_quantity);
    mvwprintw(disk_window->title_window, 1,
              (COLS / 2) - disk_qtd_size - 2 - disk_velocity_size, "%s",
              disk_velocity);
    title_print(disk_window->title_window, (COLS / 2 - (int)strlen(title)) / 2,
                title);
    wattron(disk_window->title_window, COLOR_PAIR(3) | A_BOLD);
    free(disk_direction);
    free(disk_track);
    free(disk_velocity);
    free(disk_quantity);
    //    pthread_mutex_unlock(&print_mutex);
}

_Noreturn void* refresh_disk_log() {
    sem_wait(&disk_mutex);
    list_node_t* i = disk_log_list->head;

    while (1) {
        pthread_mutex_lock(&print_mutex);
        refresh_disk_title_window();
        for (; i != NULL; i = i->next) {
            disk_log_info_t* disk_info = ((disk_log_info_t*)i->content);

            wattron(disk_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(disk_window->text_window, "Process ");

            wattron(disk_window->text_window, COLOR_PAIR(3) | A_NORMAL);
            wprintw(disk_window->text_window, "%s ", disk_info->proc_name);

            wattron(disk_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(disk_window->text_window, "has ");

            wattron(disk_window->text_window, COLOR_PAIR(3) | A_NORMAL);
            if (disk_info->is_read) {
                wprintw(disk_window->text_window, "read ");
            } else {
                wprintw(disk_window->text_window, "written ");
            }

            wattron(disk_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(disk_window->text_window, "on track ");

            wattron(disk_window->text_window, COLOR_PAIR(3) | A_NORMAL);
            wprintw(disk_window->text_window, "%d", disk_info->track);

            wattron(disk_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(disk_window->text_window, " with a turnaround of ");

            wattron(disk_window->text_window, COLOR_PAIR(3) | A_NORMAL);
            wprintw(disk_window->text_window, "%du.t.\n", disk_info->turnaround);
        }
        pthread_mutex_unlock(&print_mutex);

        i = disk_log_list->tail;

        sem_wait(&disk_mutex);
        i = i->next;
    }
}

void refresh_res_acq_title_window() {
    char* left_title_string = (char*)malloc(sizeof(char) * 16);
    sprintf(left_title_string, "Semaphore Count: %d Blocked: %d",
            kernel->sem_table.len, sem_blocked_count());

    const char title[] = "Resource Acquisition View";
    wclear(res_acq_window->title_window);

    wattron(res_acq_window->title_window, COLOR_PAIR(1) | A_BOLD);
    wmove(res_acq_window->title_window, 1, 1);
    wprintw(res_acq_window->title_window, "%s", left_title_string);

    title_print(res_acq_window->title_window, (COLS / 2 - (int)strlen(title)) / 2,
                title);

    wattron(res_acq_window->title_window, COLOR_PAIR(3) | A_BOLD);
    free(left_title_string);
}


_Noreturn void* refresh_res_acq_log() {
    sem_wait(&res_acq_mutex);

    list_node_t* i = res_acq_log_list->head;

    while (1) {
        pthread_mutex_lock(&print_mutex);
        refresh_res_acq_title_window();
        for (; i != NULL; i = i->next) {
            res_acq_log_t* log = ((res_acq_log_t *)i->content);

            wattron(res_acq_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(res_acq_window->text_window, "Process ");

            wattron(res_acq_window->text_window, COLOR_PAIR(3) | A_NORMAL);
            wprintw(res_acq_window->text_window, "%s ", log->proc_name);

            wattron(res_acq_window->text_window, COLOR_PAIR(1) | A_BOLD);

            if (!(log->acq && log->blocked)) {
                wprintw(res_acq_window->text_window, "has ");

                wattron(res_acq_window->text_window, COLOR_PAIR(3) | A_NORMAL);
                wprintw(res_acq_window->text_window, "%s ", log->acq ? "acquired" : "released");
            } else {
                wprintw(res_acq_window->text_window, "has been ");

                wattron(res_acq_window->text_window, COLOR_PAIR(3) | A_NORMAL);
                wprintw(res_acq_window->text_window, "blocked", log);

                wattron(res_acq_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(res_acq_window->text_window, ", waiting for ");
            }

            wattron(res_acq_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(res_acq_window->text_window, "the semaphore ");

            wattron(res_acq_window->text_window, COLOR_PAIR(3) | A_NORMAL);
            wprintw(res_acq_window->text_window, "%s", log->sem_name);

            wattron(res_acq_window->text_window, COLOR_PAIR(1) | A_BOLD);
            wprintw(res_acq_window->text_window, ".\n");
        }

        pthread_mutex_unlock(&print_mutex);

        i = res_acq_log_list->tail;

        sem_wait(&res_acq_mutex);

        i = i->next;
    }
}

win_t* init_io_log() {
    const int main_height = LINES / 2;
    const int main_width = COLS / 2;

    const int text_offset = BOX_OFFSET + TITLE_OFFSET;
    WINDOW* main_window = newwin(main_height - 7, main_width, 7, 0);
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

    win_t* log_window = malloc(sizeof(win_t));
    (*log_window) = (win_t) {
        .main_window = main_window,
        .text_window = text_window,
        .title_window = title_window,
    };

    return log_window;
}

void refresh_io_title_window() {
    char *print_buffer = (char *)malloc(sizeof(char) * 64);
    const int print_size = sprintf(print_buffer, "Print: %d u.t.", io_general_log->p_time);

    wclear(io_window->title_window);
    wattron(io_window->title_window, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(io_window->title_window, 1, 1, "R: %ld bytes W: %ld bytes",
              io_general_log->r_bytes, io_general_log->w_bytes);

    const char title[] = "I/O View";

    wattron(io_window->title_window, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(io_window->title_window, 1,
              (COLS / 2) - print_size - 2, "%s", print_buffer);
    mvwprintw(io_window->title_window, 1,
              (COLS / 2 - (int)strlen(title)) / 2, title);
    wattron(io_window->title_window, COLOR_PAIR(3) | A_BOLD);

    free(print_buffer);
}

_Noreturn void* refresh_io_log() {
    sem_wait(&io_mutex);
    list_node_t* i = io_log_list->head;

    while (1) {
        pthread_mutex_lock(&print_mutex);
        for (; i != NULL; i = i->next) {
            io_log_info_t* io_info = ((io_log_info_t*)i->content);

            /* It indicates that the current log is a printing log */
            if (io_info->type == IO_LOG_PRINT) {
                io_log_print_t* io_log_print = io_info->print_log;

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, "Process ");

                wattron(io_window->text_window, COLOR_PAIR(3) | A_NORMAL);
                wprintw(io_window->text_window, "%s ", io_log_print->proc_name);

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, "is printing for ");

                wattron(io_window->text_window, COLOR_PAIR(3) | A_NORMAL);
                wprintw(io_window->text_window, "%d ", io_log_print->duration);

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, "u.t.\n");
            }
            /* It indicates that the current log is a file system */
            /* handling a disk read/write operation content */
            else if (io_info->type == IO_LOG_FILE_SYSTEM) {
                io_log_fs_t* io_log_fs = io_info->fs_log;

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, "Process ");

                wattron(io_window->text_window, COLOR_PAIR(3) | A_NORMAL);
                wprintw(io_window->text_window, "%s ", io_log_fs->proc_name);

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, "has ");

                wattron(io_window->text_window, COLOR_PAIR(3) | A_NORMAL);
                if ((io_log_fs->opt & IO_LOG_FS_READ) || (io_log_fs->opt & IO_LOG_FS_WRITE)) {
                    wprintw(io_window->text_window, "%s ",
                            ((io_log_fs->opt & IO_LOG_FS_READ) ? "read" :
                             (io_log_fs->opt & IO_LOG_FS_WRITE) ? "written" : "not specified"));
                } else {
                    wprintw(io_window->text_window, "%s ",
                            ((io_log_fs->opt & IO_LOG_FS_F_OPEN) ? "opened" :
                             (io_log_fs->opt & IO_LOG_FS_F_CLOSE) ? "closed" : "not specified"));
                }

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, "the file with inode ");

                wattron(io_window->text_window, COLOR_PAIR(3) | A_NORMAL);
                wprintw(io_window->text_window, "%d", io_log_fs->inumber);

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, ".\n");
            }
            /* It indicates that the current log is a disk request */
            else if (io_info->type == IO_LOG_DISK_REQUEST) {
                io_log_disk_req_t* io_log_disk_req = io_info->disk_req_log;

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, "Process ");

                wattron(io_window->text_window, COLOR_PAIR(3) | A_NORMAL);
                wprintw(io_window->text_window, "%s ", io_log_disk_req->proc_name);

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, "has requested a ");

                wattron(io_window->text_window, COLOR_PAIR(3) | A_NORMAL);
                wprintw(io_window->text_window, "%s", io_log_disk_req->read ? "read" : "write");

                wattron(io_window->text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(io_window->text_window, " operation.\n");
            }
            refresh_io_title_window();
        }
        pthread_mutex_unlock(&print_mutex);

        i = io_log_list->tail;

        sem_wait(&io_mutex);

        i = i->next;
    }
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
    }
}

/**
 * It returns the maximum segment size
 * allocated in the memory table.
 *
 * @return the maximum segment size
 *         allocated in the memory table
 */
static int max_seg_size() {
    int max = 0;

    FOREACH(kernel->seg_table.seg_list, segment_t*) {
        if (max < it->size)
            max = it->size;
    }

    return max;
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
    FOREACH(kernel->proc_table, process_t*) {
        if (it->seg_id == sid)
            return it;
    }

    return NULL;
}

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
static int sem_blocked_count() {
    const semaphore_table_t *sem_table = &kernel->sem_table;
    const semaphore_t *sem;
    int count = 0;

    for (sem = sem_table->table; sem < &sem_table->table[sem_table->len]; sem++)
        count += sem->waiters->size;

    return count;
}
