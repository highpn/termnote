int   notes_load_all(const char *dir); // Load all notes from disk
int   notes_save(const char *filename, const char *text, const char *dir_name); // Save a note
char *notes_load(const char *filename, const char *dir_name);          // Load a single note
int   notes_list(char ***filenames, int *count, const char *dir_name); // List all note filenames
int   notes_delete(const char *filename);                              // Delete a note
