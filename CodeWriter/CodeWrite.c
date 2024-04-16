#include "../common.h"
#include "CodeWrite.h"
#include "../Parser/Parser.h"
#define STRING_SIZE 20
#define OUTPUT "out.asm"
#define BUFF_SIZE 4096 * 10 // bytes
#define INCREASE_STACK \
    __as("@R0");       \
    __as("M=M+1");
#define DECREASE_STACK \
    __as("@R0");       \
    __as("M=M-1");

static const char __SP[3] = "@R0";
static const char __LOL[3] = "@R1";
static const char __ARG[3] = "@R2";
static const char __THIS[3] = "@R3";
static const char __THAT[3] = "@R4";
static const char __LABEL[3] = "lb_";

static STACK_TYPE STACK_POINTER = 256;
static TEMP_TYPE TEMP_POINTER = 5;
static STATIC_TYPE STATIC_POINTER = 16;
static unsigned int M_C = 0;
static char *OUTPUT_BUFFER = NULL;
static FILE *FP;
static unsigned int OUTPUT_COUNT = 0;
static char *INT_TO_STR = NULL;
static CommandType COMMAND_TYPE;
static SEG_TYPE SEG;
static INDEX_TYPE ARG2;
static char *ARG1 = NULL;
static OP_TYPE OPERATION;

static void __file_constructor(void);
static void __pop(void);
static void __push(void);
static void __load_num(INDEX_TYPE);
static void __as(const char *);
static void __push_constant(void);
static void __push_pointer(void);
static void __push_memory(void);
static void __pop_memory(void);
static void __pop_pointer(void);
static void __bin_op(void);
static void __un_op(void);
static void __comp_op(void);
static void __write_comment(void);
static void __int_to_str(unsigned int);
static void __gen_label(char *);
static void __flush_to_file(void);
static void __wb(const char *);
static void __write_label(void);
static void __write_goto(void);
static void __write_if(void);
static void __write_function(void);
static void __write_call(void);
static void __write_return(void);
static void writePushPop(void);
static void writeArithematic(void);
static void __load_label(char *);
static void __label(char *);

char *getBuff(void);

void __file_constructor(void)
{
    FP = fopen(OUTPUT, "w");
    if (!FP)
    {
        perror("");
        clean();
        exit(0);
    }
}

void code_init(void)
{
    __file_constructor();
    OUTPUT_BUFFER = (char *)malloc(BUFF_SIZE);
    memset(OUTPUT_BUFFER, 0, BUFF_SIZE);
    INT_TO_STR = (char *)malloc(STRING_SIZE);
    // stack pointer
    __as("@256");
    __as("D=A");
    __as("@R0");
    __as("M=D");

    // local
    __as("@300");
    __as("D=A");
    __as("@R1");
    __as("M=D");

    // arguemnt
    __as("@400");
    __as("D=A");
    __as("@R2");
    __as("M=D");
    // this
    __as("@3000");
    __as("D=A");
    __as("@R3");
    __as("M=D");
    // that
    __as("@3010");
    __as("D=A");
    __as("@R4");
    __as("M=D");
    __flush_to_file();
}

void translate_instruction()
{
    COMMAND_TYPE = commandType();
    SEG = getSeg();
    ARG2 = arg2();
    OPERATION = getOp();
    ARG1 = arg1();

    writePushPop();
    writeArithematic();
    __write_label();
}

void __write_label()
{
    if (OPERATION == LABEL)
    {
        __label(ARG1);
    }
}

void __write_goto()
{
    if (OPERATION == GOTO)
    {
        __load_label(ARG1);
        __as("0;JMP");
    }
}

void __write_if()
{
    if (OPERATION == IF_GOTO)
    {
        __as("@R0");
        __as("A=M");
        __as("A=A-1");
        __as("D=M");
        DECREASE_STACK
        __load_label(ARG1);
        __as("D;JNQ");
    }
}

void __write_function()
{
    if (OPERATION == FN)
    {
        __label(ARG1);
    }
}

