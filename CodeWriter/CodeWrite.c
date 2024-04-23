#include "../common.h"
#include "CodeWrite.h"
#include "../Parser/Parser.h"
#define STRING_SIZE 64
#define TEMP_POINTER 5
#define STATIC_POINTER 16
#define OUTPUT "out.asm"
#define BUFF_SIZE 4096 * 10 // bytes
#define INCREASE_STACK \
    __as(__SP);        \
    __as("M=M+1");
#define DECREASE_STACK \
    __as(__SP);        \
    __as("M=M-1");
#if DEBUG == 1
#define COMMENT(S) \
    __wb("//");    \
    __wb(S);       \
    __wb("\n");
#else
#define COMMENT(S)
#endif

static FILE *FP;
static CommandType COMMAND_TYPE;
static SEG_TYPE SEG;
static OP_TYPE OPERATION;
static INDEX_TYPE ARG2;
static char *FILE_NAME = NULL;
static char *ARG1 = NULL;
static unsigned int OUTPUT_COUNT = 0;
static char *INT_TO_STR = NULL;
static unsigned int COMP_COUNT = 1;
static unsigned int RETURN_COUNT = 1;
static char *OUTPUT_BUFFER = NULL;
static char FUNCTION_NAME[STRING_SIZE] = {};

static const char __SP[4] = "@R0\0";
static const char __LOL[4] = "@R1\0";
static const char __ARG[4] = "@R2\0";
static const char __THIS[4] = "@R3\0";
static const char __THAT[4] = "@R4\0";
static const char __LABEL[4] = "lb_\0";

static void __file_constructor(void);
static void __pop(void);
static void __push(void);
static void __load_num(INDEX_TYPE);
static void __as(const char *);

static void __bin_op(void);
static void __un_op(void);
static void __comp_op(void);
static void __int_to_str(unsigned int);
static void __gen_label(char *, unsigned int);
static void __flush_to_file(void);
static void __wb(const char *);
static void __write_label(void);
static void __write_goto(void);
static void __write_if(void);
static void __write_function(void);
static void __write_call(void);
static void __write_return(void);
static void __writeArithematic(void);
static void __load_label(char *);
static void __label(char *);
static void __exit(int);

static void __push_local(INDEX_TYPE);
static void __push_constant(INDEX_TYPE);
static void __push_this(INDEX_TYPE);
static void __push_that(INDEX_TYPE);
static void __push_argument(INDEX_TYPE);
static void __push_temp(INDEX_TYPE, INDEX_TYPE);
static void __push_static(INDEX_TYPE);
static void __push_pointer(INDEX_TYPE);
static void __push_memory(INDEX_TYPE);

static void __pop_local(INDEX_TYPE);
static void __pop_this(INDEX_TYPE);
static void __pop_that(INDEX_TYPE);
static void __pop_argument(INDEX_TYPE);
static void __pop_temp(INDEX_TYPE, INDEX_TYPE);
static void __pop_static(INDEX_TYPE);
static void __pop_pointer(INDEX_TYPE);
static void __pop_memory(INDEX_TYPE);

char *getBuff(void);

void __file_constructor(void)
{
    FP = fopen(OUTPUT, "w");
    if (!FP)
    {
        perror("");
        __exit(0);
    }
}

