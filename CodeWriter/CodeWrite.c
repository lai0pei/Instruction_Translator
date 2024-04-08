#include "../common.h"
#include "CodeWrite.h"
#define OUTPUT "out.asm"

static STACK_TYPE STACK_POINTER = 256;
static TEMP_TYPE TEMP_POINTER = 5;
static STATIC_TYPE STATIC_POINTER = 16;
static FILE *FP;

static void __file_constructor(void);
static void __pop(SEG_TYPE, INDEX_TYPE);
static void __push(SEG_TYPE, INDEX_TYPE);
static void __load(INDEX_TYPE);
static void __as(char *);
static void __push_constant(INDEX_TYPE);
static void __bin_op(OP_TYPE);
static void __un_op(OP_TYPE);
static void __jump_op(OP_TYPE);
static void __write_comment(void);
static void __increase_stack(void);
static void __decrese_stack(void);

char *getBuff(void);

void __file_constructor(void)
{
    FP = fopen(OUTPUT, "w");
    if (FP == NULL)
    {
        perror("");
        exit(0);
    }
}

void code_init(void)
{
    __file_constructor();
    // __as("@256");
    // __as("D=A");
    // __as("@R0");
    // __as("M=D");
}

void code_clean(void)
{
    fclose(FP);
}

void writeArithematic(OP_TYPE Op)
{
#if DEBUG == 1
    if (Op != UNDEFINED_OPTYPE)
    {
        __write_comment();
    }
#endif
    // binary
    if (Op == ADD || Op == SUB || Op == OR || Op == AND)
    {
        __bin_op(Op);
    }

    if (Op == NOT || Op == NEG)
    {
        __un_op(Op);
    }

    if (Op == NEG || Op == EQ || Op == GT || Op == LT){
        __jump_op(Op);
    }
}

void writePushPop(CommandType ct, OP_TYPE Op, SEG_TYPE st, unsigned int m)
{
#if DEBUG == 1
    if (ct != UNDEFINED_COMMAND_TYPE)
    {
        __write_comment();
    }
#endif
    switch (ct)
    {
    case C_PUSH:
        __push(st, m);
        break;
    case C_POP:
        __pop(st, m);
        break;
    }
}

void __bin_op(OP_TYPE Op)
{
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("A=A-1");
    __as("D=M");
    __as("A=A+1");
    switch (Op)
    {
    case ADD:
        __as("D=D+M");
        break;
    case SUB:
        __as("D=D-M");
        break;
    case AND:
        __as("D=D&M");
        break;
    case OR:
        __as("D=D|M");
        break;
    }

    __as("A=A-1");
    __as("M=D");
    __as("@R0");
    __as("M=M-1");
}


void __jump_op(OP_TYPE Op){
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("A=A-1");
    __as("D=M");
    __as("A=A+1");

}
void __un_op(OP_TYPE Op)
{
    __as("@R0");
    __as("A=M-1");
    switch (Op)
    {
    case NEG:
        __as("D=-M");
        break;
    case NOT:
        __as("D=!M");
        break;
    }
    __as("M=D");
}



void __push(SEG_TYPE segment, INDEX_TYPE index)
{

    if (segment == CONSTANT)
    {
        __push_constant(index);
    }
    else
    {
        switch (segment)
        {
        case POINTER:
            if (index == 0)
            {
                segment = THIS;
            }
            else
            {
                segment = THAT;
            }
        case THIS:
            __as("@R3");
            __as("D=M");
            break;
        case THAT:
            __as("@R4");
            __as("D=M");
            break;
        case LOCAL:
            __as("@R1");
            __as("D=M");
            break;
        case ARGUMENT:
            __as("@R2");
            __as("D=M");
            break;
        case TEMP:
            __load(TEMP_POINTER);
            __as("D=A");
            TEMP_POINTER++;
            break;

        case STATIC:
            __load(STATIC_POINTER);
            __as("D=A");
            STATIC_POINTER++;
            break;
        }

        __load(index);
        __as("A=D+A");
        __as("D=M");
        __as("@R0");
        __as("A=M");
        __as("M=D");
        __as("D=A+1");
        __as("@R0");
        __as("M=D");
    }
}

void __push_constant(INDEX_TYPE value)
{
    __load(value);
    __as("D=A");
    __as("@R0");
    __as("A=M");
    __as("M=D");
    __as("@R0");
    __as("M=M+1");
}

void __pop(SEG_TYPE segment, INDEX_TYPE index)
{

    switch (segment)
    {
    case POINTER:
        if (index == 0)
        {
            segment = THIS;
        }
        else
        {
            segment = THAT;
        }
    case THIS:
        __as("@R3");
        __as("D=M");
        break;
    case THAT:
        __as("@R4");
        __as("D=M");
        break;
    case LOCAL:
        __as("@R1");
        __as("D=M");
        break;
    case ARGUMENT:
        __as("@R2");
        __as("D=M");
        break;
    case TEMP:
        __load(TEMP_POINTER);
        __as("D=A");
        TEMP_POINTER--;
        break;

    case STATIC:
        __load(STATIC_POINTER);
        __as("D=A");
        STATIC_POINTER--;
        break;
    }
    // store to R14
    __load(index);
    __as("D=D+A");
    __as("@R14");
    __as("M=D");
    // wirte to mem
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("D=M");
    __as("@R0");
    __as("M=M-1");
    __as("@R14");
    __as("A=M");
    __as("M=D");
}

void __load(INDEX_TYPE in)
{

    char val[10] = {};
    memset(val, ' ', 10);
    fwrite("@", 1, 1, FP);
    sprintf(val, "%d", in);
    char t = 0;
    while (t < 10)
    {
        if (val[t] == '\0')
        {
            val[t] = ' ';
        }
        t++;
    }
    fwrite(val, 10, 1, FP);
    fwrite("\n", 1, 1, FP);
}

void __as(char *c)
{
    char s = strlen(c);
    fwrite(c, s, 1, FP);
    fwrite("\n", 1, 1, FP);
}

void __write_comment(void)
{
    char *c = getBuff();
    char s = strlen(c);
    fwrite("// ", 3, 1, FP);
    fwrite(c, s, 1, FP);
    fwrite("\n", 1, 1, FP);
}

void end_line(void)
{
    __as("(END)");
    __as("@END");
    __as("0;JMP");
}

void __increase_stack(void)
{
    __as("@R0");
    __as("M=M+1");
}

void __decrease_stack(void)
{
    __as("@R0");
    __as("M=M-1");
}