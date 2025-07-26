#include "notes.h"
#include "poller.h"
#include "ui.h"
#include "option.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/financial_data.h"
#include <cjson/cJSON.h>
bool pl=false;
#define TEXT_BUFFER_SIZE 4096                      // Size of the text buffer for notes
char                     buffer[TEXT_BUFFER_SIZE]; // Buffer to hold note content
extern enum menu_options options;                  // Current menu option
extern WINDOW           *win_notes_names;          // Window for displaying notes list
extern WINDOW           *win_text;                 // Window for displaying note content
WINDOW *matrix;
extern MEVENT event;
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
char map_price_to_char(double p, double max);

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
#define URL_TEMPLATE "https://query1.finance.yahoo.com/v7/finance/download/%s?period1=%ld&period2=%ld&interval=1d&events=history"
// Function to initialize the UI and load notes
int main()
{
    initscr();
    start_color();
    use_default_colors();
    noecho();
    curs_set(FALSE);

    init_pair(1, COLOR_WHITE, -1);
    init_pair(2, COLOR_CYAN, -1);
    init_pair(3, COLOR_GREEN, -1);
    init_pair(4, COLOR_YELLOW, -1);
    init_pair(5, COLOR_MAGENTA, -1);
    init_pair(6, COLOR_RED, -1);
    init_pair(7, COLOR_BLUE, -1);
    init_pair(8, COLOR_BLUE, COLOR_BLACK);

    int stock_start = 2, stock_end = 15;
    int n_prices = stock_end - stock_start;
    int n_days = 200;
    int strike_price=3;
    double prices[n_days][n_prices];
    double max_price = 0.0;

    for (int i = 0; i < n_days; i++) {
        double T = (double)(i + 1) / 356.0;
        for (int j = 0; j < n_prices; j++) {
            double S = stock_start + j*0.5;
            double p = binomial_american_option(S,strike_price, T, 0.05,2.5, 100, "call");     
            prices[i][j] = i==0&&S<=strike_price ? 0.0 : p;
            if (p > max_price) max_price = p;
        }
    }

    int cell_w = 6;
    int win_w = (n_prices + 1) * cell_w + 2;
    int win_h = n_days ;

    matrix = newwin(win_h, win_w, 2, 2);
    WINDOW *matrix_pad = newpad(400, win_w); // Enough rows (200) to scroll
    box(matrix_pad, 0, 0);
    //mvwprintw(matrix_pad, 0, cell_w, "Stock Prices:");
    /*
    for (int j = 0; j < n_prices; j++) {
        mvwprintw(matrix_pad, 1, (j + 1) * cell_w, "%5d", stock_start + j);
    }
    */
    for (int i = 0; i < n_days; i++) {
        mvwprintw(matrix_pad, i + 2, 1, "D%d   ", (n_days-i ));
        for (int j = 0; j < n_prices; j++) {
            double p = prices[i][j];
            double ratio = p / max_price;
            int color = (ratio > 0.85) ? 6 :
                        (ratio > 0.65) ? 5 :
                        (ratio > 0.45) ? 4 :
                        (ratio > 0.25) ? 3 :
                        (ratio > 0.10) ? 2 : 1;

            wattron(matrix_pad, COLOR_PAIR(color));
            mvwprintw(matrix_pad, i + 2, (j + 1) * cell_w, "%5.2f", p-0.43);
            wattroff(matrix_pad, COLOR_PAIR(color));
        }
    }

    
    mvprintw(0, 2, "American Call Option Prices Matrix (Strike = $50)");
    mvprintw(win_h + 3, 2, "Press any key to exit...");
    refresh();
    wrefresh(matrix);
    int pad_row = 0;
int visible_rows = LINES - 6; // how many rows of the pad to show
int visible_cols = win_w;     // full width of matrix
int ch;
keypad(stdscr, TRUE);

int pad_rows = n_days+3;
int pad_cols = (n_prices + 1) * cell_w;

WINDOW *header_win = newwin(4, pad_cols + 2, 2, 2); // fixed header
box(header_win, 0, 0);
mvwprintw(header_win, 0, cell_w, "Stock Prices:");
for (int j = 0; j < n_prices; j++) {
    mvwprintw(header_win, 1, (j + 1) * cell_w, "%2.1lf", stock_start + j*0.5);
}
wrefresh(header_win);int max_scroll = pad_rows - visible_rows;
prefresh(matrix_pad, pad_row, 0, 4, 2, 4 + visible_rows - 1, 2 + pad_cols);
while ((ch = getch()) != 'q') {
    if (ch == KEY_DOWN && pad_row < max_scroll)
        pad_row++;
    else if (ch == KEY_UP && pad_row > 0)
        pad_row--;
    else if(ch == 'g'){
        if(!pl){
     for (int i = 0; i < n_days; i++) {
        mvwprintw(matrix_pad, i + 2, 1, "D%d   ", (n_days-i ));
        for (int j = 0; j < n_prices; j++) {
            double p = prices[i][j];
            double ratio = p / max_price;
            int color = (ratio > 0.85) ? 6 :
                        (ratio > 0.65) ? 5 :
                        (ratio > 0.45) ? 4 :
                        (ratio > 0.25) ? 3 :
                        (ratio > 0.10) ? 2 : 1;

            wattron(matrix_pad, COLOR_PAIR(color));
            mvwprintw(matrix_pad, i + 2, (j + 1) * cell_w, "%5.2f", p-0.4);
            wattroff(matrix_pad, COLOR_PAIR(color));
        }
    }
    }else{
        for (int i = 0; i < n_days; i++) {
        mvwprintw(matrix_pad, i + 2, 1, "D%d   ", (n_days-i ));
        for (int j = 0; j < n_prices; j++) {
            double p = prices[i][j];
            double ratio = p / max_price;
            int color = (ratio > 0.85) ? 6 :
                        (ratio > 0.65) ? 5 :
                        (ratio > 0.45) ? 4 :
                        (ratio > 0.25) ? 3 :
                        (ratio > 0.10) ? 2 : 1;

            wattron(matrix_pad, COLOR_PAIR(color));
            mvwprintw(matrix_pad, i + 2, (j + 1) * cell_w, "%5.2f", p);
            wattroff(matrix_pad, COLOR_PAIR(color));
        }
  
    }
    
    }pl=!pl;}
    
        

    
    
    refresh();
    wrefresh(header_win);
    prefresh(matrix_pad, pad_row, 0, 4, 2, 4 + visible_rows - 1, 2 + pad_cols);
}

    delwin(matrix_pad);
    delwin(matrix); 
    endwin();
    return 0;
    
    get_stock_price_on_date("AAPL", "2025-04-01");
    CURL *curl;
    CURLcode res;

    struct MemoryStruct chunk = { .memory = malloc(1), .size = 0 };

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        const char *api_key = "d21ve11r01qktaq783dgd21ve11r01qktaq783e0";
        const char *symbol = "KULR";
        char url[2048];
        snprintf(url, sizeof(url),
            URL_TEMPLATE);

        curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            printf("Response: %s\n", chunk.memory);
            // parse JSON here (you can use cJSON or jsmn)
        } else {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    return 0;   
     cJSON *json = cJSON_Parse(chunk.memory);
    if (!json) {
        fprintf(stderr, "Error before: %s\n", cJSON_GetErrorPtr());
        return;
    }
    
    free(chunk.memory);
    curl_global_cleanup();
    return 0;


// Example: American put
/*
    double S = 7;
    double K = 6;
    double T = (2.0/356.0);
    double r = 0.0435;
    double sigma = 0.62;
    int steps = 1000;

    double call_price = binomial_american_option(S, K, T, r, sigma, steps, "call");
    double put_price  = binomial_american_option(S, K, T, r, sigma, steps, "put");

    printf("American Call Option Price: %.4f\n", call_price);
    printf("American Put Option Price:  %.4f\n", put_price);

    return 0;
    /*
    
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
    return 0; */
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
    case KEY_HOME:
        nodelay(stdscr, TRUE); // Non-blocking input
        char buf[500];
        echo();
        curs_set(2);
        
        mvwgetnstr(win_text,10,10,buf,499);
                timeout(500);
        curs_set(0);
        noecho();
        nodelay(stdscr, FALSE); // Non-blocking input

        break;
    case '\n':
        handle_enter_key();
        break;
    case KEY_MOUSE:
        ui_handle_mouse();
        break;
    }
}

