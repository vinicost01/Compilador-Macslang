#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "parser.h"

typedef enum {
    VAL_INT,
    VAL_STRING,
    VAL_BOOL,
    VAL_NONE
} ValueType;

typedef struct {
    ValueType type;
    union {
        int int_val;
        char *str_val;
        int bool_val;
    };
} Value;

void init_vars(void);
void interpret(AST *ast);

#endif