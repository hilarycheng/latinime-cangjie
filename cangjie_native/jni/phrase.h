int search_phrase(jchar index);
int get_phrase_count();
int get_phrase_index();
int get_phrase_max();
jchar* get_phrase(int index);
int get_phrase_length(int index);
void load_phrase(char *path);
void save_phrase();
void clear_phrase();
void update_phrase_frequency(int index);
jint get_phrase_frequency(int index);
void learn_phrase(jchar key, jchar value);
#define MAX_PHRASE_LENGTH 9
