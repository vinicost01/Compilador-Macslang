#include "parser.h"
#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static Token current_token;

static AST* make_ast(ASTType type) {
    AST* ast = calloc(1, sizeof(AST));
    ast->type = type;
    return ast;
}

static AST* parse_statement();
static AST* parse_block();
static AST* parse_expr();
static AST* parse_primary();
static AST* parse_assignment();
static AST* parse_assignment_inline();

static void next() { current_token = get_next_token(); }
static int accept(TokenType t) { if (current_token.type == t) { next(); return 1; } return 0; }
static void expect(TokenType t) { if (!accept(t)) { printf("Syntax error: expected %d\n", t); exit(1); } }

void free_ast(AST *ast) {
    if (!ast) return;
    if (ast->left) free_ast(ast->left);
    if (ast->right) free_ast(ast->right);
    if (ast->cond) free_ast(ast->cond);
    if (ast->then_body) free_ast(ast->then_body);
    if (ast->else_body) free_ast(ast->else_body);
    if (ast->body) free_ast(ast->body);
    if (ast->init) free_ast(ast->init);
    if (ast->incr) free_ast(ast->incr);
    if (ast->block) free_ast(ast->block);
    if (ast->params_list) free_ast(ast->params_list);
    if (ast->children) {
        for (int i = 0; i < ast->children_count; i++)
            free_ast(ast->children[i]);
        free(ast->children);
    }
    if (ast->left_return) free_ast(ast->left_return);
    free(ast);
}

static AST* parse_program_node() {
    AST* prog = make_ast(AST_PROGRAM);
    prog->children = NULL;
    prog->children_count = 0;
    while (current_token.type != TOK_EOF) {
        AST* stmt = parse_statement();
        prog->children = realloc(prog->children, sizeof(AST*) * (prog->children_count + 1));
        prog->children[prog->children_count++] = stmt;
    }
    return prog;
}

static AST* parse_func_decl() {
    expect(TOK_FUNC);
    AST* ast = make_ast(AST_FUNC_DECL);
    if (current_token.type != TOK_IDENTIFIER) { printf("Expected function name\n"); exit(1); }
    strcpy(ast->name, current_token.text);
    next();
    expect(TOK_LPAREN);
    int pcount = 0;
    if (current_token.type != TOK_RPAREN) {
        do {
            if (current_token.type != TOK_IDENTIFIER) { printf("Expected parameter name\n"); exit(1); }
            strcpy(ast->params[pcount].name, current_token.text);
            next();
            expect(TOK_COLON);
            if (current_token.type != TOK_IDENTIFIER) { printf("Expected parameter type\n"); exit(1); }
            strcpy(ast->params[pcount].type, current_token.text);
            next();
            pcount++;
        } while (accept(TOK_COMMA));
    }
    expect(TOK_RPAREN);
    expect(TOK_COLON);
    if (current_token.type != TOK_IDENTIFIER) { printf("Expected return type\n"); exit(1); }
    strcpy(ast->str_value, current_token.text); // tipo de retorno
    next();
    ast->params_count = pcount;
    ast->body = parse_block();
    return ast;
}

static AST* parse_block() {
    expect(TOK_LBRACE);
    AST* block = make_ast(AST_PROGRAM);
    block->children = NULL;
    block->children_count = 0;
    while (current_token.type != TOK_RBRACE) {
        AST* stmt = parse_statement();
        block->children = realloc(block->children, sizeof(AST*) * (block->children_count + 1));
        block->children[block->children_count++] = stmt;
    }
    expect(TOK_RBRACE);
    return block;
}


static AST* parse_var_decl() {
    expect(TOK_VAR);
    AST* ast = make_ast(AST_VAR_DECL);
    if (current_token.type != TOK_IDENTIFIER) { printf("Expected variable name\n"); exit(1); }
    strcpy(ast->name, current_token.text);
    next();
    expect(TOK_COLON);
    if (current_token.type != TOK_IDENTIFIER) { printf("Expected type\n"); exit(1); }
    if (
        strcmp(current_token.text, "int") == 0 ||
        strcmp(current_token.text, "string") == 0 ||
        strcmp(current_token.text, "bool") == 0
    ) {
        strcpy(ast->str_value, current_token.text);
    } else {
        printf("Unknown type: %s\n", current_token.text);
        exit(1);
    }
    next();
    if (accept(TOK_ASSIGN)) {
        ast->left = parse_expr();
    }
    expect(TOK_SEMI);
    return ast;
}

