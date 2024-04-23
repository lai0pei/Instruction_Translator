#include "../common.h"
#ifndef CODE_WRITE
#define CODE_WRITE
typedef unsigned short INDEX_TYPE;
void code_init(void);
void code_clean(void);
void end_line(void);
void translate_instruction(void);
#endif