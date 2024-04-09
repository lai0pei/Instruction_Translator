#include "../common.h"
#include "CodeWrite.h"
#define STRING_SIZE 10
#define OUTPUT "out.asm"

static STACK_TYPE STACK_POINTER = 256;
static TEMP_TYPE TEMP_POINTER = 5;
static STATIC_TYPE STATIC_POINTER = 16;
static unsigned int M_C = 0;
static FILE *FP;
static const char LOC[3] = "nm_";

static void __file_constructor(void);
static void __pop(SEG_TYPE, INDEX_TYPE);
static void __push(SEG_TYPE, INDEX_TYPE);
static void __load(INDEX_TYPE);
static void __as(char *);
static void __push_constant(INDEX_TYPE);
static void __push_pointer(INDEX_TYPE);
static void __push_memory(INDEX_TYPE);
static void __pop_memory(INDEX_TYPE);
static void __pop_pointer(INDEX_TYPE);
static void __bin_op(OP_TYPE);
static void __un_op(OP_TYPE);
static void __comp_op(OP_TYPE);
static void __write_comment(void);
static void __increase_stack(void);
static void __decrease_stack(void);
static void __int_to_str(char *, unsigned int);
static void __str_cat(char *, char *);
static void __load_loc();
static void __gen_loc();

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
}

void code_clean(void)
{
    fclose(FP);
}

void writeArithematic(OP_TYPE Op)
{
    // binary
    if (Op == ADD || Op == SUB || Op == OR || Op == AND)
    {
        __bin_op(Op);
    }

    if (Op == NOT || Op == NEG)
    {
        __un_op(Op);
    }

    if (Op == EQ || Op == GT || Op == LT)
    {
        __comp_op(Op);
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
#if DEBUG == 1
    if (Op != UNDEFINED_OPTYPE)
    {
        __write_comment();
    }
#endif
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("D=M");
    __as("A=A-1");
    switch (Op)
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

void __comp_op(OP_TYPE Op)
{
#if DEBUG == 1
    if (Op != UNDEFINED_OPTYPE)
    {
        __write_comment();
    }
#endif
    char nm_loc[STRING_SIZE] = {};
    char lod_nm[STRING_SIZE] = {};
    __gen_loc(lod_nm, nm_loc);

    char next_loc[STRING_SIZE] = {};
    char lod_next[STRING_SIZE] = {};
    __gen_loc(lod_next, next_loc);
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("D=M");
    __as("A=A-1");
    __as("D=M-D");
    __as(lod_nm);
    switch (Op)
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
    __as(lod_next);
    __as("0;JMP");
    __as(nm_loc);
    __as("@R0");
    __as("A=M");
    __as("A=A-1");
    __as("A=A-1");
    __as("M=0");
    __as(next_loc);
    __as("@R0");
    __as("M=M-1");
}

void __un_op(OP_TYPE Op)
{
#if DEBUG == 1
    if (Op != UNDEFINED_OPTYPE)
    {
        __write_comment();
    }
#endif
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

    switch (segment)
    {
    case CONSTANT:
        __push_constant(index);
        break;
    case POINTER:
        __push_pointer(index);
        break;
    case THIS:
        __as("@R3");
        __as("D=M");
        __push_memory(index);
        break;
    case THAT:
        __as("@R4");
        __as("D=M");
        __push_memory(index);
        break;
    case LOCAL:
        __as("@R1");
        __as("D=M");
        __push_memory(index);
        break;
    case ARGUMENT:
        __as("@R2");
        __as("D=M");
        __push_memory(index);
        break;
    case TEMP:
        __load(TEMP_POINTER);
        __as("D=A");
        __push_memory(index);
        break;

    case STATIC:
        __load(STATIC_POINTER);
        __as("D=A");
        __push_memory(index);
        break;
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

void __push_pointer(INDEX_TYPE index)
{
    if (index == 0)
    {
        __as("@R3");
    }

    if (index == 1)
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

void __push_memory(INDEX_TYPE index)
{
    __load(index);
    __as("A=D+A");
    __as("D=M");
    __as("@R0");
    __as("A=M");
    __as("M=D");
    __increase_stack();
}

void __pop(SEG_TYPE segment, INDEX_TYPE index)
{

    switch (segment)
    {
    case POINTER:
        __pop_pointer(index);
        break;
    case THIS:
        __as("@R3");
        __as("D=M");
        __pop_memory(index);
        break;
    case THAT:
        __as("@R4");
        __as("D=M");
        __pop_memory(index);
        break;
    case LOCAL:
        __as("@R1");
        __as("D=M");
        __pop_memory(index);
        break;
    case ARGUMENT:
        __as("@R2");
        __as("D=M");
        __pop_memory(index);
        break;
    case TEMP:
        __load(TEMP_POINTER);
        __as("D=A");
        __pop_memory(index);
        break;
    case STATIC:
        __load(STATIC_POINTER);
        __as("D=A");
        __pop_memory(index);
        break;
    }
}

void __pop_memory(INDEX_TYPE index)
{
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
    __as("@R14");
    __as("A=M");
    __as("M=D");
    __decrease_stack();
}

void __pop_pointer(INDEX_TYPE index)
{
     __as("@R0");
     __as("A=M");
     __as("A=A-1");
     __as("D=M");
    if (index == 0)
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

void __load(INDEX_TYPE in)
{
    char val[STRING_SIZE] = {};
    __int_to_str(val, in);
    fwrite("@", 1, 1, FP);
    fwrite(val, STRING_SIZE, 1, FP);
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

void __str_cat(char *d, char *s)
{
    char di = 0;
    char si = 0;

    while (d[di] != '\0')
    {
        di++;
    }

    while (di < STRING_SIZE)
    {
        d[di] = s[si];
        di++;
        si++;
    }
}

void __int_to_str(char *c, unsigned int in)
{
    char val[STRING_SIZE] = {};
    memset(val, ' ', STRING_SIZE);
    sprintf(val, "%d", in);
    char t = 0;
    while (t < STRING_SIZE)
    {
        if (val[t] == '\0')
        {
            val[t] = ' ';
        }
        t++;
    }
    strcpy(c, val);
}

void __gen_loc(char *a_inst, char *l_inst)
{
    char num[STRING_SIZE] = {};
    __int_to_str(num, M_C);
    char a_instruction[STRING_SIZE] = {};
    char l_instruction[STRING_SIZE] = {};
    char t = 0;
    while (t < STRING_SIZE)
    {
        if (t == 0)
        {
            a_instruction[t] = '@';
            l_instruction[t] = '(';
            t++;
            continue;
        }

        if (t > 4 && num[t - 4] == ' ')
        {
            l_instruction[t] = ')';
            break;
        }

        if (t < 4)
        {
            a_instruction[t] = LOC[t - 1];
            l_instruction[t] = LOC[t - 1];
        }
        else
        {
            a_instruction[t] = num[t - 4];
            l_instruction[t] = num[t - 4];
        }

        t++;
    }
    strcpy(a_inst, a_instruction);
    strcpy(l_inst, l_instruction);
    M_C++;
}
