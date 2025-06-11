#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void exec(AST *ast);

typedef struct Var {
    char name[64];
    Value value;
    struct Var *next;
} Var;

typedef struct Scope {
    Var *vars;
    struct Scope *prev;
} Scope;

static Scope *current_scope = NULL;

void init_vars(void) {
    while (current_scope) {
        Var *v = current_scope->vars;
        while (v) {
            if (v->value.type == VAL_STRING && v->value.str_val)
                free(v->value.str_val);
            Var *next = v->next;
            free(v);
            v = next;
        }
        Scope *prev = current_scope->prev;
        free(current_scope);
        current_scope = prev;
    }
    current_scope = malloc(sizeof(Scope));
    current_scope->vars = NULL;
    current_scope->prev = NULL;
}

static void push_scope() {
    Scope *s = malloc(sizeof(Scope));
    s->vars = NULL;
    s->prev = current_scope;
    current_scope = s;
}

static void pop_scope() {
    Var *v = current_scope->vars;
    while (v) {
        if (v->value.type == VAL_STRING && v->value.str_val)
            free(v->value.str_val);
        Var *next = v->next;
        free(v);
        v = next;
    }
    Scope *prev = current_scope->prev;
    free(current_scope);
    current_scope = prev;
}

static Var* find_var(const char *name) {
    Scope *s = current_scope;
    while (s) {
        Var *v = s->vars;
        while (v) {
            if (strcmp(v->name, name) == 0)
                return v;
            v = v->next;
        }
        s = s->prev;
    }
    return NULL;
}

static void set_var(const char *name, Value v) {
    Var *var = find_var(name);
    if (!var) {
        var = calloc(1, sizeof(Var));
        strncpy(var->name, name, 63);
        var->name[63] = '\0';
        var->next = current_scope->vars;
        current_scope->vars = var;
    } else {
        if (var->value.type == VAL_STRING && var->value.str_val)
            free(var->value.str_val);
    }
    var->value = v;
    if (v.type == VAL_STRING && v.str_val)
        var->value.str_val = strdup(v.str_val); 
}

static void assign_var(const char *name, Value v) {
    Var *var = find_var(name);
    if (!var) {
        printf("Undefined variable: %s\n", name);
        exit(1);
    }
    if (var->value.type == VAL_STRING && var->value.str_val)
        free(var->value.str_val);
    var->value = v;
    if (v.type == VAL_STRING && v.str_val)
        var->value.str_val = strdup(v.str_val);
}
typedef struct Func {
    char name[64];
    int param_count;
    char param_names[8][64];
    char param_types[8][16];
    char return_type[16];
    AST* block;
    struct Func* next;
} Func;

static Func* funcs = NULL;

static void add_func(const char* name, int param_count, char param_names[][64], char param_types[][16], const char* ret_type, AST* block) {
    Func* f = calloc(1, sizeof(Func));
    strcpy(f->name, name);
    f->param_count = param_count;
    for (int i = 0; i < param_count; i++) {
        strcpy(f->param_names[i], param_names[i]);
        strcpy(f->param_types[i], param_types[i]);
    }
    strcpy(f->return_type, ret_type);
    f->block = block;
    f->next = funcs;
    funcs = f;
}

static Func* find_func(const char* name) {
    Func* f = funcs;
    while (f) {
        if (strcmp(f->name, name) == 0)
            return f;
        f = f->next;
    }
    return NULL;
}

typedef struct {
    int is_returning;
    Value value;
} ReturnValue;

static ReturnValue ret_val;

static Value eval_expr(AST* ast);

static Value value_int(int v) {
    Value val = { VAL_INT };
    val.int_val = v;
    return val;
}
static Value value_bool(int v) {
    Value val = { VAL_BOOL };
    val.bool_val = !!v;
    return val;
}
static Value value_string(const char* s) {
    Value val = { VAL_STRING };
    val.str_val = s ? strdup(s) : strdup("");
    return val;
}

static Value value_none() {
    Value val = { VAL_NONE };
    return val;
}

static int is_truthy(Value v) {
    if (v.type == VAL_BOOL) return v.bool_val;
    if (v.type == VAL_INT) return v.int_val != 0;
    if (v.type == VAL_STRING) return v.str_val && v.str_val[0] != '\0';
    return 0;
}

