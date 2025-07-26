#include "ui.h"
#include "notes.h"
#include <errno.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>
#include <math.h>
#define TEXT_BUFFER_SIZE 4096

#define COLORS_PER_ROW 16
#define COLOR_BOX_WIDTH 4
#define COLOR_BOX_HEIGHT 1
char             *tools_[] = {"View Note", "Edit Note", "Quit", "save", "new note","delete note" ,"change color"    }; 
enum menu_options options;
WINDOW           *win_text;
WINDOW           *win_notes_names;
WINDOW           *win_options;
WINDOW           *win_tools;
WINDOW           *win_graph;

WINDOW *win_option_params[7];
enum option_params params;
PANEL *panel_text;
extern char      *notes_dir_path; // Path to the notes directory
void ui_draw_note(const char *title, const char *content);
void show_popup(const char*);
MEVENT event;
int * graph_x2(int);
int ui_init()
{
    initscr();
    cbreak();
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL); // Enable mouse events
    noecho();
    noqiflush();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();                        // Enable color functionality
    init_pair(1, COLOR_BLUE, COLOR_RED); // Pair #1: Red text on black backgro
    init_pair(2, COLOR_BLUE, COLOR_RED);
    /*
    show_popup(  "Welcome to TermNote\n\n"
    "A lightweight terminal-based note-taking application.\n"
    "Create, view, and manage plain text notes quickly\n"
    "without leaving the terminal.\n\n"
    "Shortcuts:\n"
    "Use arrow keys and Enter to navigate the interface.");
    */
    init_pair(10,COLOR_WHITE,show_color_popup());
    win_graph = newwin(20, 50, 5, 75);
    //win_text        = newwin(20, 50, 10, 10);
    //win_notes_names = newwin(5, 100, 5, 10);
   // win_options     = newwin(5, 100, 30, 10);
    //win_tools       = newwin(20, 20, 10, 60);
    //wbkgd(win_notes_names, COLOR_PAIR(2));
    //wbkgd(win_text, COLOR_PAIR(10));    // Set background color of window
    //wbkgd(win_options, COLOR_PAIR(2)); // Set background color of window
    //wbkgd(win_tools, COLOR_PAIR(2));   // Set background color of window
    box(win_graph,0,0);
     
    ui_init_win_options();
    int returns[] = {1,2,3,4,5, 6, 7, 8,9,10,5,4,3,2,1};
    int count = sizeof(returns)/sizeof(returns[0]);
    int *y=graph_x2(50);
    draw_graph(y,50);
    wrefresh(win_graph);    
    //wrefresh(win_options);
    //wrefresh(win_tools);
    return 1;
}

void ui_draw_header(const char *title)
{
    mvprintw(0, 0, "%s", title);
    refresh();
}

void ui_draw_input(const char *prefix, int ch)
{
   // move(0, 0);
    clrtoeol();
    // printw("%s%c", prefix, ch);

    refresh();
}

int ui_get_key()
{
    return getch();
}
void ui_list_notes(WINDOW *win_notes_names, char **filenames, int count, int selected, char *buffer)
{
    werase(win_notes_names);
    werase(win_text);
    // wrefresh(win_text);
    box(win_notes_names, 0, 0);
    box(win_tools, 0, 0); // Optional border
    int x = 2;
    if (count == 0)
    {
        mvwprintw(win_notes_names, 1, x, "No notes available.");
        mvwprintw(win_text, 1, x, "No notes available.");
        wrefresh(win_notes_names);
        //wrefresh(win_text);
        return;
    }
    for (int i = 0; i < count; i++)
    {
        if (i == selected)
        {
            wattron(win_notes_names, A_REVERSE);
            mvwprintw(win_notes_names, 1, x, "%s", filenames[i]);
            wattroff(win_notes_names, A_REVERSE);
        }
        else
        {
            mvwprintw(win_notes_names, 1, x, "%s", filenames[i]);
        }
        x += strlen(filenames[i]) + 3; // Adjust spacing
    }
    buffer = memset(buffer, 0, TEXT_BUFFER_SIZE);
    strncpy(buffer, notes_load(filenames[selected], notes_dir_path), TEXT_BUFFER_SIZE - 1);
    (buffer)[TEXT_BUFFER_SIZE - 1] = '\0'; // Ensure null termination

    if (buffer[0] == '\0')
    {
        wclear(win_text);
        mvwprintw(win_text, 4, 4, "No content available for this note.");
    }
    else
    {
        wclear(win_text);
        mvwprintw(win_text, 4, 4, "%s", buffer);
    }
    mvwprintw(win_text, 2, 2, "Note %s:", filenames[selected]);
    wrefresh(win_notes_names);
    //wrefresh(win_text);
    wrefresh(win_tools);
}
void ui_list_tools(int selected)
{
    werase(win_tools);
    box(win_tools, 0, 0);
    int y = 1;
    mvwprintw(win_tools, 1, 1, "Tools");
    for (int i = 0; i < sizeof(tools_) / sizeof(tools_[0]); i++)
    {
        if (i == selected)
        {
            wattron(win_tools, A_REVERSE);
            mvwprintw(win_tools, y, 1, "%s", tools_[i]);
            wattroff(win_tools, A_REVERSE);
        }
        else
        {
            mvwprintw(win_tools, y, 1, "%s", tools_[i]);
        }
        y++;
    }
    wrefresh(win_tools);
}