void code_init(void)
{
    __file_constructor();
    OUTPUT_BUFFER = (char *)malloc(BUFF_SIZE);
    memset(OUTPUT_BUFFER, 0, BUFF_SIZE);
    INT_TO_STR = (char *)malloc(STRING_SIZE);

    // stack pointer
    COMMENT("boostrapping")
    __as("@256");
    __as("D=A");
    __as(__SP);
    __as("M=D");
    // local
    __as("@300");
    __as("D=A");
    __as(__LOL);
    __as("M=D");
    // arguemnt
    __as("@400");
    __as("D=A");
    __as(__ARG);
    __as("M=D");
    // this
    __as("@3000");
    __as("D=A");
    __as(__THIS);
    __as("M=D");
    // that
    __as("@3010");
    __as("D=A");
    __as(__THAT);
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
    FILE_NAME = getFileName();

    COMMENT(getBuff())
    switch (COMMAND_TYPE)
    {
    case C_ARITHEMATIC:
        __writeArithematic();
        break;
    case C_PUSH:
        __push();
        break;
    case C_POP:
        __pop();
        break;
    case C_LABEL:
        __write_label();
        break;
    case C_GOTO:
        __write_goto();
        break;
    case C_IF:
        __write_if();
        break;
    case C_FUNCTION:
        __write_function();
        break;
    case C_RETURN:
        __write_return();
        break;
    case C_CALL:
        __write_call();
        break;
    case UNDEFINED_COMMAND_TYPE:
        fprintf(stderr, "Unkown Type \n");
        __exit(0);
    }
}

void __write_label()
{
    char label[STRING_SIZE] = {};
    strcpy(label, ARG1);
    __gen_label(label, 0);
    __label(label);
}

void __write_goto()
{
    char label[STRING_SIZE] = {};
    strcpy(label, ARG1);
    __gen_label(label, 0);
    __load_label(label);
    __as("0;JMP");
}

void __write_if()
{
    __as(__SP);
    __as("A=M-1");
    __as("D=M");
    DECREASE_STACK
    char label[STRING_SIZE] = {};
    strcpy(label, ARG1);
    __gen_label(label, 0);
    __load_label(label);
    __as("D;JNE");
}

void __write_function()
{
    memset(FUNCTION_NAME, 0, STRING_SIZE);
    strcpy(FUNCTION_NAME, ARG1);
    __label(FUNCTION_NAME);
    char c = 1;
    while (c <= ARG2)
    {
        __load_num(c - 1);
        __as("D=A");
        __as(__LOL);
        __as("A=M+D");
        __as("M=0");
        c++;
    }
}

void __write_call()
{
    // push return
    COMMENT("generate return address")
    char label[STRING_SIZE] = "ret";
    __gen_label(label, RETURN_COUNT);
    __load_label(label);
    RETURN_COUNT++;
    __as("D=A");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
    COMMENT("saved local");
    // saved loc
    __as(__LOL);
    __as("D=M");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
    COMMENT("saved argument")
    // saved arg
    __as(__ARG);
    __as("D=M");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
    COMMENT("saved this")
    // saved this
    __as(__THIS);
    __as("D=M");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
    COMMENT("saved that")
    // saved that
    __as(__THAT);
    __as("D=M");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
    COMMENT("reposition argument")
    __load_num(ARG2 + 5);
    __as("D=A");
    __as(__SP);
    __as("D=M-D");
    __as(__ARG);
    __as("M=D");

    COMMENT("saved stack to local")
    // saved stack to local
    __as(__SP);
    __as("D=M");
    __as(__LOL);
    __as("M=D");
    COMMENT("call the function")
    // go to function call
    __load_label(ARG1);
    __as("0;JMP");
    COMMENT("return address")
    __label(label);
}

void __write_return()
{
    char frame_pointer[STRING_SIZE] = "frame_pointer";
    __gen_label(frame_pointer, 0);
    char retAddr[STRING_SIZE] = "retAddr";
    __gen_label(retAddr, 0);

    COMMENT("saved frame pointer to temp variable")
    __as(__LOL);
    __as("D=M");
    __load_label(frame_pointer);
    __as("M=D");

    COMMENT("saved return address to temp variable")
    __as("@5");
    __as("D=A");
    __load_label(frame_pointer);
    __as("A=M-D");
    __as("D=M");
    __load_label(retAddr);
    __as("M=D");
    // study here
    COMMENT("save return value")
    __as(__SP);
    __as("A=M-1");
    __as("D=M");
    __as(__ARG);
    __as("A=M");
    __as("M=D");

    COMMENT("restore stack pointer")
    __as(__ARG);
    __as("D=M+1");
    __as(__SP);
    __as("M=D");

    COMMENT("restore that pointer")
    __load_label(frame_pointer);
    __as("A=M-1");
    __as("D=M");
    __as(__THAT);
    __as("M=D");

    COMMENT("restore this pointer")
    __as("@2");
    __as("D=A");
    __load_label(frame_pointer);
    __as("A=M-D");
    __as("D=M");
    __as(__THIS);
    __as("M=D");

    COMMENT("restore arg pointer")
    __as("@3");
    __as("D=A");
    __load_label(frame_pointer);
    __as("A=M-D");
    __as("D=M");
    __as(__ARG);
    __as("M=D");

    COMMENT("restore lcl pointer")
    __as("@4");
    __as("D=A");
    __load_label(frame_pointer);
    __as("A=M-D");
    __as("D=M");
    __as(__LOL);
    __as("M=D");

    COMMENT("return")
    __load_label(retAddr);
    __as("A=M");
    __as("0;JMP");
}

