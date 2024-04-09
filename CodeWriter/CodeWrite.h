#include "../common.h"
#ifndef CODE_WRITE
#define CODE_WRITE
typedef unsigned short STACK_TYPE;
typedef unsigned char TEMP_TYPE;
typedef unsigned char STATIC_TYPE;
typedef unsigned short INDEX_TYPE;
void writeArithematic(OP_TYPE);
void writePushPop(CommandType, OP_TYPE, SEG_TYPE,unsigned int);
void close(void);
void code_init(void);
void code_clean(void);
void end_line(void);
#endif