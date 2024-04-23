#include "../Common.h"
#include "Parser.h"
#include "Helper.h"
#include "../CodeWriter/CodeWrite.h"

#define CHAR_STACK 20
#define OP_COUNT 17
#define SEG_COUNT 8
#define FILE_SIZE 256
#define __ERROR(S)                         \
    fprintf(stderr, "%s. %s \n", S, BUFF); \
    __exit(0);

static Map OPERATION[OP_COUNT] = {
    {"push", .type.op = PUSH},
    {"pop", .type.op = POP},
    {"add", .type.op = ADD},
    {"sub", .type.op = SUB},
    {"neg", .type.op = NEG},
    {"eq", .type.op = EQ},
    {"gt", .type.op = GT},
    {"lt", .type.op = LT},
    {"and", .type.op = AND},
    {"or", .type.op = OR},
    {"not", .type.op = NOT},
    {"function", .type.op = FN},
    {"label", .type.op = LABEL},
    {"call", .type.op = CALL},
    {"return", .type.op = RETURN},
    {"goto", .type.op = GOTO},
    {"if-goto", .type.op = IF_GOTO}};

static Map SEGMENT[SEG_COUNT] = {
    {"constant", .type.seg = CONSTANT},
    {"local", .type.seg = LOCAL},
    {"temp", .type.seg = TEMP},
    {"argument", .type.seg = ARGUMENT},
    {"static", .type.seg = STATIC},
    {"this", .type.seg = THIS},
    {"that", .type.seg = THAT},
    {"pointer", .type.seg = POINTER}};

static CommandType TYPE;
static OP_TYPE Op;
static SEG_TYPE Seg;

static FILE *FP;
static char *BUFF;
static char *COMMENT;
static char *VAR = NULL;
static char *FILE_NAME = NULL;
static char BUFF_COUNT = 0;
static bool EOL = false;
static unsigned int ARG2;
static char *ARG1 = NULL;
static char *TOK;
static List *FILE_LIST = NULL;
static char *__DIR = NULL;
static char *FILE_PATH = NULL;
static char *TOKEN = NULL;

static void __operation(void);
static void __segment(void);
static void __index(void);
static void __next(void);

static void __exit(int type)
{
    clean();
    exit(type);
}

bool hasMoreLines(void)
{
    return !EOL;
}

void init(void)
{

    get_file_stat();

    BUFF = (char *)malloc(sizeof(char) * BUFF_LEN);
    if (!BUFF)
    {
        perror("Buffer allocation failed!");
        __exit(0);
    }

    COMMENT = (char *)malloc(sizeof(char) * BUFF_LEN);
    if (!COMMENT)
    {
        perror("Buffer allocation failed!");
        __exit(0);
    }
    memset(COMMENT, 0, BUFF_LEN);

    ARG1 = (char *)malloc(sizeof(char) * CHAR_STACK);
    if (!ARG1)
    {
        perror("Buffer allocation failed!");
        __exit(0);
    }
    memset(ARG1, 0, CHAR_STACK);

    FILE_NAME = (char *)malloc(sizeof(char) * CHAR_STACK);
    if (!FILE_NAME)
    {
        perror("Buffer allocation failed!");
        __exit(0);
    }
    memset(FILE_NAME, 0, CHAR_STACK);

    FILE_LIST = get_list();
    __DIR = get_dir();
    FILE_PATH = (char *)malloc(FILE_SIZE);
    VAR = (char *)malloc(CHAR_STACK);
    code_init();
}

bool set_fp()
{
    if (FILE_LIST)
    {
        memset(FILE_PATH, 0, FILE_SIZE);
        memcpy(FILE_PATH, __DIR, strlen(__DIR));
   
        if (FP)
        {
            file_free(FP);
        }

        FP = fopen(strcat(FILE_PATH, FILE_LIST->name), "r");
        if (!FP)
        {
            perror("");
            __exit(0);
        }
        memset(FILE_NAME, 0, CHAR_STACK);
        strcpy(FILE_NAME,FILE_LIST->name);
        strtok(FILE_NAME,".");
        FILE_LIST = FILE_LIST->next;
        EOL = false;
        return true;
    }
    return false;
}