static Value eval_expr(AST* ast) {
    if (!ast) return value_none();
    if (ast->type == AST_LITERAL && ast->str_value[0])
        return value_string(ast->str_value);
    if (ast->type == AST_LITERAL)
        return value_int(ast->int_value);
    if (ast->type == AST_IDENTIFIER) {
        Var* v = find_var(ast->name);
        if (!v) { printf("Undefined variable: %s\n", ast->name); exit(1);}
        if (v->value.type == VAL_STRING)
            return value_string(v->value.str_val);
        else if (v->value.type == VAL_INT)
            return value_int(v->value.int_val);
        else if (v->value.type == VAL_BOOL)
            return value_bool(v->value.bool_val);
        return value_none();
    }
    if (ast->type == AST_BINOP) {
        Value left = eval_expr(ast->left);
        Value right = eval_expr(ast->right);
        if ((left.type == VAL_STRING) || (right.type == VAL_STRING)) {
            const char *lstr = (left.type == VAL_STRING) ? left.str_val : "";
            char ltmp[64]; if (left.type == VAL_INT) sprintf(ltmp, "%d", left.int_val);
            char lbool[8]; if (left.type == VAL_BOOL) sprintf(lbool, "%s", left.bool_val ? "true" : "false");
            if (left.type == VAL_INT) lstr = ltmp;
            if (left.type == VAL_BOOL) lstr = lbool;
            const char *rstr = (right.type == VAL_STRING) ? right.str_val : "";
            char rtmp[64]; if (right.type == VAL_INT) sprintf(rtmp, "%d", right.int_val);
            char rbool[8]; if (right.type == VAL_BOOL) sprintf(rbool, "%s", right.bool_val ? "true" : "false");
            if (right.type == VAL_INT) rstr = rtmp;
            if (right.type == VAL_BOOL) rstr = rbool;
            size_t len = strlen(lstr) + strlen(rstr) + 1;
            char *buf = malloc(len);
            strcpy(buf, lstr);
            strcat(buf, rstr);
            if (left.type == VAL_STRING) free(left.str_val);
            if (right.type == VAL_STRING) free(right.str_val);
            Value v = value_string(buf);
            free(buf); 
            return v;
        }
        int l = (left.type == VAL_INT) ? left.int_val : 0;
        int r = (right.type == VAL_INT) ? right.int_val : 0;
        int res = 0;
        if (strcmp(ast->name, "+") == 0) res = l + r;
        else if (strcmp(ast->name, "-") == 0) res = l - r;
        else if (strcmp(ast->name, "*") == 0) res = l * r;
        else if (strcmp(ast->name, "/") == 0) res = r != 0 ? l / r : 0;
        else if (strcmp(ast->name, "%") == 0) res = r != 0 ? l % r : 0;
        else if (strcmp(ast->name, "<") == 0) return value_bool(l < r);
        else if (strcmp(ast->name, "<=") == 0) return value_bool(l <= r);
        else if (strcmp(ast->name, ">") == 0) return value_bool(l > r);
        else if (strcmp(ast->name, ">=") == 0) return value_bool(l >= r);
        else if (strcmp(ast->name, "==") == 0) return value_bool(l == r);
        else if (strcmp(ast->name, "!=") == 0) return value_bool(l != r);
        if (left.type == VAL_STRING) free(left.str_val);
        if (right.type == VAL_STRING) free(right.str_val);
        return value_int(res);
    }
    if (ast->type == AST_FUNC_CALL) {
        Func* f = find_func(ast->name);
        if (!f) { printf("Undefined function: %s\n", ast->name); exit(1); }
        push_scope();
        for (int i = 0; i < f->param_count; i++) {
            Value argval = eval_expr(ast->children[i]);
            set_var(f->param_names[i], argval);
        }
        ret_val.is_returning = 0;
        ret_val.value = value_none();
        for (int i = 0; i < f->block->children_count; i++) {
            exec(f->block->children[i]);
            if (ret_val.is_returning)
                break;
        }
        Value ret = ret_val.value;
        pop_scope();
        return ret;
    }
    return value_none();
}
static void exec(AST *ast);

