#include <stdlib.h>
#include "ui.h"
#include "notes.h"
#include "poller.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#define TEXT_BUFFER_SIZE 4096 // Size of the text buffer for notes
char buffer[TEXT_BUFFER_SIZE]; // Buffer to hold note content
extern enum menu_options options; // Current menu option
extern WINDOW*win_notes_names; // Window for displaying notes list

char **file_names=NULL;// Array to hold note filenames
int selected_index = 0; // Current selected index in the notes list
int count = 0;  // Number of notes
int running = 1; // Flag to control the main loop
void handle_key_press(int ch); // Function to handle key presses
void delete_selected_note(); // Function to delete the selected note
void redraw_ui();   // Function to redraw the UI    
int refresh_notes_list();   // Function to refresh the list of notes
void delete_selected_note(); // Function to delete the selected note
// Function to initialize the UI and load notes
int main() {
    if (!ui_init()) {
        return EXIT_FAILURE;
    }
    
    while (running) {
        if (refresh_notes_list() == -1)
            break;

        int ch = ui_get_key();
        handle_key_press(ch);

        if (refresh_notes_list() == -1)
            break;

        redraw_ui();
    }

    ui_cleanup();
    return 0;
}
// Function to handle key presses
void handle_key_press(int ch) {
        char*tmp=buffer;

    switch (ch) {
        case 'q':
            running = 0;
            break;
        case '\t':
            if (count > 0)
                selected_index = (selected_index + 1) % count;
            break;
        case ';':
            options = selected_editor;
            break;
        case '\\':
            options = selected_viewer;
            break;
        case 'c':
            *tmp = ' ';
            ui_del_input(--tmp);
            break;
        case KEY_F(1):
            ui_new_note(buffer, TEXT_BUFFER_SIZE);
            break;
        case KEY_F(2):
            delete_selected_note();
            break;
    }
}

void redraw_ui() {
    if (count == 0) {
        selected_index = 0;
        buffer[0] = '\0';
    } else {
        ui_draw_note(file_names[selected_index], buffer);
    }
    ui_list_notes(win_notes_names, file_names, count, selected_index, buffer);
}
int refresh_notes_list() {
    return notes_list(&file_names, &count, "notes");
}

void delete_selected_note() {
    if (count == 0) return; // No notes to delete
    if (notes_delete(file_names[selected_index]) == -1) {
        printf("Error deleting note: %s\n", strerror(errno));
        return;
    }
    // Adjust selected index if necessary
    if (selected_index >= count - 1) {
        selected_index--; // Move up if at the end
    }
    count--; // Decrease count after deletion
}
