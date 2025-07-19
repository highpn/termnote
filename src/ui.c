#include "ui.h"
#include "notes.h"
#include <ncurses.h>
#include <string.h>
#include <errno.h>
enum menu_options options;
WINDOW * win_text;
WINDOW * win_notes_names;
WINDOW * win_options;
int ui_init() {
    initscr();
    cbreak();
    noecho();
    noqiflush();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();       // Enable color functionality
    init_pair(1, COLOR_RED, COLOR_BLACK);  // Pair #1: Red text on black backgro
    init_pair(2, COLOR_BLUE, COLOR_GREEN);
    win_text=newwin( 20, 50, 10, 10);
    win_notes_names=newwin( 5, 100, 0, 10);
    win_options=newwin( 5, 100, 30, 10);
    wbkgd(win_notes_names, COLOR_PAIR(2)); 
    wbkgd(win_text, COLOR_PAIR(1));       // Set background color of window
    wbkgd(win_options, COLOR_PAIR(1));       // Set background color of window

    mvwprintw(win_text, 1, 1, "Colored window!");
    wrefresh(win_text);
    wrefresh(win_notes_names);
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
void ui_list_notes(WINDOW *win_notes_names, char **filenames, int count, int selected) {
     werase(win_notes_names);
    box(win_notes_names, 0, 0);
    int x = 2;
    for (int i = 0; i < count; i++) {
        if (i == selected) {
            wattron(win_notes_names, A_REVERSE);
            mvwprintw(win_notes_names, 1, x, "%s", filenames[i]);
            wattroff(win_notes_names, A_REVERSE);
        } else {
            mvwprintw(win_notes_names, 1, x, "%s", filenames[i]);
        }
        x += strlen(filenames[i]) + 3;  // Adjust spacing
    }
    wrefresh(win_notes_names);
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
    mvwprintw(win_text, 4, 0,"Note saved: %s", buffer);
    wrefresh(win_text);
    wrefresh(win_notes_names);
    refresh();

}
ui_draw_note(const char *title, const char *content) {
    if(options == selected_editor) {
        wbkgd(win_text, COLOR_PAIR(1));
        wbkgd(win_notes_names, COLOR_PAIR(2));
        wbkgd(win_options, COLOR_PAIR(1)); 
 
    } else {
        wbkgd(win_text, COLOR_PAIR(2));
        wbkgd(win_notes_names, COLOR_PAIR(1)); 
        wbkgd(win_options, COLOR_PAIR(1)); 

    }
    box(win_text, 0, 0);                          // Optional border
    box(win_notes_names, 0, 0);                          // Optional border

    box(win_options, 0, 0);                          // Optional border
    mvwprintw(win_text, 4, 0, "Colored window!");
    mvwprintw(win_notes_names, 1, 1, "Notes List");
    mvwprintw(win_options, 1, 1, "F1: New   F2: Edit   F3: Delete   F4: Save  F5: Quit F6:");
    move(10, 0);
    //clrtoeol();
    printw("Note: %s", title);
    move(20, 10);
    //clrtoeol();
    mvwprintw(win_text, 4, 0,"%s",content);
    wrefresh(win_text);
    wrefresh(win_notes_names);
    wrefresh(win_options);
}
void ui_new_note(char *buffer, size_t bufsize) {
    move(3, 0);
    clrtoeol();
    printw("New note: ");
    
    getnstr(buffer, bufsize - 1);
    buffer[bufsize - 1] = '\0'; // Ensure null termination
    move(4, 0);
    mvwprintw(win_notes_names, 4, 0,"Note created: %s", buffer);
    if(notes_save(buffer,"", "notes")==-1){
        mvwprintw(win_notes_names, 5, 0,"Error saving note: %s", strerror(errno));
    } else {
        mvwprintw(win_notes_names, 5, 0,"Note saved successfully.");
    }
    wrefresh(win_notes_names);
}
void ui_del_input(char*buffer){
    *buffer = '\0'; // Clear the input buffer
    clrtobot();

    refresh();
}
