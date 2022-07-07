#include "terminal.h"

#define SI(S, V) {S, V},
int begin_terminal() {
    /* Initialize curses */
    init_screen(0);

    print_welcome();

    menu_choice_t choices[] = {MAIN_MENU(SI)};
    menu_t* main_menu
        = create_menu(ARRAY_SIZE(choices), choices, "Menu Principal");
    start_menu_and_loop(main_menu, (void (*)(int))NULL);
    wclear(main_menu->menu_window->main_win);
    wrefresh(main_menu->menu_window->main_win);
    delete_menu(main_menu);

    menu_choice_t choices_2[] = {SEC_MENU(SI)};
    menu_t* menu_2 = create_menu(ARRAY_SIZE(choices_2), choices_2,
                                 "Menu GRANDE PARA CARALHO Puta Que me pariu");
    start_menu_and_loop(menu_2, (void (*)(int))NULL);
    delete_menu(menu_2);

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

    set_menu_win(menu->curses_menu, menu->menu_window->main_win);
    set_menu_sub(menu->curses_menu, menu->menu_window->items_win);

    return menu;
}
menu_window_t* init_menu_window(const menu_t* menu) {
    WINDOW* mainWin
        = newwin(menu->height, menu->width, (LINES - menu->height) / 2,
                 (COLS - menu->width) / 2);

    const int titleHeight = 3;
    WINDOW* titleWin = derwin(mainWin, titleHeight, menu->width, 0, 0);

    /* Create Boxes around windows */
    box(titleWin, 0, 0);
    box(mainWin, 0, 0);

    int titleOffset = 3;
    int boxOffset = 2;
    WINDOW* choiceWin = derwin(mainWin, menu->height - titleOffset,
                               menu->width - boxOffset, titleOffset, boxOffset);

    const int titleMiddleY = 1;
    mvwaddstr(titleWin, titleMiddleY, (menu->width - strlen(menu->title)) / 2,
              menu->title);

    menu_window_t* newMenuWindow
        = (menu_window_t*)malloc(sizeof(menu_window_t));
    (*newMenuWindow) = (menu_window_t) {
        .main_win = mainWin,
        .title_win = titleWin,
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

void menu_loop(menu_t* main_menu, void (*func)(int)) {
    MENU* curses_menu = main_menu->curses_menu;
    WINDOW* main_window = main_menu->menu_window->main_win;
    int c;
    while ((c = getch())) {
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
            int type = get_type_from_string(main_menu, item_name(cur));

            if (type == EXIT) {
                return;
            }

            if (func) {
                return;
            }

            print_item_index(main_menu, cur);
            break;
        }
        default:
            break;
        }
        wrefresh(main_window);
    }
}

void start_menu_and_loop(menu_t* menu, void (*func)(int)) {
    MENU* curses_menu = menu->curses_menu;
    menu_window_t* menu_window = menu->menu_window;
    refresh();
    post_menu(curses_menu);
    wrefresh(menu_window->main_win);

    menu_loop(menu, func);
    unpost_menu(curses_menu);
}

void print_welcome() {
    mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
    mvprintw(LINES - 2, 0, "Up and Down arrow keys to navigate.");
}
int get_type_from_string(menu_t* menu, const char* buffer) {
    for (int i = 0; i < menu->choices_count; i++) {
        if (!strcmp(menu->choices[i].text, buffer)) {
            return menu->choices[i].value;
        }
    }
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