void __writeArithematic()
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

void __bin_op()
{
    __as(__SP);
    __as("A=M-1");
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
    default:
        fprintf(stderr, "Unknown Binary Operation\n");
        __exit(0);
    }
    __as("M=D");
    DECREASE_STACK
}

void __comp_op()
{
    char nm_loc[STRING_SIZE] = "comp_";
    __gen_label(nm_loc, COMP_COUNT);
    char next_loc[STRING_SIZE] = "next_";
    __gen_label(next_loc, COMP_COUNT);
    COMP_COUNT++;
    __as(__SP);
    __as("A=M-1");
    __as("D=M");
    __as("A=A-1");
    __as("D=M-D");
    DECREASE_STACK
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
    default:
        fprintf(stderr, "Unknown Comparison Operation\n");
        __exit(0);
    }
    __as(__SP);
    __as("A=M-1");
    __as("M=-1");
    __load_label(next_loc);
    __as("0;JMP");
    __label(nm_loc);
    __as(__SP);
    __as("A=M-1");
    __as("M=0");
    __label(next_loc);
}

void __un_op()
{
    __as(__SP);
    __as("A=M-1");
    switch (OPERATION)
    {
    case NEG:
        __as("D=-M");
        break;
    case NOT:
        __as("D=!M");
        break;
    default:
        fprintf(stderr, "Unknown Unary Operation\n");
        __exit(0);
    }
    __as("M=D");
}

void __push()
{

    switch (SEG)
    {
    case CONSTANT:
        __push_constant(ARG2);
        break;
    case POINTER:
        __push_pointer(ARG2);
        break;
    case THIS:
        __push_this(ARG2);
        break;
    case THAT:
        __push_that(ARG2);
        break;
    case LOCAL:
        __push_local(ARG2);
        break;
    case ARGUMENT:
        __push_argument(ARG2);
        break;
    case TEMP:
        __push_temp(ARG2, TEMP_POINTER);
        break;
    case STATIC:
        __push_static(ARG2);
        break;
    default:
        fprintf(stderr, "Unmatch Segment Type \n");
        __exit(0);
    }
}

void __push_local(INDEX_TYPE in)
{
    char f[STRING_SIZE] = "local";
    __gen_label(f, in);
    __load_label(f);
    __as("D=M");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
}

void __push_constant(INDEX_TYPE in)
{
    __load_num(in);
    __as("D=A");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
}

void __push_this(INDEX_TYPE in)
{
    __as(__THIS);
    __as("D=M");
    __push_memory(in);
}

void __push_that(INDEX_TYPE in)
{
    __as(__THAT);
    __as("D=M");
    __push_memory(in);
}

void __push_argument(INDEX_TYPE in)
{
    __as(__ARG);
    __as("D=M");
    __push_memory(in);
}

void __push_temp(INDEX_TYPE in, INDEX_TYPE temp_pointer)
{
    __load_num(temp_pointer);
    __as("D=A");
    __push_memory(in);
}

void __push_static(INDEX_TYPE in)
{
    char f[STRING_SIZE] = {};
    strcpy(f, FILE_NAME);
    strcat(f, ".static.");
    __int_to_str(in);
    strcat(f, INT_TO_STR);
    __load_label(f);
    __as("D=M");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
}