void ui_cleanup()
{
    endwin();
}

void ui_edit_note(char *buffer, size_t bufsize)
{
    //move(0, 0);
    clrtoeol();
    printw("Edit note: ");
    memset(buffer, ' ', bufsize);
    echo();
    getnstr(buffer, bufsize - 1);
    noecho();
    clear();
    wclear(win_text);
    buffer[bufsize - 1] = '\0'; // Ensure null termination
    move(0, 0);
    wprintw(win_text, "%s", buffer);
    //wrefresh(win_text);
    wrefresh(win_notes_names);
    wrefresh(win_tools);
    refresh();
    move(0, 0);
}
void ui_draw_note(const char *title, const char *content)
{
    if (options == selected_editor)
    {
        wbkgd(win_text, COLOR_PAIR(10));
        wbkgd(win_notes_names, COLOR_PAIR(2));
    }
    else
    {
        wbkgd(win_text, COLOR_PAIR(10));
        wbkgd(win_notes_names, COLOR_PAIR(2));
    }
    // box(win_text, 0, 0);        // Optional border
    box(win_notes_names, 0, 0); // Optional border
    // mvwprintw(win_notes_names, 1, 1, "Notes List");
    //move(0, 0);
    clrtoeol();
    //printw("Note: %s", title);
    //move(0, 0);
    // clrtoeol();
    //(win_options);
    // wrefresh(win_text);
    wrefresh(win_notes_names);
    wrefresh(win_tools);
    wrefresh(win_options);
}
void ui_new_note(char *buffer, size_t bufsize)
{
    int pos = 0;
    int ch;
    int blink = 1;

    memset(buffer, 0, bufsize);

    nodelay(stdscr, TRUE); // Non-blocking input
    curs_set(0);           // Hide cursor

    for (;;)
    {
        // Draw input prompt and text
        move(0, 0);
        clrtoeol();
        printw("Enter note: ");
        addnstr(buffer, pos);

        // Draw blinking cursor as underscore
        if (blink)
        {
            addch('_');
        }
        else
        {
            addch(' ');
        }

        refresh();

        blink = !blink; // Toggle cursor
        usleep(40000);  // Blink delay (400 ms)

        ch = getch(); // Get key (non-blocking)
        if (ch == ERR)
            continue;
        if (ch == 27)
        {
            goto end;
        } // Escape to finish
        if (ch == '\n')
            break; // Enter to finish
        else if (ch == KEY_BACKSPACE || ch == 127)
        {
            if (pos > 0)
                buffer[--pos] = '\0';
        }
        else if (pos < bufsize - 1 && ch >= 32 && ch <= 126)
        {
            buffer[pos++] = ch;
            buffer[pos]   = '\0';
        }
    }
    notes_save(buffer, "", notes_dir_path); // Save the new note
end:
    nodelay(stdscr, FALSE); // Restore blocking input
    curs_set(0);            // Show real cursor again
    clear();                // Clear screen
    move(0, 0);
    refresh();
}
void ui_del_input(char *buffer)
{
    *buffer = '\0'; // Clear the input buffer
    clrtobot();

    refresh();
}
void ui_display_options()
{   
    box(win_options, 0, 0);
    mvwprintw(win_options, 1, 1, "F1: New Note   F2: Delete Note   F3: Save Note   F4: Quit");
    wrefresh(win_options);
}

