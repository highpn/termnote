#include "../include/notes.h"
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define TEXT_BUFFER_SIZE 4096
#define NOTES_APP_DIR "/notes"
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
    DIR *dirp;
    if (NULL == (dirp = opendir(dir_name)))
    {
        return NULL;
    }
    int fd;
    if ((fd = openat(dirfd(dirp), filename, O_RDONLY)) == -1)
    {
        close(fd);
        closedir(dirp);
        return NULL;
    }
    static char buffer[TEXT_BUFFER_SIZE];
    memset(buffer, 0, TEXT_BUFFER_SIZE); // Clear buffer
    int n;
    if ((n = read(fd, buffer, TEXT_BUFFER_SIZE - 1)) == -1)
    {
        close(fd);
        closedir(dirp);
        return NULL;
    }
    buffer[n] = 0;
    close(fd);
    closedir(dirp);
    return buffer;
}

// List all note filenames
int notes_list(char ***filenames, int *count, const char *dir_name)
{
    DIR           *dirp;
    struct dirent *dir;
    int            temp_count = 0;
    char           names[100][256];

    dirp = opendir(dir_name);
    if (dirp == NULL)
        return -1;

    while ((dir = readdir(dirp)) != NULL)
    {
        if (dir->d_name[0] != '.')
        {
            strncpy(names[temp_count], dir->d_name, 255);
            names[temp_count][255] = '\0';
            temp_count++;
            if (temp_count >= 100)
                break;
        }
    }
    closedir(dirp);

    if (*filenames != NULL && *count > 0)
    {
        for (int i = 0; i < *count; i++)
        {
            free((*filenames)[i]);
        }
        free(*filenames);
        *filenames = NULL;
    }

    *count = temp_count; // Now safe to update

    if (*count == 0)
    {
        *filenames = NULL;
        return 0;
    }

    *filenames = malloc(sizeof(char *) * (*count));
    if (*filenames == NULL)
        return -1;

    for (int i = 0; i < *count; i++)
    {
        (*filenames)[i] = malloc(strlen(names[i]) + 1);
        if ((*filenames)[i] == NULL)
        {
            for (int j = 0; j < i; j++)
                free((*filenames)[j]);
            free(*filenames);
            *filenames = NULL;
            return -1;
        }
        strcpy((*filenames)[i], names[i]);
    }

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
        fprintf(stderr, "Cannot determine home directory.\n");
        return NULL;
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
