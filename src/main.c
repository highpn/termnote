#include "notes.h"
#include "poller.h"
#include "ui.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEXT_BUFFER_SIZE 4096                      // Size of the text buffer for notes
char                     buffer[TEXT_BUFFER_SIZE]; // Buffer to hold note content
extern enum menu_options options;                  // Current menu option
extern WINDOW           *win_notes_names;          // Window for displaying notes list
extern WINDOW           *win_text;                 // Window for displaying note content

char       **file_names;               // Array to hold note filenames
int          selected_index = 0;       // Current selected index in the notes list
int          tools_selected = 0;       // Current selected index in the tools menu
int          count          = 0;       // Number of notes
int          running        = 1;       // Flag to control the main loop
extern char *notes_dir_path;           // Path to the notes directory
void         handle_key_press(int ch); // Function to handle key presses
void         delete_selected_note();   // Function to delete the selected note
void         redraw_ui();              // Function to redraw the UI
int          refresh_notes_list();     // Function to refresh the list of notes
void         delete_selected_note();   // Function to delete the selected note
void handle_enter_key();
void add_note_to_list();

// Function to initialize the UI and load notes
int main()
{

    get_notes_dir();
    if (notes_dir_path == NULL)
    {
        fprintf(stderr, "Failed to get notes directory.\n");
        return EXIT_FAILURE;
    }
    if (!ui_init())
    {
        return EXIT_FAILURE;
    }
    refresh_notes_list();
    ui_list_notes(win_notes_names, file_names, count, selected_index, buffer);
    refresh();
    while (running)
    {
        redraw_ui();
        if (refresh_notes_list() == -1)
            break;

        int ch = ui_get_key();
        handle_key_press(ch);

        if (refresh_notes_list() == -1)
            break;
    }

    ui_cleanup();
    return 0;
}
// Function to handle key presses
void handle_key_press(int ch)
{
    char *tmp = buffer;

    switch (ch)
    {
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
        add_note_to_list();
        break;
    case KEY_F(2):
        delete_selected_note();
        break;
    case KEY_LEFT:
        if (selected_index > 0)
        {
            selected_index--;
        }
        break;
    case KEY_RIGHT:
        if (selected_index < count - 1)
        {
            selected_index++;
        }
        break;
    case KEY_UP:
        if (tools_selected > 0)
        {
            tools_selected--;
        }
        break;
    case KEY_DOWN:
        if (tools_selected < selected_count - 1)
        {
            tools_selected++;
        }
        break;
    case '\n':
        handle_enter_key();
    }
}

void redraw_ui()
{
    if (count == 0)
    {
        selected_index = 0;
        buffer[0]      = '\0';
    }
    else
    {
        ui_draw_note(file_names[selected_index], buffer);
    }
    ui_list_notes(win_notes_names, file_names, count, selected_index, buffer);
    ui_display_options();
    ui_list_tools(tools_selected);
    refresh();
}
int refresh_notes_list()
{
    return notes_list(&file_names, &count, notes_dir_path);
}

void delete_selected_note()
{
    if (count == 0)
    {
        beep(); // No notes to delete
        return; // No notes to delete
    }
    if (notes_delete(file_names[selected_index]) == -1)
    {
        printf("Error deleting note: %s\n", strerror(errno));
        return;
    }
    // Adjust selected index if necessary
    if (selected_index >= count - 1)
    {
        selected_index--; // Move up if at the end
    }
    count--; // Decrease count after deletion
}
void add_note_to_list()
{
    if (count >= 100)
        return; // Prevent overflow
    ui_new_note(buffer, TEXT_BUFFER_SIZE);

    // Adjust selected index if necessary
}
void handle_enter_key()
{

    if (tools_selected == selected_editor)
    {
        if (count == 0)
        {
            beep();
            return;
        }
        ui_edit_note(buffer, TEXT_BUFFER_SIZE);
        notes_delete(file_names[selected_index]); // Delete the old note
        if (notes_save(file_names[selected_index], buffer, notes_dir_path) == -1)
        {
            mvprintw(0, 0, "Error saving note: %s", strerror(errno));
        }
        else
        {
            mvprintw(0, 0, "Note saved successfully.");
        }
    }
    else if (tools_selected == selected_viewer)
    {
        if (count == 0)
        {
            beep();
            return;
        }
        ui_draw_note(file_names[selected_index], buffer);
    }
    else if (tools_selected == selected_quit)
    {
        running = 0;
    }
    else if (tools_selected == selected_save)
    {
        if (count == 0)
        {
            beep();
            return;
        }
        notes_save(file_names[selected_index], buffer, notes_dir_path);
    }
    else if (tools_selected == selected_new_note)
    {
        add_note_to_list();
    }
    else if (tools_selected == selected_delete_note)
    {
        delete_selected_note();
    }
    else if (tools_selected == selected_change_color){
        change_color();
    }
    else
    {
        mvprintw(0, 0, "Unknown tool selected.");
    }
}
