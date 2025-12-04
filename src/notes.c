#include "../include/notes.h"
#include "../include/system.h"
#include "../include/ui.h"
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#define TEXT_BUFFER_SIZE 4096
#define NOTES_APP_DIR "/notes"
#define CONFIG_FILE_NAME "/config.txt"
#define MAX_LINE 1024
#define MAX_TEXT 65536


char *notes_dir_path;
int   notes_load_all(const char *dir_name)
{
    DIR *dirp;
    dirp = opendir(dir_name);
    if (dirp == NULL)
    {
        return -1;
    }
    struct dirent *dir;
    int            fd_dirp = dirfd(dirp);
    char           buffer[TEXT_BUFFER_SIZE];
    int            fd;

    while ((dir = readdir(dirp)) != NULL)
    {
        if (*dir->d_name != '.')
        {
            if ((fd = openat(fd_dirp, dir->d_name, O_RDONLY)) == -1)
            {
                return -1;
            }
            if (read(fd, buffer, TEXT_BUFFER_SIZE) == -1)
            {
                return -1;
            }
            buffer[TEXT_BUFFER_SIZE] = 0;
        }
    }
    closedir(dirp);
    return 0;
}

int notes_save(const char *filename, const char *text, const char *dir_name)
{
    DIR *dirp;
    dirp = opendir(dir_name);
    if (dirp == NULL)
    {
        return -1;
    }
    int fd;
    if ((fd = openat(dirfd(dirp), filename, O_CREAT | O_WRONLY, S_IRWXU)) == -1)
    {
        return -1;
    }
    if (write(fd, text, strlen(text)) == -1)
    {
        return -1;
    }
    closedir(dirp);
    return 0;
}
// Load a single note
char *notes_load(const char *filename, const char *dir_name)
{
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", dir_name, filename);

    FILE *f = fopen(path, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char *buffer = malloc(len + 1);
    if (!buffer) { fclose(f); return NULL; }

    fread(buffer, 1, len, f);
    buffer[len] = '\0';
    fclose(f);
    return buffer;
}

// List all note filenames
int notes_list(char ***filenames, int *count, const char *dir_name)
{
   
    *filenames = NULL;
    
    *count = 0;
    
    struct dirent *dir;
    DIR           *dirp;
    dirp = opendir(dir_name);
    if (dirp == NULL)
        return -1;

    while ((dir = readdir(dirp)) != NULL)
    {
        if (dir->d_name[0] != '.')
        {
            *filenames = realloc(*filenames, (*count + 1) * sizeof(char *));
            (*filenames)[*count] = strdup(dir->d_name);
            (*count)++;
        }
    }
    closedir(dirp);
    sort_filenames(*filenames, *count);
    return 0;
}
int notes_delete(const char *filename)
{
    DIR *dirp = opendir(notes_dir_path);
    if (dirp == NULL)
    {
        return -1;
    }
    int fd = openat(dirfd(dirp), filename, O_RDONLY);
    if (fd == -1)
    {
        closedir(dirp);
        return -1;
    }
    close(fd);

    if (unlinkat(dirfd(dirp), filename, 0) == -1)
    {
        closedir(dirp);
        return -1;
    }

    closedir(dirp);
    return 0;
}
/*
// Delete a note
int notes_delete(const char *filename); */

char *get_notes_dir()
{
    const char *xdg_data_home = getenv("XDG_DATA_HOME");
    const char *home          = getenv("HOME");
    char       *path          = NULL;

    // removed: return 0;

    if (xdg_data_home && xdg_data_home[0] != '\0')
    {
        // Use $XDG_DATA_HOME/mynotes_app
        path = malloc(strlen(xdg_data_home) + sizeof(NOTES_APP_DIR));
        sprintf(path, "%s%s", xdg_data_home, NOTES_APP_DIR);
        path[strlen(path)] = '\0'; // Ensure null termination
        notes_dir_path     = path; // Set the global variable
    }
    else if (home && home[0] != '\0')
    {
        // Fallback: ~/.local/share/mynotes_app
        const char *fallback = "/.local/share";
        path                 = malloc(strlen(home) + strlen(fallback) + sizeof(NOTES_APP_DIR));
        sprintf(path, "%s%s%s", home, fallback, NOTES_APP_DIR);
    }
    else
    {
        char buffer[PATH_MAX];
        if (getcwd(buffer, sizeof(buffer)) != NULL)
        {
            path = malloc(strlen(buffer) + sizeof(NOTES_APP_DIR));
            sprintf(path, "%s%s", buffer, NOTES_APP_DIR);
            path[strlen(path)] = '\0'; // Ensure null termination
        }



        fprintf(stderr, "Cannot determine home directory.\n");
        
    }

    // Create directory if it doesn't exist
    struct stat st = {0};
    if (stat(path, &st) == -1)
    {
        if (mkdir(path, 0700) == -1)
        {
            perror("mkdir");
            free(path);
            return NULL;
        }
    }

    notes_dir_path = path; // Set the global variable
                           // Ensure the path ends with a slash
    return path;
}
char *get_config_path() {
    const char *xdg_config_home = getenv("XDG_CONFIG_HOME");
    const char *home = getenv("HOME");
    char *path = malloc(PATH_MAX);
    if (xdg_config_home && xdg_config_home[0] != '\0') {
        snprintf(path, PATH_MAX, "%s%s", xdg_config_home, CONFIG_FILE_NAME);
    } else if (home && home[0] != '\0') {
        snprintf(path, PATH_MAX, "%s/.config%s", home, CONFIG_FILE_NAME);
    } else {
        snprintf(path, PATH_MAX, "./config.txt");
    }
    return path;
}

void load_config(TermNoteConfig *cfg) {
    char *path = get_config_path();
    FILE *f = fopen(path, "r");
    if (!f) {
        // Defaults
        cfg->background_color = 0; // black
        cfg->text_color = 7;       // white
        cfg->last_note[0] = '\0';
        free(path);
        return;
    }
    fscanf(f, "%d %d %255[^\n]", &cfg->background_color, &cfg->text_color, cfg->last_note);
    fclose(f);
    printf("Config loaded from %s\n", path);
    free(path);
}

void save_config(TermNoteConfig *cfg) {
    char *path = get_config_path();
    
    // Ensure directory exists
    char dir[PATH_MAX];
    strncpy(dir, path, PATH_MAX);
    for (int i = strlen(dir); i >= 0; i--) {
        if (dir[i] == '/') { dir[i] = '\0'; break; }
    }
    mkdir(dir, 0700); // create ~/.config/TermNote if needed

    FILE *f = fopen(path, "w");
    if (!f) { free(path); return; }
    fprintf(f, "%d %d %s\n", cfg->background_color, cfg->text_color, cfg->last_note);
    fclose(f);
    free(path);
}

void sort_filenames(char **filenames, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(filenames[i], filenames[j]) > 0) {
                char *temp = filenames[i];
                filenames[i] = filenames[j];
                filenames[j] = temp;
            }
        }
    }
}

void find_command(char *buffer)
{

    char output[MAX_TEXT] = {0};
    char line[MAX_LINE];

    char *p = buffer;

    while (*p) {
        // read a line
        int len = 0;
        while (p[len] && p[len] != '\n' && len < MAX_LINE-1)
            len++;
        strncpy(line, p, len);
        line[len] = '\0';

        // advance pointer past this line
        p += len;
        if (*p == '\n') p++;

        // does the line start with "<<"
        if (strncmp(line, "<<", 2) == 0) {
            char *cmd = line + 2;

            // run the command
            FILE *fp = popen(cmd, "r");
            if (!fp) {
                strcat(output, "[Error running command]\n");
                continue;
            }

            char temp[256];
            while (fgets(temp, sizeof(temp), fp)) {
                strcat(output, temp);
            }
            pclose(fp);
        } else {
            // normal line → copy to output
            strcat(output, line);
            strcat(output, "\n");
        }
    }

    // replace original text
    strcpy(buffer, output);
}