void show_popup(const char* message) {
    int height = 40, width = 40;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *popup_win = newwin(height, width, starty, startx);
    PANEL *popup_panel = new_panel(popup_win); // Attach to panel system

    box(popup_win, 0, 0);
    mvwprintw(popup_win, 2, 2, message);
    mvwprintw(popup_win, 4, 2, "Press any key to close...");
    update_panels();
    doupdate();

    getch(); // Wait for input

    // Clean up
    del_panel(popup_panel);
    delwin(popup_win);
    update_panels();
    doupdate();
}
int show_color_popup() {
    int win_height = 20;
    int win_width = COLORS_PER_ROW * COLOR_BOX_WIDTH + 4;
    int starty = (LINES - win_height) / 2;
    int startx = (COLS - win_width) / 2;

    WINDOW *popup = newwin(win_height, win_width, starty, startx);
    PANEL *panel = new_panel(popup);
    keypad(popup, TRUE);
    box(popup, 0, 0);
    curs_set(0);
    noecho();

    // Initialize color pairs
    start_color();
    use_default_colors();
    for (int i = 0; i < 256; i++) {
        init_pair(i + 1, COLOR_BLACK, i);
    }

    int selected = 0;

    for(;;){
        // Draw color grid
        for (int i = 0; i < 256; i++) {
            int row = i / COLORS_PER_ROW;
            int col = i % COLORS_PER_ROW;
            int y = 2 + row * COLOR_BOX_HEIGHT;
            int x = 2 + col * COLOR_BOX_WIDTH;

            if (i == selected) {
                wattron(popup, A_REVERSE);
            }
            wattron(popup, COLOR_PAIR(i + 1));
            mvwprintw(popup, y, x, " %3d", i);
            wattroff(popup, COLOR_PAIR(i + 1));
            if (i == selected) {
                wattroff(popup, A_REVERSE);
            }
        }

        // Preview selected color
        mvwprintw(popup, win_height - 2, 2, "Selected color: %3d ", selected);
        init_pair(10,COLOR_WHITE,selected);
        wbkgdset(win_text,10);
        //wrefresh(win_text); 
        wattron(popup, COLOR_PAIR(selected + 1));
        mvwprintw(popup, win_height - 2, 25, "     ");
        wattroff(popup, COLOR_PAIR(selected + 1));

        update_panels();
        doupdate();

        int ch = wgetch(popup);
        if (ch == '\n') break;
        else if (ch == KEY_LEFT && selected % COLORS_PER_ROW > 0)
            selected--;
        else if (ch == KEY_RIGHT && selected % COLORS_PER_ROW < COLORS_PER_ROW - 1)
            selected++;
        else if (ch == KEY_UP && selected >= COLORS_PER_ROW)
            selected -= COLORS_PER_ROW;
        else if (ch == KEY_DOWN && selected < 256 - COLORS_PER_ROW)
            selected += COLORS_PER_ROW;
    }

    del_panel(panel);
    delwin(popup);
    update_panels();
    doupdate();
    return selected;
}
change_color(){
    init_pair(10, COLOR_WHITE,show_color_popup());
}

void ui_init_win_options(){
    for(int i=0;i<option_param_count;i++){
        win_option_params[i]=newwin(5,40,2+i*5,10);
        
        //box(win_option_params[i],0,0);
        wbkgd(win_option_params[i],COLOR_PAIR(2));
        box(win_option_params[i],0,0);
        ui_init_win_options_titles(win_option_params[i],i);
        wrefresh(win_option_params[i]);
    }
}
void ui_init_win_options_titles(WINDOW*win,int n){
    switch(n){
        case option_type :
            mvwprintw(win,1,1,"option type");
            break;
        case strike_price:
            mvwprintw(win,1,1,"strike price");
            break;
        case stock_price :
            mvwprintw(win,1,1,"stock price");
            break;
        case time_to_expiry :
            mvwprintw(win,1,1,"time to expiry");
            break;
        case volatility :
            mvwprintw(win,1,1,"volatility");
            break;
        case interest_rate:
            mvwprintw(win,1,1,"interest rate");
            break;
        case premium:
            mvwprintw(win,1,1,"premium");
            break;
    }
}

void ui_list_option_params(int selected)
{   
      
    char buff[10];
    int y = 1;
    mvwprintw(win_tools, 1, 1, "Tools");
    for (int i = 0; i < option_param_count; i++)
    {   werase(win_option_params[i]);
        
        if (i == selected)
        {   box(win_option_params[i],0,0);
            wattr_on(win_option_params[i],A_UNDERLINE | A_REVERSE ,0);
            curs_set(1);
            echo();
            
            move(i*5+5,20);
            int x,y;
            wmouse_trafo(win_option_params[i],&x,&y,true);
            printw("%d %d",x,y);
        }
        else
        {
            wattr_off(win_option_params[i],A_UNDERLINE | A_REVERSE ,0);
        }
        
        ui_init_win_options_titles(win_option_params[i],i);
        wrefresh(win_option_params[i]);
    }
}



void draw_graph(int *data, int n) {
       int max_y, max_x;
    getmaxyx(win_graph, max_y, max_x);

    float max_val = data[0], min_val = data[0];
    for (int i = 1; i < n; i++) {
        if (data[i] > max_val) max_val = data[i];
        if (data[i] < min_val) min_val = data[i];
    }

    float range = max_val - min_val;
    if (range == 0) range = 1;

    int prev_y = 0;
    for (int i = 0; i < n && i < max_x; i++) {
        int y = max_y - 1 - round((data[i] - min_val) * (max_y - 1) / range);

        if (i > 0) {
            int dy = y - prev_y;
            int dx = 1;

            for (int j = 0; j <= abs(dy); j++) {
                int interp_y = prev_y + (dy > 0 ? j : -j);
                mvwaddch(win_graph,interp_y, i - 1 + j * dx / abs(dy), dy > 0 ? '╱' : '╲');
            }
        }

        mvwaddch(win_graph,y, i, '●'); // data point
        prev_y = y;
    }

    refresh();
}


int * graph_x2(int n){
    int *x2=(int*)malloc(sizeof(int)*(n+1));
    for(int i=0;i<n+1;i++){
        x2[i]=pow(-n/2+i-40,2);
    }
    return x2;
}