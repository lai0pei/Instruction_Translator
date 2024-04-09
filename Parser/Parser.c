#include "../common.h"
#include "./Parser.h"
#include "../CodeWriter/CodeWrite.h"


#define CHAR_STACK 50
#define OP_COUNT 11
#define SEG_COUNT 8
#define __ERROR(S)                         \
    fprintf(stderr, "%s. %s \n", S, BUFF); \
    exit(0);

extern char* FILE_NAME;
static CommandType TYPE;
static OP_TYPE Op;
static SEG_TYPE Seg;


static FILE *FP;
static char *BUFF;
static char *VAR = NULL;
static char BUFF_COUNT = 0;
static char INDEX = 0;
static bool EOL = false;
static unsigned int ARG2;


static void __operation(void);
static void __segment(void);
static void __index(void);
static void __next(void);
static void __file_constructor(void);


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
    {"not", .type.op = NOT}};

static Map SEGMENT[SEG_COUNT] = {
    {"constant", .type.seg = CONSTANT},
    {"local", .type.seg = LOCAL},
    {"temp", .type.seg = TEMP},
    {"argument", .type.seg = ARGUMENT},
    {"static", .type.seg = STATIC},
    {"this", .type.seg = THIS},
    {"that", .type.seg = THAT},
    {"pointer", .type.seg = POINTER}};

bool hasMoreLines(void)
{
    return !EOL;
}

void __file_constructor(void){
    FP = fopen(FILE_NAME,"r");
    if(FP == NULL){
        perror("");
        exit(0);
    }
}

void parser_init(void)
{
    __file_constructor();
    BUFF = (char *)malloc(sizeof(char) * BUFF_LEN);
    if (BUFF == NULL)
    {
        perror("Buffer allocation failed!");
        exit(0);
    }
    memset(BUFF, 0, BUFF_LEN);
    VAR = (char *)malloc(sizeof(char) * CHAR_STACK);
    if (BUFF == NULL)
    {
        perror("Char Stack allocation failed!");
        exit(0);
    }
    EOL = false;
}



unsigned int advance(void)
{

    BUFF_COUNT = 0;
    INDEX = 0;
    ARG2 = 0;
    Op = UNDEFINED_OPTYPE;
    TYPE = UNDEFINED_COMMAND_TYPE;
    Seg = UNDEFINED_SEG_TYPE;
    memset(VAR, '\0', CHAR_STACK);
    memset(BUFF, 0, BUFF_LEN);

    int m = 0;
    unsigned int t = 0;
    bool is_space = true;
    bool is_comment = false;

    while ((m = fgetc(FP)) != '\n')
    {

        if (m == EOF)
        {
            EOL = true;
            break;
        }

        if (is_comment || (m == '/' && fgetc(FP) == '/'))
        {
            is_comment = true;
            t = 0;
            continue;
        }

        if (isalnum(m))
        {
            is_space = false;
        }

        if (BUFF_COUNT > BUFF_LEN)
        {
            fprintf(stderr, "Max allowed char per line is %d\n", BUFF_LEN);
            exit(0);
        }

        BUFF[t] = m;
        BUFF_COUNT++;
        t++;
    }
    if (is_space)
        t = 0;

    return t;
}

void parser_clean(void){
    free(BUFF);
    fclose(FP);
}

CommandType commandType(void)
{
    return TYPE;
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
        if (strcmp(VAR, OPERATION[t].arg) == 0)
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

    switch (Op)
    {
    case PUSH:
        TYPE = C_PUSH;
        break;
    case POP:
        TYPE = C_POP;
        break;
    case ADD:
        TYPE = C_ARITHEMATIC;
        break;
    case SUB:
        TYPE = C_ARITHEMATIC;
        break;
    }
}

void __segment()
{

    if (Op == PUSH || Op == POP)
    {
        __next();
        char t = 0;

        if (strlen(VAR) == 0)
        {
            __ERROR("No segment and index found! ")
        }

        while (t < SEG_COUNT)
        {
            if (strcmp(VAR, SEGMENT[t].arg) == 0)
            {
                Seg = SEGMENT[t].type.seg;
                break;
            }
            t++;
        }

        if (t >= SEG_COUNT)
        {
            __ERROR("Unsported Segment ")
        }

    }
}

void __index()
{
    if (Op == PUSH || Op == POP)
    {
        __next();

        if (strlen(VAR) == 0)
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

SEG_TYPE arg1(void)
{
    return Seg;
}

OP_TYPE getOp(){
    return Op;
}

unsigned int arg2()
{
    return ARG2;
}

void __next(void)
{
    char temporary_index = 0;
    memset(VAR, 0, CHAR_STACK);

    while (INDEX < BUFF_COUNT && isspace(BUFF[INDEX]) == 0)
    {
        VAR[temporary_index] = BUFF[INDEX];
        INDEX++;
        temporary_index++;
    }
    INDEX++;
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
    }
    printf("%s ", c);
    memset(c, ' ', 10);
}

void printSegType(void)
{
    char c[10] = {};
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
    printf("%s ", c);
    memset(c, ' ', 10);
}

void printIndex(void)
{
    printf("%d \n", ARG2);
}

char* getBuff(void){
    return BUFF;
}
#endif