char* getFileName(){
    return FILE_NAME;
}

unsigned int advance(void)
{
    BUFF_COUNT = 0;
    ARG2 = 0;
    bool is_char = false;
    Op = UNDEFINED_OPTYPE;
    TYPE = UNDEFINED_COMMAND_TYPE;
    Seg = UNDEFINED_SEG_TYPE;
    memset(BUFF, 0, BUFF_LEN);
    memset(ARG1, 0, CHAR_STACK);
    memset(COMMENT, 0, BUFF_LEN);
    TOK = BUFF;
    int m = 0;
    unsigned int t = 0;
    bool is_comment = false;

    while ((m = fgetc(FP)) != '\n')
    {
        if (m == EOF)
        {
            EOL = true;
            break;
        }

        if (!isspace(m))
        {
            is_char = true;
        }

        if (m == '/' && fgetc(FP) == '/')
        {
            is_comment = true;
            continue;
        }

        if (!is_char || is_comment)
        {
            continue;
        }

        if (BUFF_COUNT > BUFF_LEN)
        {
            fprintf(stderr, "Max allowed char per line is %d\n", BUFF_LEN);
            __exit(0);
        }
        BUFF[t] = m;
        BUFF_COUNT++;
        t++;
    }
    strcpy(COMMENT,BUFF);
    return t;
}

void expression()
{
    __operation();
    __segment();
    __index();
}

void __operation()
{
    __next();

    char t = 0;

    while (t < OP_COUNT)
    {
        if (!strcmp(VAR, OPERATION[t].arg))
        {
            Op = OPERATION[t].type.op;
            break;
        }
        t++;
    }

    if (t >= OP_COUNT)
    {
        __ERROR("Unspported Operation ")
    }

    if (Op == PUSH)
    {
    }

    if (Op == PUSH)
    {
        TYPE = C_PUSH;
        goto Label;
    }

    if (Op == POP)
    {
        TYPE = C_POP;
        goto Label;
    }

    if (Op == POP)
    {
        TYPE = C_POP;
        goto Label;
    }

    if (Op == POP)
    {
        TYPE = C_POP;
        goto Label;
    }

    if (Op == ADD || Op == SUB || Op == NEG || Op == EQ || Op == GT || Op == LT || Op == AND || Op == OR || Op == NOT)
    {
        TYPE = C_ARITHEMATIC;
        goto Label;
    }

    if (Op == FN)
    {
        TYPE = C_FUNCTION;
        goto Label;
    }

    if (Op == LABEL)
    {
        TYPE = C_LABEL;
        goto Label;
    }

    if (Op == CALL)
    {
        TYPE = C_CALL;
        goto Label;
    }

    if (Op == RETURN)
    {
        TYPE = C_RETURN;
        goto Label;
    }

    if (Op == GOTO)
    {
        TYPE = C_GOTO;
        goto Label;
    }

    if (Op == IF_GOTO)
    {
        TYPE = C_IF;
        goto Label;
    }

    fprintf(stderr, "Unknown Operation \n");
    __exit(0);
Label:
}

void __segment()
{

    if (Op == PUSH || Op == POP)
    {
        __next();
        char t = 0;

        if (!strlen(VAR))
        {
            __ERROR("No segment found! ")
        }

        while (t < SEG_COUNT)
        {
            if (!strcmp(VAR, SEGMENT[t].arg))
            {
                Seg = SEGMENT[t].type.seg;
                break;
            }
            t++;
        }

        if (t >= SEG_COUNT)
        {
            __ERROR("Unsupported Segment")
        }
    }

    if (Op == LABEL || Op == FN || Op == CALL || Op == GOTO || Op == IF_GOTO)
    {
        __next();
        strcpy(ARG1, VAR);
    }
}

