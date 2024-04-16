#include "../Common.h"
#include "Parser.h"
#include "Helper.h"
#include <dirent.h>
#include <sys/stat.h>
#define DIR_SIZE 256
#define INIT_FILE "Sys.vm"

extern const char *FILE_NAME;

static List *FILE_LIST = NULL;
static struct stat *STAT = NULL;
static char *__DIR = NULL;

void __open_dir(void);
void __open_file(void);
static void __create_list(const char *);
static void __print();
const char *search(char *);
static void __helper_free(void);

void get_file_stat()
{
    STAT = (struct stat *)malloc(sizeof(struct stat));
    memset(STAT, 0, sizeof(struct stat));
    __DIR = (char *)malloc(DIR_SIZE);
    memset(__DIR, 0, DIR_SIZE);
    int res = stat(FILE_NAME, STAT);
    if (res)
    {
        perror("");
        __helper_free();
        exit(0);
    }

    switch (STAT->st_mode & S_IFMT)
    {
    case S_IFREG:
        __open_file();
        mem_free(STAT);
        break;
    case S_IFDIR:
        __open_dir();
        mem_free(STAT);
        break;
    default:
        printf("Only Regular file and directory \n");
    }
    // __print();
}

void __open_file()
{
    char t = strlen(FILE_NAME);
    const char *file_name = FILE_NAME;
    if (strchr(file_name, '/'))
    {
        while (1)
        {
            if (file_name[t] == '/' || t < 0)
            {
                break;
            }
            t--;
        }
        strncpy(__DIR, file_name, t + 1);
        file_name = file_name + t + 1;
    }
    char *tmp_str = (char *)malloc(strlen(file_name) + 1);
    strcpy(tmp_str, file_name);
    if (strtok(tmp_str, "."))
    {
        if (strcmp(strtok(NULL, "."), "vm"))
        {
            fprintf(stderr, "Not a vm file\n");
            mem_free(tmp_str);
            __helper_free();
            exit(0);
        }
    }
    mem_free(tmp_str);
    __create_list(file_name);
}

void __open_dir()
{
    strcpy(__DIR, FILE_NAME);
    DIR *dir = opendir(FILE_NAME);
    struct dirent *t = NULL;
    char *c = NULL;
    char *td_name = (char *)malloc(256);
    memset(td_name, 0, 256);
    // put all the .vm file to the list
    while (true)
    {
        t = readdir(dir);
        if (!t)
        {
            break;
        }

        strcpy(td_name, t->d_name);

        if (strtok(td_name, "."))
        {
            c = strtok(NULL, ".");
            if (c)
            {
                if (strcmp("vm", c) == 0)
                {
                    __create_list(t->d_name);
                }
            }
        }
    }
    mem_free(td_name);
    closedir(dir);

    if (!FILE_LIST)
    {
        fprintf(stderr, "No .vm file found\n");
        __helper_free();
        exit(0);
    }
    // swap Sys.vm file to the first place
    List *f = FILE_LIST;
    char *swap = NULL;
    while (f)
    {
        if (!strcmp(INIT_FILE, f->name))
        {
            swap = f->name;
            f->name = FILE_LIST->name;
            FILE_LIST->name = swap;
            break;
        }
        f = f->next;
    }
}

// create a list
void __create_list(const char *c)
{
    char *t = (char *)malloc(strlen(c) + 1);

    if (!FILE_LIST)
    {
        FILE_LIST = (List *)malloc(sizeof(List));
        strcpy(t, c);
        FILE_LIST->name = t;
        FILE_LIST->next = NULL;
    }
    else
    {
        List *m = FILE_LIST;
        while (1)
        {
            if (!m->next)
            {
                List *l = (List *)malloc(sizeof(List));
                strcpy(t, c);
                l->name = t;
                l->next = NULL;
                m->next = l;
                break;
            }
            m = m->next;
        }
    }
}

char *get_dir()
{
    return __DIR;
}

List *get_list()
{
    return FILE_LIST;
}

void free_list()
{
    List *tmp = FILE_LIST;
    while (tmp)
    {
        List *t2 = (List *)tmp->next;
        mem_free(tmp->name);
        mem_free(tmp);
        tmp = t2;
    }
    mem_free(__DIR);
}

void mem_free(void *mem)
{
    if (mem)
        free(mem);
    mem = NULL;
}

void file_free(void *file)
{
    if (file)
        fclose(file);

    file = NULL;
}

void __print()
{
    List *tmp = FILE_LIST;
    while (tmp)
    {
        if (tmp->name)
        {
            printf("F %s\n", tmp->name);
        }
        tmp = tmp->next;
    }
}

void __helper_free()
{
    mem_free(STAT);
    mem_free(__DIR);
}