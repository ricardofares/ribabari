#include "terminal.h"
#include "../kernel/kernel.h"
#include <stdio.h>
#include <curses.h>
#include <pthread.h>
#include <unistd.h>


list_t* log_list;

void log_list_init() {
    log_list = list_init();
}

void main_menu_functions(int x) {
    coordinates_t whatever = {
        .begin_x = 1,
        .begin_y = 1,
    };
    char* input;
    switch (x) {
    case CREATE:
        input = get_input_from_window("Qual o Input", whatever, 40);
        if (access(input, F_OK)) {
            print_with_window("The name of the file doesn't exist.", "WARNING.", 1, 1);
            return;
        }
        sysCall(PROCESS_CREATE, (void*)input);
        break;
    default:
        break;
    }
}

#define SI(S, V) {S, V},
int begin_terminal() {
    /* Initialize curses */
    init_screen();

    print_welcome();

    WINDOW* log_window = newwin(LINES / 2, COLS / 2, 0, COLS / 2 );
    box(log_window, 0, 0);
    refresh();
    wrefresh(log_window);
    pthread_t log_thread;
    pthread_attr_t log_thread_attr;

    pthread_attr_init(&log_thread_attr);
    pthread_attr_setscope(&log_thread_attr, PTHREAD_SCOPE_PROCESS);
    pthread_create(&log_thread, &log_thread_attr, make_process_log_window,
                   (void*)log_window);

    WINDOW* memory_window = newwin(LINES / 2, COLS / 2, LINES / 2, COLS / 2);
    box(memory_window, 0, 0);
    refresh();
    wrefresh(memory_window);
    pthread_t memory_thread;
    pthread_attr_t memory_thread_attr;
    pthread_attr_init(&memory_thread_attr);
    pthread_attr_setscope(&memory_thread_attr, PTHREAD_SCOPE_PROCESS);
    pthread_create(&memory_thread, &memory_thread_attr, make_memory_window,
                   (void*)memory_window);


    menu_choice_t choices[] = {MAIN_MENU(SI)};
    menu_t* main_menu
        = create_menu(ARRAY_SIZE(choices), choices, "Menu Principal");

    start_menu_and_loop(main_menu, (void (*)(int))NULL);
    unpost_menu(main_menu->curses_menu);
    wclear(main_menu->menu_window->main_win);
    wrefresh(main_menu->menu_window->main_win);
    delete_menu(main_menu);

    /* delwin(log_window); */
    endwin();

    return 0;
}

menu_t* create_menu(int choices_count,
                    menu_choice_t choices[static choices_count],
                    const char* title) {

    menu_t* menu = malloc(sizeof(menu_t));
    (*menu) = (menu_t) {
        .height = choices_count + 4,
        .width = get_biggest_item(choices, choices_count, title) + 5,
        .choices_count = choices_count,
        .choices = choices,
        .title = title,
    };

    menu->items = (ITEM**)calloc(menu->choices_count + 1, sizeof(ITEM*));

    for (int i = 0; i < menu->choices_count; i++) {
        menu->items[i] = new_item(menu->choices[i].text, "");
    }

    menu->curses_menu = new_menu(menu->items);
    menu->menu_window = init_menu_window(menu);
    set_menu_fore(menu->curses_menu, COLOR_PAIR(2) | A_BOLD);
    set_menu_back(menu->curses_menu, COLOR_PAIR(2));


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
    WINDOW* mainWin
        = newwin(menu->height, menu->width, (LINES - menu->height) / 2,
                 10);

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
    title_print(title_win, (menu->width - strlen(menu->title)) / 2, menu->title);

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
        case ENTER_KEY: {
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

        refresh();
        wrefresh(main_window);
        c = 0;
    }
}

void start_menu_and_loop(menu_t* menu, void (*func)(int)) {
    MENU* curses_menu = menu->curses_menu;
    menu_window_t* menu_window = menu->menu_window;
    refresh();
    wattrset(menu->menu_window->items_win, COLOR_PAIR(2));
    post_menu(curses_menu);
    wrefresh(menu_window->main_win);
    menu_loop(menu, main_menu_functions);
}

void print_welcome() {
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(LINES - 3, 1, "Press <ENTER> to see the option selected");
    mvprintw(LINES - 2, 1, "Up and Down arrow keys to navigate.");
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
    refresh();
    wrefresh(input_win->main_window);
    wrefresh(input_win->text_window);
    wrefresh(input_win->title_window);
    refresh();
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
    io_window_t io_win;
    echo();

    const int title_len = strlen(title);
    io_win.main_window = newwin(5, buffer_size + BOX_SIZE, coordinates.begin_y,
                                coordinates.begin_x);


    const int title_window_height = 3;
    io_win.title_window = derwin(io_win.main_window, title_window_height,
                                 buffer_size + BOX_SIZE, 0, 0);


    title_print(io_win.title_window, (buffer_size + BOX_SIZE - strlen(title)) / 2, title);

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

    int window_height = 5;
    io_win.main_window
        = newwin(window_height, window_width, y, x);

    int title_height = 3;
    io_win.title_window = derwin(io_win.main_window, 3, window_width, 0, 0);
    io_win.text_window = derwin(io_win.main_window, 1, window_width - 2, 3, 1);
    input_refresh(&io_win);

    wattrset(io_win.title_window, COLOR_PAIR(3));
    wattrset(io_win.main_window, COLOR_PAIR(3));
    wattrset(io_win.text_window, COLOR_PAIR(2));
    box(io_win.title_window, 0, 0);
    box(io_win.main_window, 0, 0);

    title_print(io_win.title_window, (window_width - title_len) / 2, title);

    mvwprintw(io_win.text_window, 0, (window_width - 2 - string_len) / 2, "%s",
              string);
    input_refresh(&io_win);
            getch();

    del_io_window(&io_win);
}