static AST* parse_statement() {
    if (current_token.type == TOK_VAR)
        return parse_var_decl();
    if (current_token.type == TOK_FUNC)
        return parse_func_decl();
    if (current_token.type == TOK_IDENTIFIER) {
        char tmpname[64];
        strcpy(tmpname, current_token.text);
        next();
        if (current_token.type == TOK_ASSIGN) {
            AST* ast = make_ast(AST_ASSIGN);
            strcpy(ast->name, tmpname);
            expect(TOK_ASSIGN);
            ast->left = parse_expr();
            expect(TOK_SEMI);
            return ast;
        } else if (current_token.type == TOK_LPAREN) {
            AST* ast = make_ast(AST_FUNC_CALL);
            strcpy(ast->name, tmpname);
            next();
            ast->children = NULL;
            ast->children_count = 0;
            if (current_token.type != TOK_RPAREN) {
                do {
                    AST* arg = parse_expr();
                    ast->children = realloc(ast->children, sizeof(AST*) * (ast->children_count + 1));
                    ast->children[ast->children_count++] = arg;
                } while (accept(TOK_COMMA));
            }
            expect(TOK_RPAREN);
            expect(TOK_SEMI);
            return ast;
        } else {
            printf("Syntax error after identifier\n");
            exit(1);
        }
    }
    if (current_token.type == TOK_PRINT) {
        AST* ast = make_ast(AST_PRINT);
        next();
        expect(TOK_LPAREN);
        ast->left = parse_expr();
        expect(TOK_RPAREN);
        expect(TOK_SEMI);
        return ast;
    }
    if (current_token.type == TOK_INPUT) {
        AST* ast = make_ast(AST_INPUT);
        next();
        expect(TOK_LPAREN);
        if (current_token.type != TOK_IDENTIFIER) { printf("Expected variable name for input\n"); exit(1); }
        strcpy(ast->name, current_token.text);
        next();
        expect(TOK_RPAREN);
        expect(TOK_SEMI);
        return ast;
    }
    if (current_token.type == TOK_IF) {
        AST* ast = make_ast(AST_IF);
        next();
        expect(TOK_LPAREN);
        ast->cond = parse_expr();
        expect(TOK_RPAREN);
        ast->then_body = parse_block();
        if (accept(TOK_ELSE)) {
            ast->else_body = parse_block();
        }
        return ast;
    }
    if (current_token.type == TOK_WHILE) {
        AST* ast = make_ast(AST_WHILE);
        next();
        expect(TOK_LPAREN);
        ast->cond = parse_expr();
        expect(TOK_RPAREN);
        ast->body = parse_block();
        return ast;
    }
    if (current_token.type == TOK_FOR) {
        AST* ast = make_ast(AST_FOR);
        next();
        expect(TOK_LPAREN);
        if (current_token.type == TOK_VAR) {
            ast->init = parse_var_decl();
        } else if (current_token.type == TOK_IDENTIFIER) {
            ast->init = parse_assignment_inline();
            expect(TOK_SEMI);
        } else {
            printf("Syntax error in for-init\n");
            exit(1);
        }

        ast->cond = parse_expr();
        expect(TOK_SEMI);

        ast->incr = parse_assignment_inline();
        expect(TOK_RPAREN);

        ast->body = parse_block();
        return ast;
    }
    if (current_token.type == TOK_RETURN) {
        AST* ast = make_ast(AST_RETURN);
        next();
        ast->left = parse_expr();
        expect(TOK_SEMI);
        return ast;
    }
    printf("Syntax error: unknown statement\n");
    exit(1);
}

