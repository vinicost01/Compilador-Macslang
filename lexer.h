#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOK_EOF,
    TOK_IDENTIFIER,
    TOK_INT,
    TOK_STRING,
    TOK_TRUE, TOK_FALSE,
    TOK_VAR, TOK_FUNC, TOK_IF, TOK_ELSE, TOK_WHILE, TOK_FOR, TOK_PRINT, TOK_INPUT, TOK_RETURN,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_COLON, TOK_COMMA, TOK_SEMI, TOK_ASSIGN,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_EQ, TOK_NEQ, TOK_LT, TOK_LTE, TOK_GT, TOK_GTE
} TokenType;

typedef struct {
    TokenType type;
    char text[256];
    int int_value;
} Token;

Token get_next_token(void);
void init_lexer(const char *src);

#endif