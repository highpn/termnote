#ifndef UI_H
#define UI_H
#include <stddef.h>
int ui_init();
void ui_draw_header(const char *title);
void ui_draw_input(const char *prefix, int ch);
int ui_get_key();
void ui_cleanup();
void ui_draw_note(const char *title, const char *content); // Display note content
void ui_edit_note(char *buffer, size_t bufsize);           // Simple line editor (raw mode)
void ui_list_notes(char **filenames, int count);           // Display file list menu

#endif
