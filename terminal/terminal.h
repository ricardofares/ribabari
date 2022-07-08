#ifndef OS_PROJECT_TERMINAL_H
#define OS_PROJECT_TERMINAL_H
#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>

static const int EXIT = -1;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define ENTER_KEY 10

typedef enum main_menu_choices {
    CREATE,
    TABLE,
    DELETE,
    RESOURCES_WATCH,
    DISK_WATCH,
    STATISTICS,
} main_menu_choices;

typedef struct menu_window_t {
    WINDOW* main_win;
    WINDOW* title_win;
    WINDOW* items_win;
    int height;
    int width;
    int begin_y;
    int begin_x;
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

#define MAIN_MENU(M)                                                           \
    M("Criar processo", CREATE)                                                \
    M("Destruir Processo", DELETE)                                             \
    M("Ver tabela de processos", TABLE)                                        \
    M("Visualizacao de recursos", RESOURCES_WATCH)                             \
    M("Visualizacao de disco", DISK_WATCH)                                     \
    M("Estatistica de uso", STATISTICS)                                        \
    M("Sair", EXIT)

#define SEC_MENU(M)                                                            \
    M("UHUL", CREATE)                                                          \
    M("YEAH", DELETE)                                                          \
    M("FUNCIONA", EXIT)

void print_item_index(menu_t* menu, const ITEM* item);
void menu_loop(menu_t* main_menu, void (*func)(int));
void init_screen();
void start_menu_and_loop(menu_t* menu, void (*func)(int));
void print_welcome();
MENU* make_curses_menu(menu_t* menu);
menu_t* create_menu(int choices_count,
                    menu_choice_t choices[static choices_count],
                    const char* title);
const char* get_string_from_type(menu_t* menu, int type);
int get_type_from_string(menu_t* menu, const char* buffer);
int get_biggest_item(menu_choice_t choices[], int choice_count,
                     const char* title);

menu_window_t* init_menu_window(const menu_t* menu);
int begin_terminal();
void delete_menu(menu_t* menu);
void free_menu_window(menu_window_t* menu_window);

#endif // OS_PROJECT_TERMINAL_H

