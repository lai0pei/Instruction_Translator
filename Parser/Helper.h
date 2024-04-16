#ifndef HELPER
#define HELPER
typedef struct
{
    char *name;
    void *next;
} List;
void get_file_stat();
void free_list(void);
List* get_list(void);
char* get_dir(void);
#endif