void __index()
{
    if (Op == PUSH || Op == POP || Op == FN || Op == CALL)
    {
        __next();

        if (!strlen(VAR))
        {
            __ERROR("No index found! ")
        }
        char t = 0;
        while (VAR[t] != '\0')
        {
            if (isalpha(VAR[t]))
            {
                __ERROR("Only support positive integer!")
            }
            t++;
        }
        ARG2 = atoi(VAR);
    }
}

void __next(void)
{
    memset(VAR, 0, CHAR_STACK);
    TOKEN = strtok(TOK, " ");

    TOK = NULL;
    char t = 0;

    while (TOKEN && *TOKEN != '\0')
    {
        if (!isspace(*TOKEN))
        {
            VAR[t] = *TOKEN;
            t++;
        }
        TOKEN++;
    }
}

SEG_TYPE getSeg(void)
{
    return Seg;
}

char *arg1(void)
{
    return ARG1;
}

OP_TYPE getOp()
{
    return Op;
}

CommandType commandType(void)
{
    return TYPE;
}

unsigned int arg2()
{
    return ARG2;
}

void clean(void)
{
    mem_free(BUFF);
    mem_free(FILE_PATH);
    mem_free(ARG1);
    mem_free(VAR);
    mem_free(COMMENT);
    mem_free(FILE_NAME);
    free_list();
    if (FP)
        file_free(FP);
    code_clean();
}

char *getBuff(void)
{
    return COMMENT;
}

#if DEBUG == 1
void printOpType(void)
{
    char c[10] = {};
    switch (Op)
    {
    case PUSH:
        strcpy(c, "PUSH");
        break;
    case POP:
        strcpy(c, "POP");
        break;
    case ADD:
        strcpy(c, "ADD");
        break;
    case SUB:
        strcpy(c, "SUB");
        break;
    case NEG:
        strcpy(c, "NEG");
        break;
    case EQ:
        strcpy(c, "EQ");
        break;
    case GT:
        strcpy(c, "GT");
        break;
    case LT:
        strcpy(c, "LT");
        break;
    case AND:
        strcpy(c, "AND");
        break;
    case OR:
        strcpy(c, "OR");
        break;
    case NOT:
        strcpy(c, "NOT");
        break;
    case FN:
        strcpy(c, "FN");
        break;
    case LABEL:
        strcpy(c, "LABEL");
        break;
    case CALL:
        strcpy(c, "CALL");
        break;
    case RETURN:
        strcpy(c, "RETURN");
        break;
    case GOTO:
        strcpy(c, "GOTO");
        break;
    case IF_GOTO:
        strcpy(c, "IF_GOTO");
        break;
    }
    printf("%s ", c);
    memset(c, ' ', 10);
}

void printSegType(void)
{
    char c[20] = {};
    switch (Seg)
    {
    case CONSTANT:
        strcpy(c, "CONSTANT");
        break;

    case LOCAL:
        strcpy(c, "LOCAL");
        break;

    case ARGUMENT:
        strcpy(c, "ARGUMENT");
        break;

    case TEMP:
        strcpy(c, "TEMP");
        break;

    case THIS:
        strcpy(c, "THIS");
        break;

    case THAT:
        strcpy(c, "THAT");
        break;

    case POINTER:
        strcpy(c, "POINTER");
        break;

    case STATIC:
        strcpy(c, "STATIC");
        break;
    }

    if (Op == LABEL || Op == FN || Op == CALL)
    {
        strcpy(c, ARG1);
    }
    printf("%s ", c);
    memset(c, ' ', 20);
}

void printIndex(void)
{
    if (Op == PUSH || Op == POP || Op == FN || Op == CALL)
        printf("%d", ARG2);

    printf("\n");
}

#endif