#include <stdlib.h>
#include "ui.h"
#include "notes.h"
#include "poller.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main() {
    if (!ui_init()) {
        return 1;
    }
    char **file_names=NULL;
    int count=0;
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
    for (;;) {
        int ch = ui_get_key();
        if (ch == 'q') break;

        // Placeholder: handle input
        ui_draw_input("Key pressed: ", ch);
    }

    ui_cleanup();
    return 0;
}
