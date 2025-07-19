#include "ui.h"
#include "notes.h"
#include <ncurses.h>
#include <string.h>
#include <errno.h>
#define TEXT_BUFFER_SIZE 4096
enum menu_options options;
WINDOW * win_text;
WINDOW * win_notes_names;
WINDOW * win_options;
WINDOW * win_tools;
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
    win_tools=newwin( 20, 20, 10, 60);
    wbkgd(win_notes_names, COLOR_PAIR(2)); 
    wbkgd(win_text, COLOR_PAIR(1));       // Set background color of window
    wbkgd(win_options, COLOR_PAIR(2));       // Set background color of window
    
    wrefresh(win_tools);

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
void ui_list_notes(WINDOW *win_notes_names, char **filenames, int count, int selected,char*buffer) {
    werase(win_notes_names);
    wrefresh(win_text);
    box(win_notes_names, 0, 0);
    box(win_tools, 0, 0);                   // Optional border
    int x = 2;
    if(count == 0) {
        mvwprintw(win_notes_names, 1, x, "No notes available.");
        wrefresh(win_notes_names);
        return;
    }
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
    buffer=memset(buffer, 0, TEXT_BUFFER_SIZE);
    strncpy(buffer,notes_load(filenames[selected], "notes"), TEXT_BUFFER_SIZE - 1);
    (buffer)[TEXT_BUFFER_SIZE - 1] = '\0'; // Ensure null termination

    
    if(buffer[0] == '\0') {
        wclear(win_text);
        mvwprintw(win_text, 4, 4,"No content available for this note.");
    } else {
        mvwprintw(win_text, 2, 2,"Note %s:", filenames[selected]);
        mvwprintw(win_text, 4, 4,"%s", buffer);
    }
    wrefresh(win_notes_names);
    wrefresh(win_text);
    wrefresh(win_tools);
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
    wrefresh(win_text);
    wrefresh(win_notes_names);
    wrefresh(win_tools);
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
    box(win_notes_names, 0, 0);                   // Optional border
    mvwprintw(win_notes_names, 1, 1, "Notes List");
    move(10, 0);
    //clrtoeol();
    printw("Note: %s", title);
    move(20, 10);
    //clrtoeol();
    wrefresh(win_text);
    wrefresh(win_notes_names);
    wrefresh(win_tools);
}
void ui_new_note(char *buffer, size_t bufsize) {
    int pos = 0;
    int ch;
    int blink = 1;

    memset(buffer, 0, bufsize);

    nodelay(stdscr, TRUE);  // Non-blocking input
    curs_set(0);            // Hide cursor

    for(;;) {
        // Draw input prompt and text
        move(0, 0);
        clrtoeol();
        printw("Enter note: ");
        addnstr(buffer, pos);

        // Draw blinking cursor as underscore
        if (blink) {
            addch('_');
        } else {
            addch(' ');
        }

        refresh();

        blink = !blink;         // Toggle cursor
        usleep(40000);         // Blink delay (400 ms)

        ch = getch();           // Get key (non-blocking)
        if (ch == ERR) continue;
        if (ch == 27){ goto end; } // Escape to finish
        if (ch == '\n') break;  // Enter to finish
        else if (ch == KEY_BACKSPACE || ch == 127) {
            if (pos > 0) buffer[--pos] = '\0';
        } else if (pos < bufsize - 1 && ch >= 32 && ch <= 126) {
            buffer[pos++] = ch;
            buffer[pos] = '\0';
        }
    }
    notes_save(buffer,"", "notes"); // Save the new note
    end:
    nodelay(stdscr, FALSE); // Restore blocking input
    curs_set(1);            // Show real cursor again
    clear();             // Clear screen    
    move(0, 0);
    refresh();
}
void ui_del_input(char*buffer){
    *buffer = '\0'; // Clear the input buffer
    clrtobot();

    refresh();
}
void ui_display_options() {
    werase(win_options);
    box(win_options, 0, 0);
    mvwprintw(win_options, 1, 1, "F1: New Note   F2: Delete Note   F3: Save Note   F4: Quit");
    wrefresh(win_options);
}