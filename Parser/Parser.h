#ifndef VM_PARSER
#define VM_PARSER
#define BUFF_LEN 50
typedef union {
    OP_TYPE op;
    SEG_TYPE seg;
} OP_SEG_TYPE;

typedef struct
{
    char *arg;
    OP_SEG_TYPE type;
} Map;
SEG_TYPE arg1(void);
unsigned int arg2(void);
bool hasMoreLines(void);
void parser_clean();
void expression(void);
CommandType commandType();
unsigned int  advance(void);
void parser_init(void);
OP_TYPE getOp(void);
#if DEBUG == 1
void printOpType(void);
void printSegType(void);
void printIndex(void);
char* getBuff(void);
#endif
#endif