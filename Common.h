#ifndef COMMON
#define COMMON
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#define DEBUG 1
typedef enum
{
    C_ARITHEMATIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
    UNDEFINED_COMMAND_TYPE
} CommandType;
typedef enum
{
    PUSH,
    POP,
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT,
    FN,
    LABEL,
    CALL,
    RETURN,
    GOTO,
    IF_GOTO,
    UNDEFINED_OPTYPE
} OP_TYPE;

typedef enum
{
    CONSTANT,
    LOCAL,
    ARGUMENT,
    TEMP,
    THIS,
    THAT,
    POINTER,
    STATIC,
    UNDEFINED_SEG_TYPE
} SEG_TYPE;
void mem_free(void *);
void file_free(void *);
#endif