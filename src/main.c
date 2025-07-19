#include <stdlib.h>
#include "ui.h"
#include "notes.h"
#include "poller.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#define TEXT_BUFFER_SIZE 4096
char buffer[TEXT_BUFFER_SIZE];
extern enum menu_options options;
extern WINDOW*win_notes_names;
int main() {
    if (!ui_init()) {
        return 1;
    }
    char **file_names=NULL;
    int count=0;
    int selected_index = 0;
    
    if(notes_list(&file_names,&count,"notes")==-1)
        return -1;
    ui_draw_header("TermNote - Press 'q' to quit");
    if(notes_load_all("notes")==-1){
        printf("%s\n",strerror(errno));
        return EXIT_FAILURE;
    }
    if(notes_save("filename","text","notes")==-1){
        printf("%s\n",strerror(errno));
        return EXIT_FAILURE;
    }
    printf("%s",notes_load("filename","notes"));
    char*tmp=buffer;
    for (;;) {
        int ch = ui_get_key();
        if (ch == 'q') break;
        if (ch == '\t') {selected_index = (selected_index + 1) % count;}
        
        if (ch == ';') {options=selected_editor;}
        if (ch == '\\') {options=selected_viewer;}
        if (ch == 'c' ) {*tmp=' '; ui_del_input(--tmp); }
        if (ch == KEY_F(1)){ui_new_note(buffer, TEXT_BUFFER_SIZE);}
        if (ch == KEY_F(2)) {
            if(notes_delete(file_names[selected_index])==-1){printf("%s\n",strerror(errno));
            return EXIT_FAILURE;
        }
     }
        if(notes_list(&file_names,&count,"notes")==-1)
        return -1;
    
            if (tmp - buffer < TEXT_BUFFER_SIZE - 1) {
                *tmp++ = (char)ch;
            } else {
                // Buffer overflow, reset
                tmp = buffer;
                *tmp++ = (char)ch;
            }
        
        ui_draw_note(file_names[selected_index],buffer);
        ui_list_notes(win_notes_names,file_names, count,selected_index);
    
    }   
    ui_cleanup();
    return 0;
}
