#include "ui.h"
#include <ncurses.h>

int ui_init() {
    initscr();
    cbreak();
    noecho();
    noqiflush();
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
void ui_list_notes(char **filenames, int count){
    
 for(int i = 0; i < count; i++) {
        mvprintw(i + 1, 0, "%s |", filenames[i]);
    }
    refresh();
}

void ui_cleanup() {
    endwin();
}

void ui_edit_note(char*buffer, size_t bufsize) {
    move(3, 0);
    clrtoeol();
    printw("Edit note: ");
    getnstr(buffer, bufsize - 1);
    buffer[bufsize - 1] = '\0'; // Ensure null termination
    move(4, 0);
    printw("Note saved: %s", buffer);
    refresh();

}
ui_draw_note(const char *title, const char *content) {
    WINDOW * win=newwin( 100 , 100, 100 , 100);
    attrset(A_BOLD);
    move(10, 0);
    //clrtoeol();
    printw("Note: %s", title);
    move(20, 10);
    //clrtoeol();
    printw("%s", content);
    refresh();
}
void ui_del_input(char*buffer){
    *buffer = '\0'; // Clear the input buffer
    refresh();
}
