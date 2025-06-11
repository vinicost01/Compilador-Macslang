#ifndef PARSER_H
#define PARSER_H

typedef enum {
    AST_PROGRAM,
    AST_VAR_DECL,
    AST_ASSIGN,
    AST_FUNC_DECL,
    AST_FUNC_CALL,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_PRINT,
    AST_INPUT,
    AST_RETURN,
    AST_BINOP,
    AST_LITERAL,
    AST_IDENTIFIER
} ASTType;

typedef struct {
    char name[64];
    char type[16];
} Param;

typedef struct AST {
    ASTType type;
    char name[64];
    char str_value[256];    
    int int_value;          
    int children_count;
    struct AST **children;  
    struct AST *left, *right;
    struct AST *cond, *then_body, *else_body, *body, *init, *incr;
    struct AST *params_list;
    int params_count;
    Param params[8];
    struct AST *block; 
    struct AST *left_return;
} AST;

void init_lexer(const char *src);
AST* parse_program(void);
void free_ast(AST *ast);

#endif