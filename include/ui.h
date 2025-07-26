#ifndef UI_H
#define UI_H
#include <ncurses.h>
#include <stddef.h>
int  ui_init();
void ui_draw_header(const char *title);
void ui_draw_input(const char *prefix, int ch);
int  ui_get_key();
void ui_cleanup();
void ui_edit_note(char *buffer, size_t bufsize);           // Simple line editor (raw mode)
void ui_list_notes(WINDOW *win_notes_names, char **filenames, int count, int selected,
                   char *buffer);               // Display file list menu
void ui_del_input(char *buffer);                // Clear input buffer
void ui_display_options();                      // Display options menu
void ui_list_tools(int selected);               // Display tools menu
void ui_new_note(char *buffer, size_t bufsize); // Create a new note
void ui_init_win_options(void);
void ui_init_win_options_titles(WINDOW *win,int n);
enum option_params{option_type,strike_price,stock_price,time_to_expiry,volatility,interest_rate,premium,option_param_count};

enum menu_options
{
    selected_viewer,
    selected_editor,
    selected_quit,
    selected_save,
    selected_new_note,
    selected_delete_note,
    selected_change_color,
    selected_count
}; // Menu options
#endif