void __write_call()
{
    // push return

    // push loc
    __as("@R1");
    __as("D=M");
    __as("@R0");
    __as("A=M");
    __as("M=D");
    INCREASE_STACK

    // push arg
    __as("@R2");
    __as("D=M");
    __as("@R0");
    __as("A=M");
    __as("M=D");
    INCREASE_STACK

    // push this
    __as("@R3");
    __as("D=M");
    __as("@R0");
    __as("A=M");
    __as("M=D");
    INCREASE_STACK

    // push that
    __as("@R4");
    __as("D=M");
    __as("@R0");
    __as("A=M");
    __as("M=D");
    INCREASE_STACK

    INDEX_TYPE nargs = 5 + ARG2;
    // reposition arguemnt
    __load_num(nargs);
    __as("D=A");
    __as("@R0");
    __as("D=M+D");
    __as("@R2");
    __as("M=D");

    // load stack to local
    __as("@R0");
    __as("D=M");
    __as("@R1");
    __as("M=D");

    // go to function call
    __load_label(ARG1);
    __as("0;JMP");
}

void __write_return()
{
}

void writeArithematic()
{
    // binary
    if (OPERATION == ADD || OPERATION == SUB || OPERATION == OR || OPERATION == AND)
    {
        __bin_op();
    }

    if (OPERATION == NOT || OPERATION == NEG)
    {
        __un_op();
    }

    if (OPERATION == EQ || OPERATION == GT || OPERATION == LT)
    {
        __comp_op();
    }
}

void writePushPop()
{
#if DEBUG == 1
    if (COMMAND_TYPE != UNDEFINED_COMMAND_TYPE)
    {
        __write_comment();
    }
#endif
    switch (COMMAND_TYPE)
    {
    case C_PUSH:
        __push();
        break;
    case C_POP:
        __pop();
        break;
    }
}

void __bin_op()
{

    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("D=M");
    __as("A=A-1");
    switch (OPERATION)
    {
    case ADD:
        __as("D=D+M");
        break;
    case SUB:
        __as("D=M-D");
        break;
    case AND:
        __as("D=D&M");
        break;
    case OR:
        __as("D=D|M");
        break;
    }
    __as("M=D");
    __as("@R0");
    __as("M=M-1");
}

void __comp_op()
{
#if DEBUG == 1
    if (OPERATION != UNDEFINED_OPTYPE)
    {
        __write_comment();
    }
#endif
    char nm_loc[STRING_SIZE] = {};
    __gen_label(nm_loc);
    char next_loc[STRING_SIZE] = {};
    __gen_label(next_loc);
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("D=M");
    __as("A=A-1");
    __as("D=M-D");
    __load_label(nm_loc);
    switch (OPERATION)
    {
    case EQ:
        __as("D;JNE");
        break;
    case GT:
        __as("D;JLE");
        break;
    case LT:
        __as("D;JGE");
        break;
    }
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("A=A-1");
    __as("M=-1");
    __load_label(next_loc);
    __as("0;JMP");
    __label(nm_loc);
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("A=A-1");
    __as("M=0");
    __label(next_loc);
    __as("@R0");
    __as("M=M-1");
}