static AST* parse_primary() {
    if (current_token.type == TOK_INT) {
        AST* ast = make_ast(AST_LITERAL);
        ast->int_value = current_token.int_value;
        next();
        return ast;
    }
    if (current_token.type == TOK_STRING) {
        AST* ast = make_ast(AST_LITERAL);
        strcpy(ast->str_value, current_token.text);
        next();
        return ast;
    }
    if (current_token.type == TOK_TRUE || current_token.type == TOK_FALSE) {
        AST* ast = make_ast(AST_LITERAL);
        ast->int_value = (current_token.type == TOK_TRUE);
        next();
        return ast;
    }
    if (current_token.type == TOK_IDENTIFIER) {
        char tmpname[64];
        strcpy(tmpname, current_token.text);
        next();
        if (current_token.type == TOK_LPAREN) {
            AST* ast = make_ast(AST_FUNC_CALL);
            strcpy(ast->name, tmpname);
            next();
            ast->children = NULL;
            ast->children_count = 0;
            if (current_token.type != TOK_RPAREN) {
                do {
                    AST* arg = parse_expr();
                    ast->children = realloc(ast->children, sizeof(AST*) * (ast->children_count + 1));
                    ast->children[ast->children_count++] = arg;
                } while (accept(TOK_COMMA));
            }
            expect(TOK_RPAREN);
            return ast;
        } else {
            AST* ast = make_ast(AST_IDENTIFIER);
            strcpy(ast->name, tmpname);
            return ast;
        }
    }
    if (accept(TOK_LPAREN)) {
        AST* e = parse_expr();
        expect(TOK_RPAREN);
        return e;
    }
    printf("Syntax error: expected expression\n");
    exit(1);
}

static int is_binop(TokenType t) {
    return t == TOK_PLUS || t == TOK_MINUS || t == TOK_STAR || t == TOK_SLASH ||
           t == TOK_PERCENT || t == TOK_EQ || t == TOK_NEQ ||
           t == TOK_LT || t == TOK_LTE || t == TOK_GT || t == TOK_GTE;
}

static int precedence(TokenType t) {
    switch (t) {
        case TOK_PLUS: case TOK_MINUS: return 1;
        case TOK_STAR: case TOK_SLASH: case TOK_PERCENT: return 2;
        case TOK_EQ: case TOK_NEQ: case TOK_LT: case TOK_LTE: case TOK_GT: case TOK_GTE: return 0;
        default: return -1;
    }
}

static AST* parse_binop_rhs(int min_prec, AST* lhs) {
    while (is_binop(current_token.type) && precedence(current_token.type) >= min_prec) {
        TokenType op = current_token.type;
        char opname[3] = {0};
        switch (op) {
            case TOK_PLUS: strcpy(opname, "+"); break;
            case TOK_MINUS: strcpy(opname, "-"); break;
            case TOK_STAR: strcpy(opname, "*"); break;
            case TOK_SLASH: strcpy(opname, "/"); break;
            case TOK_PERCENT: strcpy(opname, "%"); break;
            case TOK_EQ: strcpy(opname, "=="); break;
            case TOK_NEQ: strcpy(opname, "!="); break;
            case TOK_LT: strcpy(opname, "<"); break;
            case TOK_LTE: strcpy(opname, "<="); break;
            case TOK_GT: strcpy(opname, ">"); break;
            case TOK_GTE: strcpy(opname, ">="); break;
            default: opname[0] = 0;
        }
        next();
        AST* rhs = parse_primary();
        while (is_binop(current_token.type) &&
               precedence(current_token.type) > precedence(op)) {
            rhs = parse_binop_rhs(precedence(current_token.type), rhs);
        }
        AST* ast = make_ast(AST_BINOP);
        strcpy(ast->name, opname);
        ast->left = lhs;
        ast->right = rhs;
        lhs = ast;
    }
    return lhs;
}

static AST* parse_expr() {
    AST* lhs = parse_primary();
    return parse_binop_rhs(0, lhs);
}

static AST* parse_assignment_inline() {
    AST* ast = make_ast(AST_ASSIGN);
    if (current_token.type != TOK_IDENTIFIER) { printf("Expected variable name for assignment\n"); exit(1); }
    strcpy(ast->name, current_token.text);
    next();
    expect(TOK_ASSIGN);
    ast->left = parse_expr();
    return ast;
}

static AST* parse_assignment() {
    AST* ast = parse_assignment_inline();
    expect(TOK_SEMI);
    return ast;
}

void init_lexer(const char *src); 

AST* parse_program(void) {
    next();
    return parse_program_node();
}