void __push_pointer(INDEX_TYPE in)
{
    (in) ? __as(__THAT) : __as(__THIS);
    __as("D=M");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
}

void __push_memory(INDEX_TYPE in)
{
    __load_num(in);
    __as("A=D+A");
    __as("D=M");
    __as(__SP);
    __as("A=M");
    __as("M=D");
    INCREASE_STACK
}

void __pop()
{

    switch (SEG)
    {
    case POINTER:
        __pop_pointer(ARG2);
        break;
    case THIS:
        __pop_this(ARG2);
        break;
    case THAT:
        __pop_that(ARG2);
        break;
    case LOCAL:
        __pop_local(ARG2);
        break;
    case ARGUMENT:
        __pop_argument(ARG2);
        break;
    case TEMP:
        __pop_temp(ARG2, TEMP_POINTER);
        break;
    case STATIC:
        __pop_static(ARG2);
        break;
    default:
        fprintf(stderr, "Unmatch Segment Type \n");
        __exit(0);
    }
}

void __pop_local(INDEX_TYPE in)
{
    char f[STRING_SIZE] = "local";
    __gen_label(f, in);
    __as(__SP);
    __as("A=M-1");
    __as("D=M");
    __load_label(f);
    __as("M=D");
    DECREASE_STACK
}

void __pop_this(INDEX_TYPE in)
{
    __as(__THIS);
    __as("D=M");
    __pop_memory(in);
}

void __pop_that(INDEX_TYPE in)
{
    __as(__THAT);
    __as("D=M");
    __pop_memory(in);
}

void __pop_argument(INDEX_TYPE in)
{
    __as(__ARG);
    __as("D=M");
    __pop_memory(in);
}

void __pop_temp(INDEX_TYPE in, INDEX_TYPE temp_pointer)
{
    __load_num(temp_pointer);
    __as("D=A");
    __pop_memory(in);
}

void __pop_static(INDEX_TYPE in)
{
    char f[STRING_SIZE] = {};
    strcpy(f, FILE_NAME);
    strcat(f, ".static.");
    __int_to_str(in);
    strcat(f, INT_TO_STR);
    __load_label(f);
    __as(__SP);
    __as("A=M-1");
    __as("D=M");
    __load_label(f);
    __as("M=D");
    DECREASE_STACK
}

void __pop_pointer(INDEX_TYPE in)
{
    __as(__SP);
    __as("A=M-1");
    __as("D=M");
    (in) ? __as(__THAT) : __as(__THIS);
    __as("M=D");
    DECREASE_STACK
}

void __load_num(INDEX_TYPE in)
{
    __int_to_str(in);
    __wb("@");
    __wb(INT_TO_STR);
    __wb("\n");
}

void __pop_memory(INDEX_TYPE in)
{
    // store to R14
    __load_num(in);
    __as("D=D+A");
    __as("@R14");
    __as("M=D");
    // wirte to mem
    __as(__SP);
    __as("A=M-1");
    __as("D=M");
    __as("@R14");
    __as("A=M");
    __as("M=D");
    DECREASE_STACK
}

void __as(const char *c)
{
    __wb(c);
    OUTPUT_BUFFER[OUTPUT_COUNT] = '\n';
    OUTPUT_COUNT++;
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
}

void __gen_label(char *label, unsigned int num)
{
    if ((strlen(FUNCTION_NAME) + strlen(label)) >= STRING_SIZE)
    {
        fprintf(stderr, "gen_label string size overflow\n");
        __exit(0);
    }
    char tmp[STRING_SIZE] = {};
    strcpy(tmp, FUNCTION_NAME);
    strcat(tmp, "$");
    strcat(tmp, label);

    if (num)
    {
        strcat(tmp, ".");
        __int_to_str(num);
        strcat(tmp, INT_TO_STR);
    }
    memset(label, 0, STRING_SIZE);
    strcpy(label, tmp);
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
    while (c && *c != '\0')
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

static void __exit(int type)
{
    clean();
    exit(type);
}