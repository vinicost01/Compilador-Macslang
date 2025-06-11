#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static const char *src;
static int pos;

static Token make_token(TokenType t, const char *text) {
    Token tok = { t };
    if (text) strncpy(tok.text, text, 255);
    return tok;
}

void init_lexer(const char *s) {
    src = s;
    pos = 0;
}

static void skip_ws() {
    while (1) {
        while (isspace(src[pos])) pos++;
        if (src[pos] == '/' && src[pos+1] == '/') {
            pos += 2;
            while (src[pos] && src[pos] != '\n') pos++;
        } else {
            break;
        }
    }
}

static int is_id_start(char c) {
    return isalpha(c) || c == '_';
}

static int is_id_char(char c) {
    return isalnum(c) || c == '_';
}

static Token read_number() {
    int start = pos;
    while (isdigit(src[pos])) pos++;
    Token t = make_token(TOK_INT, NULL);
    t.int_value = atoi(&src[start]);
    return t;
}

static Token read_identifier() {
    int start = pos;
    while (is_id_char(src[pos])) pos++;
    int len = pos - start;
    char buf[64] = {0};
    strncpy(buf, &src[start], len);

    if (strcmp(buf, "var") == 0)      return make_token(TOK_VAR, buf);
    if (strcmp(buf, "func") == 0)     return make_token(TOK_FUNC, buf);
    if (strcmp(buf, "if") == 0)       return make_token(TOK_IF, buf);
    if (strcmp(buf, "else") == 0)     return make_token(TOK_ELSE, buf);
    if (strcmp(buf, "while") == 0)    return make_token(TOK_WHILE, buf);
    if (strcmp(buf, "for") == 0)      return make_token(TOK_FOR, buf);
    if (strcmp(buf, "print") == 0)    return make_token(TOK_PRINT, buf);
    if (strcmp(buf, "input") == 0)    return make_token(TOK_INPUT, buf);
    if (strcmp(buf, "return") == 0)   return make_token(TOK_RETURN, buf);
    if (strcmp(buf, "true") == 0)     return make_token(TOK_TRUE, buf);
    if (strcmp(buf, "false") == 0)    return make_token(TOK_FALSE, buf);

    Token t = make_token(TOK_IDENTIFIER, buf);
    return t;
}

static Token read_string() {
    pos++; 
    char buf[256] = {0};
    int i = 0;
    while (src[pos] && src[pos] != '"' && i < 255) {
        if (src[pos] == '\\' && src[pos+1] == 'n') {
            buf[i++] = '\n';
            pos += 2;
        } else {
            buf[i++] = src[pos++];
        }
    }
    if (src[pos] == '"') pos++;
    Token t = make_token(TOK_STRING, buf);
    return t;
}

Token get_next_token(void) {
    skip_ws();
    char c = src[pos];
    if (!c) return make_token(TOK_EOF, NULL);

    if (isdigit(c)) return read_number();

    if (is_id_start(c)) return read_identifier();

    if (c == '"') return read_string();

    if (c == '(') { pos++; return make_token(TOK_LPAREN, "("); }
    if (c == ')') { pos++; return make_token(TOK_RPAREN, ")"); }
    if (c == '{') { pos++; return make_token(TOK_LBRACE, "{"); }
    if (c == '}') { pos++; return make_token(TOK_RBRACE, "}"); }
    if (c == ':') { pos++; return make_token(TOK_COLON, ":"); }
    if (c == ',') { pos++; return make_token(TOK_COMMA, ","); }
    if (c == ';') { pos++; return make_token(TOK_SEMI, ";"); }
    if (c == '+') { pos++; return make_token(TOK_PLUS, "+"); }
    if (c == '-') { pos++; return make_token(TOK_MINUS, "-"); }
    if (c == '*') { pos++; return make_token(TOK_STAR, "*"); }
    if (c == '/') { pos++; return make_token(TOK_SLASH, "/"); }
    if (c == '%') { pos++; return make_token(TOK_PERCENT, "%"); }
    if (c == '=') {
        if (src[pos+1] == '=') { pos+=2; return make_token(TOK_EQ, "=="); }
        pos++; return make_token(TOK_ASSIGN, "=");
    }
    if (c == '!') {
        if (src[pos+1] == '=') { pos+=2; return make_token(TOK_NEQ, "!="); }
    }
    if (c == '<') {
        if (src[pos+1] == '=') { pos+=2; return make_token(TOK_LTE, "<="); }
        pos++; return make_token(TOK_LT, "<");
    }
    if (c == '>') {
        if (src[pos+1] == '=') { pos+=2; return make_token(TOK_GTE, ">="); }
        pos++; return make_token(TOK_GT, ">");
    }
    printf("Unknown character: %c\n", c);
    exit(1);
}