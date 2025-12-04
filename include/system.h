typedef struct {
    int background_color;
    int text_color;
    char last_note[256];
} TermNoteConfig;

char *get_notes_dir(void);

void save_config(TermNoteConfig *cfg);
void load_config(TermNoteConfig *cfg);
char *get_config_path();