void redraw_ui()
{   /*
    if (count == 0)
    {
        selected_index = 0;
        buffer[0]      = '\0';
    }
    else
    {
        ui_draw_note(file_names[selected_index], buffer);
    }
    */
    //ui_list_notes(win_notes_names, file_names, count, selected_index, buffer);
    //ui_display_options();
    ui_list_option_params(tools_selected);
    //refresh();
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

void ui_handle_mouse(){

    if (getmouse(&event) == OK) {
        if (event.bstate & BUTTON1_CLICKED) {
            mvprintw(2, 0, "Mouse clicked at (%d, %d)", event.y, event.x);
            refresh();
        }
    }
}


size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) return 0;

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

void get_stock_price_on_date(const char *ticker, const char *target_date) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk = {malloc(1), 0};

    // Choose a wide enough date range to catch target_date
    time_t start = 0;  // 1970
    time_t end = time(NULL); // today

    char url[512];
    snprintf(url, sizeof(url), URL_TEMPLATE, ticker, start, end);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        
        if (res == CURLE_OK) {
            // Parse CSV line by line
            printf("Downloaded data:\n%s\n", chunk.memory);
            char *line = strtok(chunk.memory, "\n");
            while (line) {
                if (strstr(line, target_date)) {
                    printf("Data for %s: %s\n", target_date, line);
                    break;
                }
                line = strtok(NULL, "\n");
            }
        } else {
            fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
        free(chunk.memory);
    }
}


// Map option price to a color intensity level
int map_price_to_color(double price, double max_price) {
    double ratio = price / max_price;
    
    if (ratio > 0.85) return 7;
    else if (ratio > 0.8) return 8;
    else if (ratio > 0.70) return 6;
    else if (ratio > 0.55) return 5;
    else if (ratio > 0.40) return 4;
    else if (ratio > 0.25) return 3;
    else if (ratio > 0.10) return 2;
    else return 1;
}