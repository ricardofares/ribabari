#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define ENTER_KEY 10

typedef struct menu_window_t {
    WINDOW* main_window;
    WINDOW* title_window;
    WINDOW* items_window;
    int height;
    int width;
    int begin_y;
    int begin_x;
} menu_window_t;

#define EXIT "Sair"
enum mainMenuEnum {
    CREATE,
    TABLE,
    EXIT_,
};

#define MAIN_MENU(M)                                                           \
    M("Criar processo", CREATE)                                                \
    M("Ver tabela de processos", TABLE)                                        \
    M(EXIT, EXIT_)

#define SI(S, V) {S, V},
typedef struct menu_choice_t {
    const char* text;
    int value;
} menu_choice_t;

typedef struct menu_t {
    menu_choice_t* choices;
    const int choices_count;
    const char* title;
    int height;
    int width;
    MENU* curses_menu;
    menu_window_t* menu_window;
} menu_t;

int fn(const char* s) {
#define CMP(S, V)                                                              \
    if (strcmp(s, S) == 0)                                                     \
        return V;
    MAIN_MENU(CMP)
    exit(1);
}

#define MENU_ITEM_COUNT(a) (sizeof(a) / sizeof(menu_choice_t))

void print_item_index(const ITEM* item);
void main_menu_loop(MENU* main_menu, WINDOW* window);
void init_screen();
void init_colors();
menu_window_t* init_menu_window_t(int height, int width, int begin_y,
                                  int begin_x, const char* name);
void cfg_menu_window(MENU* menu, menu_window_t* menu_window);
void start_menu_and_loop(MENU* menu, menu_window_t* menu_window,
                         void (*function_loop)(MENU*, WINDOW*));
void print_welcome();
MENU* make_curses_menu(menu_t* menu);
void create_menu(menu_t* menu);

int main() {
    /* Initialize curses */
    init_screen(0);

    print_welcome();

    menu_t main_menu = {
        .title = "Menu Principal",
        .height = 9,
        .width = 70,
        .choices = (menu_choice_t[]) {MAIN_MENU(SI)},
        .choices_count = 3, // TODO generalize this, bit
    };
    create_menu(&main_menu);

    // ITEM **anothers_items = (ITEM **)calloc(4, sizeof(ITEM *));
    // delwin(main_menu_window->main_window);

    // for (int i = 0; i < ARRAY_SIZE(main_choices); ++i)
    //     free_item(anothers_items[i]);
    // free_menu(main_menu);
    endwin();
}

MENU* make_curses_menu(menu_t* menu) {
    ITEM** anothers_items
        = (ITEM**)calloc(menu->choices_count + 1, sizeof(ITEM*));

    // DANGER ZONE
#define IT(S, V) anothers_items[V] = new_item(S, ""); /* exec_##V (); */
    MAIN_MENU(IT)

    return new_menu(anothers_items);
}

void create_menu(menu_t* menu) {
    int win_height = menu->height;
    int win_width = menu->width;
    menu_window_t* main_menu_window
        = init_menu_window_t(win_height, win_width, (LINES - win_height) / 2,
                             (COLS - win_width) / 2, menu->title);

    MENU* curses_menu = make_curses_menu(menu);

    cfg_menu_window(curses_menu, main_menu_window);
    start_menu_and_loop(curses_menu, main_menu_window, main_menu_loop);
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

void print_item_index(const ITEM* item) {
    move(20, 0);
    clrtoeol();
    mvprintw(20, 0, "%dth choice was chosen.", item_index(item));
}

void main_menu_loop(MENU* main_menu, WINDOW* window) {
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
            ITEM* cur = current_item(main_menu);

            if (!strcmp(item_name(cur), EXIT)) {
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

menu_window_t* init_menu_window_t(int height, int width, int begin_y,
                                  int begin_x, const char* name) {

    WINDOW* new_window = newwin(height, width, begin_y, begin_x);

    const int title_height = 3;
    WINDOW* title_window = derwin(new_window, title_height, width, 0, 0);

    /* Create Boxes around windows */
    box(title_window, 0, 0);
    box(new_window, 0, 0);

    int title_offset = 4;
    int box_offset = 2;
    WINDOW* sub_window = derwin(new_window, height - title_offset,
                                width - box_offset, title_offset, box_offset);

    const int title_middle_y = 1;
    mvwaddstr(title_window, title_middle_y,
              (width - sizeof(name) - box_offset) / 2, name);

    menu_window_t* new_menu_window
        = (menu_window_t*)malloc(sizeof(menu_window_t));
    (*new_menu_window) = (menu_window_t) {
        .main_window = new_window,
        .title_window = title_window,
        .items_window = sub_window,
        .height = height,
        .width = width,
        .begin_y = begin_y,
        .begin_x = begin_x,
    };

    return new_menu_window;
}

void cfg_menu_window(MENU* menu, menu_window_t* menu_window) {
    set_menu_win(menu, menu_window->main_window);
    set_menu_sub(menu, menu_window->items_window);
}

void start_menu_and_loop(MENU* menu, menu_window_t* menu_window,
                         void (*function_loop)(MENU*, WINDOW*)) {
    refresh();
    post_menu(menu);
    wrefresh(menu_window->main_window);

    (*function_loop)(menu, menu_window->main_window);
    unpost_menu(menu);
}

void print_welcome() {
    mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
    mvprintw(LINES - 2, 0, "Up and Down arrow keys to navigate.");
}
