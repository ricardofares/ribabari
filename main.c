#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define ENTER_KEY 10

typedef struct menu_window_t {
    WINDOW *main_window;
    WINDOW *sub_window;
    int height;
    int width;
    int begin_y;
    int begin_x;
} menu_window_t;

char *choices[] = {
    "Criar Processo", 
    "Ver tabela de processos",
    "Sair"
};

void print_item_index(const ITEM *item);
void main_menu_loop(MENU *main_menu, WINDOW *window);
void init_screen();
void init_colors();
menu_window_t *init_menu_window_t(int height, int width, int begin_y,
                                  int begin_x);
void cfg_menu_window(MENU* menu, menu_window_t* menu_window);

int main() {
    ITEM **my_items;
    int c;

    MENU *my_menu;
    int n_choices, i;

    /* Initialize curses */
    init_screen(0);

    mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
    mvprintw(LINES - 2, 0, "Up and Down arrow keys to navigate.");

    ITEM **anothers_items = (ITEM **)calloc(4, sizeof(ITEM *));

    MENU *another_menu = new_menu(anothers_items);

    int win_height = 7;
    int win_width = 70;

    menu_window_t* menu_window = init_menu_window_t(win_height, win_width, (LINES - win_height) / 2,
            (COLS - win_width) / 2);

    cfg_menu_window(another_menu, menu_window);

    refresh();
    post_menu(another_menu);
    wrefresh(menu_window->main_window);

    main_menu_loop(another_menu, menu_window->main_window);

    unpost_menu(another_menu);
    delwin(menu_window->main_window);

    for (i = 0; i < n_choices; ++i)
        free_item(my_items[i]);
    free_menu(my_menu);
    endwin();
}

void init_screen() {
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}

void init_colors() {
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
}

void print_item_index(const ITEM *item) {
    move(20, 0);
    clrtoeol();
    mvprintw(20, 0, "%dth choice was chosen.", item_index(item));
}

void main_menu_loop(MENU *main_menu, WINDOW *window) {
    int c;
    while ((c = getch())) {
        switch (c) {
        case 'j':
        case KEY_DOWN:
            menu_driver(main_menu, REQ_DOWN_ITEM);
            break;
        case 'k':
        case KEY_UP:
            menu_driver(main_menu, REQ_UP_ITEM);
            break;
        case ENTER_KEY: {
            ITEM *cur = current_item(main_menu);

            if (!strcmp(item_name(cur), "Exit")) {
                return;
            }

            print_item_index(cur);
            // pos_menu_cursor(main_menu);
            break;
        }
        default:
            break;
        }
        wrefresh(window);
    }
}

menu_window_t *init_menu_window_t(int height, int width, int begin_y,
                                  int begin_x) {
    int box_offset = 2;
    WINDOW *new_window = newwin(height, width, begin_y, begin_x);
    WINDOW *sub_window =
        derwin(new_window, height - box_offset, width - box_offset, 2, 2);
    box(new_window, 0, 0);

    menu_window_t* new_menu_window = (menu_window_t*) malloc(sizeof(menu_window_t));
    new_menu_window->main_window = new_window;
    new_menu_window->sub_window = sub_window;
    new_menu_window->height = height;
    new_menu_window->width = width;
    new_menu_window->begin_y = begin_y;
    new_menu_window->begin_x = begin_x;

    return new_menu_window;
}

void cfg_menu_window(MENU* menu, menu_window_t* menu_window) {
    set_menu_win(menu, menu_window->main_window);
    set_menu_sub(menu, menu_window->sub_window);
}
