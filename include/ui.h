#ifndef UI_H
#define UI_H
#include <stddef.h>
#include <ncurses.h>
int ui_init();
void ui_draw_header(const char *title);
void ui_draw_input(const char *prefix, int ch);
int ui_get_key();
void ui_cleanup();
void ui_draw_note(const char *title, const char *content); // Display note content
void ui_edit_note(char *buffer, size_t bufsize);           // Simple line editor (raw mode)
void ui_list_notes(WINDOW*win_notes_names,char **filenames, int count, int selected,char*buffer);           // Display file list menu
void ui_del_input(char*buffer); // Clear input buffer
void ui_display_options(); // Display options menu
enum menu_options{ selected_editor,
       selected_viewer,
       selected_quit,
       selected_count }; // Menu options
#endif