void* make_process_log_window(void* log_window) {
    WINDOW* main_window = (WINDOW*)log_window;
    WINDOW* text_window = derwin(main_window, LINES / 2 - 4, COLS / 2 - 3, 3, 1);
    WINDOW* title_window = derwin(main_window, 3, COLS / 2, 0, 0);
    scrollok(text_window, 1);
    wattrset(text_window, COLOR_PAIR(2));

    const char title[] = "Processos";
    title_print(title_window, ((COLS / 2) - strlen(title)) / 2, title);

    struct timespec start;
    struct timespec end;

    clock_gettime(CLOCK_REALTIME, &start);

    wattrset(main_window, COLOR_PAIR(3));
    wattrset(title_window, COLOR_PAIR(3));
    while (1) {
        clock_gettime(CLOCK_REALTIME, &end);
        const int elapsed = (end.tv_sec - start.tv_sec) * 1000000000L
                                + (end.tv_nsec - start.tv_nsec);

        if (elapsed >= 1000000000L) {
            start = end;

            wmove(text_window, 0, 0);
            for (list_node_t* i = log_list->head; i != NULL; i = i->next) {
                proc_log_info_t* log_info = ((proc_log_info_t*)i->content);
                if (!log_info->is_proc) {
                    wprintw(text_window, "No process running.\n");
                    continue;
                }

                wattron(text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(text_window, "PROCESS: ");
                wattroff(text_window, A_BOLD);

                wattron(text_window, COLOR_PAIR(3));
                wprintw(text_window, "%s, ", log_info->name);

                wattron(text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(text_window, "TIME  REMAINING: ");
                wattroff(text_window, A_BOLD);

                wattron(text_window, COLOR_PAIR(3));
                wprintw(text_window, "%d, ", log_info->remaining);

                wattron(text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(text_window, "PC: ");
                wattroff(text_window, A_BOLD);

                wattron(text_window, COLOR_PAIR(3));
                wprintw(text_window, "%d, ", log_info->pc);

                wattron(text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(text_window, "ID: ");
                wattroff(text_window, A_BOLD);

                wattron(text_window, COLOR_PAIR(3));
                wprintw(text_window, "%d.\n", log_info->id);


                refresh();
                box(title_window, 0, 0);
                box(main_window, 0, 0);
                wrefresh(main_window);
            }
        }
    }

    return NULL;
}

#define COLOR_BOLD  "\e[1m"
#define COLOR_OFF   "\e[m"
void *make_memory_window(void* memory_window) {
    WINDOW* main_window = (WINDOW*)memory_window;
    WINDOW* title_window = derwin(main_window, 3, COLS / 2, 0, 0);
    WINDOW* text_window = derwin(main_window, LINES / 2 - 4, COLS / 2 - 3, 3, 1);
    list_t* seg_list = kernel->seg_table.seg_list;

    char title[] = "Memória";
    title_print(title_window, ((COLS / 2) - strlen(title)) / 2, title);

    struct timespec start;
    struct timespec end;

    clock_gettime(CLOCK_REALTIME, &start);

    wattrset(main_window, COLOR_PAIR(3));
    wattrset(title_window, COLOR_PAIR(3));
    while (1) {
        clock_gettime(CLOCK_REALTIME, &end);
        const int elapsed = (end.tv_sec - start.tv_sec) * 1000000000L
                                + (end.tv_nsec - start.tv_nsec);

        if (elapsed >= 1000000000L) {
            start = end;

            wmove(text_window, 0, 0);
            wclear(text_window);

            for (list_node_t* i = seg_list->head; i != NULL; i = i->next) {
                segment_t* seg = ((segment_t*)i->content);
                char *buffer = malloc(100);


                wattron(text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(text_window, "ID: ");
                wattroff(text_window, A_BOLD);

                wattron(text_window, COLOR_PAIR(2));
                snprintf(buffer, 100, "%d | ", seg->id);
                wprintw(text_window, buffer);

                wattron(text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(text_window, "Segment Size: ");
                wattroff(text_window, A_BOLD);

                wattron(text_window, COLOR_PAIR(2));
                snprintf(buffer, 100, "%d bytes | ", seg->size);
                wprintw(text_window, buffer);

                wattron(text_window, COLOR_PAIR(1) | A_BOLD);
                wprintw(text_window, "Pages count: ");
                wattroff(text_window, A_BOLD);

                wattron(text_window, COLOR_PAIR(2));
                snprintf(buffer, 100, "%d avaiable, page %d in use -> \n", seg->page_qtd, seg->page_count);
                wprintw(text_window, buffer);

                wattroff(text_window, COLOR_PAIR(2));
            }
            refresh();
            box(title_window, 0, 0);
            box(main_window, 0, 0);
            wrefresh(main_window);
        }
    }

    return NULL;
}

