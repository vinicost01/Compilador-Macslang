#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <file.macslang>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        printf("Could not open file: %s\n", argv[1]);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);

    char *source = malloc(sz + 1);
    fread(source, 1, sz, f);
    source[sz] = '\0';
    fclose(f);

    init_lexer(source);
    AST *program = parse_program();

    if (!program) {
        printf("Parsing failed.\n");
        free(source);
        return 1;
    }

    init_vars();
    interpret(program);

    free_ast(program);
    free(source);

    return 0;
}