void __un_op()
{
#if DEBUG == 1
    if (OPERATION != UNDEFINED_OPTYPE)
    {
        __write_comment();
    }
#endif
    __as("@R0");
    __as("A=M-1");
    switch (OPERATION)
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

void __push()
{

    switch (SEG)
    {
    case CONSTANT:
        __push_constant();
        break;
    case POINTER:
        __push_pointer();
        break;
    case THIS:
        __as("@R3");
        __as("D=M");
        __push_memory();
        break;
    case THAT:
        __as("@R4");
        __as("D=M");
        __push_memory();
        break;
    case LOCAL:
        __as("@R1");
        __as("D=M");
        __push_memory();
        break;
    case ARGUMENT:
        __as("@R2");
        __as("D=M");
        __push_memory();
        break;
    case TEMP:
        __load_num(TEMP_POINTER);
        __as("D=A");
        __push_memory();
        break;

    case STATIC:
        __load_num(STATIC_POINTER);
        __as("D=A");
        __push_memory();
        break;
    }
}

void __push_constant()
{
    __load_num(ARG2);
    __as("D=A");
    __as("@R0");
    __as("A=M");
    __as("M=D");
    __as("@R0");
    __as("M=M+1");
}

void __push_pointer()
{
    if (ARG2 == 0)
    {
        __as("@R3");
    }

    if (ARG2 == 1)
    {
        __as("@R4");
    }
    __as("D=M");
    __as("@R0");
    __as("A=M");
    __as("M=D");
    __as("@R0");
    __as("M=M+1");
}

void __push_memory()
{
    __load_num(ARG2);
    __as("A=D+A");
    __as("D=M");
    __as("@R0");
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
}

void __pop()
{

    switch (SEG)
    {
    case POINTER:
        __pop_pointer();
        break;
    case THIS:
        __as("@R3");
        __as("D=M");
        __pop_memory();
        break;
    case THAT:
        __as("@R4");
        __as("D=M");
        __pop_memory();
        break;
    case LOCAL:
        __as("@R1");
        __as("D=M");
        __pop_memory();
        break;
    case ARGUMENT:
        __as("@R2");
        __as("D=M");
        __pop_memory();
        break;
    case TEMP:
        __load_num(TEMP_POINTER);
        __as("D=A");
        __pop_memory();
        break;
    case STATIC:
        __load_num(STATIC_POINTER);
        __as("D=A");
        __pop_memory();
        break;
    }
}

void __pop_memory()
{
    // store to R14
    __load_num(ARG2);
    __as("D=D+A");
    __as("@R14");
    __as("M=D");
    // wirte to mem
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("D=M");
    __as("@R14");
    __as("A=M");
    __as("M=D");
    DECREASE_STACK
}

void __pop_pointer()
{
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("D=M");
    if (ARG2 == 0)
    {
        __as("@R3");
    }
    else
    {
        __as("@R4");
    }
    __as("M=D");
    __as("@R0");
    __as("M=M-1");
}

void __load_num(INDEX_TYPE in)
{
    __int_to_str(in);
    __wb("@");
    __wb(INT_TO_STR);
    __wb("\n");
}

void __as(const char *c)
{
    __wb(c);
    OUTPUT_BUFFER[OUTPUT_COUNT] = '\n';
    OUTPUT_COUNT++;
}

void __write_comment(void)
{
    __wb("//");
    __wb(getBuff());
    __wb("\n");
}

void end_line(void)
{
    __as("(END)");
    __as("@END");
    __as("0;JMP");
    __flush_to_file();
}

void __int_to_str(unsigned int in)
{
    memset(INT_TO_STR, ' ', STRING_SIZE);
    sprintf(INT_TO_STR, "%d", in);
    char t = 0;
    while (t < STRING_SIZE)
    {
        if (INT_TO_STR[t] == '\0')
        {
            INT_TO_STR[t] = ' ';
        }
        t++;
    }
}

void __gen_label(char *label)
{
    __int_to_str(M_C);
    strcat(label, __LABEL);
    strcat(label, INT_TO_STR);
    M_C++;
}

void code_clean(void)
{
    mem_free(INT_TO_STR);
    mem_free(OUTPUT_BUFFER);
    if (FP)
        file_free(FP);
}

void __wb(const char *c)
{
    while (*c != '\0')
    {
        if (OUTPUT_COUNT >= BUFF_SIZE)
        {
            fwrite(OUTPUT_BUFFER, 1, OUTPUT_COUNT, FP);
            memset(OUTPUT_BUFFER, 0, BUFF_SIZE);
            OUTPUT_COUNT = 0;
        }

        OUTPUT_BUFFER[OUTPUT_COUNT] = *c;
        OUTPUT_COUNT++;
        c++;
    }
}

void __flush_to_file()
{
    if (OUTPUT_COUNT > 0)
    {
        fwrite(OUTPUT_BUFFER, 1, OUTPUT_COUNT, FP);
        memset(OUTPUT_BUFFER, 0, BUFF_SIZE);
        OUTPUT_COUNT = 0;
    }
}

void __load_label(char *c)
{
    __wb("@");
    __wb(c);
    __wb("\n");
}

void __label(char *c)
{
    __wb("(");
    __wb(c);
    __wb(")");
    __wb("\n");
}