void interpret(AST *ast) {
    push_scope();
    if (ast && ast->type == AST_PROGRAM) {
        for (int i = 0; i < ast->children_count; i++) {
            AST *stmt = ast->children[i];
            if (stmt->type == AST_FUNC_DECL) {
                int paramc = stmt->params_count;
                char param_names[8][64];
                char param_types[8][16];
                for (int j = 0; j < paramc; j++) {
                    strncpy(param_names[j], stmt->params[j].name, 63);
                    param_names[j][63] = 0;
                    strncpy(param_types[j], stmt->params[j].type, 15);
                    param_types[j][15] = 0;
                }
                add_func(stmt->name, paramc, param_names, param_types, stmt->str_value, stmt->body);
            }
        }
    }
    for (int i = 0; i < ast->children_count; i++) {
        AST *stmt = ast->children[i];
        if (stmt->type != AST_FUNC_DECL)
            exec(stmt);
    }
    pop_scope();
}

static void exec(AST *ast) {
    if (!ast) return;
    switch (ast->type) {
        case AST_PROGRAM:
            for (int i = 0; i < ast->children_count; i++)
                exec(ast->children[i]);
            break;
        case AST_VAR_DECL: {
            Value v = value_none();
            if (ast->left) v = eval_expr(ast->left);
            if (strcmp(ast->str_value, "int") == 0 && v.type != VAL_INT) v = value_int(0);
            if (strcmp(ast->str_value, "string") == 0 && v.type != VAL_STRING) v = value_string("");
            if (strcmp(ast->str_value, "bool") == 0 && v.type != VAL_BOOL) v = value_bool(0);
            set_var(ast->name, v);
            break;
        }
        case AST_ASSIGN: {
            Value v = eval_expr(ast->left);
            assign_var(ast->name, v);
            break;
        }
        case AST_PRINT: {
            Value v = eval_expr(ast->left);
            if (v.type == VAL_INT) printf("%d\n", v.int_val);
            else if (v.type == VAL_STRING) {
                printf("%s\n", v.str_val);
                free(v.str_val);
            }
            else if (v.type == VAL_BOOL) printf("%s\n", v.bool_val ? "true" : "false");
            break;
        }
        case AST_INPUT: {
            Var* var = find_var(ast->name);
            if (!var) { printf("Undefined variable: %s\n", ast->name); exit(1); }
            if (var->value.type == VAL_INT) {
                int tmp;
                fflush(stdout);
                scanf("%d", &tmp);
                var->value.int_val = tmp;
            } else if (var->value.type == VAL_STRING) {
                char buf[256];
                fflush(stdout);
                if (fgets(buf, sizeof(buf), stdin)) {
                    buf[strcspn(buf, "\n")] = 0;
                    free(var->value.str_val);
                    var->value.str_val = strdup(buf);
                } else {
                    if (var->value.str_val) free(var->value.str_val);
                    var->value.str_val = strdup("");
                }
            } else if (var->value.type == VAL_BOOL) {
                int tmp;
                fflush(stdout);
                scanf("%d", &tmp);
                var->value.bool_val = (tmp != 0);
            }
            break;
        }
        case AST_IF: {
            Value cond = eval_expr(ast->cond);
            if (is_truthy(cond)) {
                exec(ast->then_body);
            } else if (ast->else_body) {
                exec(ast->else_body);
            }
            if (cond.type == VAL_STRING) free(cond.str_val);
            break;
        }
        case AST_WHILE: {
            while (1) {
                Value cond = eval_expr(ast->cond);
                int is_true = is_truthy(cond);
                if (cond.type == VAL_STRING) free(cond.str_val);
                if (!is_true) break;
                exec(ast->body);
            }
            break;
        }
        case AST_FOR: {
            push_scope();
            exec(ast->init);
            while (1) {
                Value cond = eval_expr(ast->cond);
                int is_true = is_truthy(cond);
                if (cond.type == VAL_STRING) free(cond.str_val);
                if (!is_true) break;
                exec(ast->body);
                exec(ast->incr);
            }
            pop_scope();
            break;
        }
        case AST_FUNC_DECL:
            break;
        case AST_FUNC_CALL: {
            eval_expr(ast);
            break;
        }
        case AST_RETURN: {
            Value v = eval_expr(ast->left);
            ret_val.is_returning = 1;
            ret_val.value = v;
            break;
        }
        default: break;
    }
}