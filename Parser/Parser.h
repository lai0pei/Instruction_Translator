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
char* arg1(void);
SEG_TYPE getSeg(void);
unsigned int arg2(void);
bool hasMoreLines(void);
void clean();
void expression(void);
CommandType commandType();
unsigned int  advance(void);
void init(void);
OP_TYPE getOp(void);
char* getBuff(void);
char* getFileName(void);
bool set_fp(void);
void printOpType(void);
void printSegType(void);
void printIndex(void);
#endif