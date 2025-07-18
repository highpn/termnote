#include "ui.h"
#include <ncurses.h>

int ui_init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    return 1;
}

void ui_draw_header(const char *title) {
    mvprintw(0, 0, "%s", title);
    refresh();
}

void ui_draw_input(const char *prefix, int ch) {
    move(2, 0);
    clrtoeol();
    printw("%s%c", prefix, ch);
    refresh();
}

int ui_get_key() {
    return getch();
}

void ui_cleanup() {
    endwin();
}

