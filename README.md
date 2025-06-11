# MACSLang: An Educational Programming Language and Interpreter

## Abstract

MACSLang is a didactic, structured, statically-typed programming language, originally specified by Professor Marco Antônio, and fully implemented in C by Vinicius Costa.
This repository contains a complete interpreter for MACSLang, including a lexical analyzer (lexer), syntactic analyzer (parser), AST construction, and a tree-walking interpreter.

---

## Objectives

* Demonstrate the construction of a simple compiler/interpreter pipeline (lexer, parser, interpreter).
* Provide a minimal language for educational purposes, covering variables, types, functions, loops, conditionals, and I/O.
* Serve as a foundation for further academic experiments with language and compiler design.

---

## Project Architecture

**The interpreter is organized in four main modules:**

* **lexer.c / lexer.h**
  Tokenizes MACSLang source code, identifying keywords, identifiers, literals, and operators.

* **parser.c / parser.h**
  Builds an Abstract Syntax Tree (AST) from the token sequence, according to MACSLang’s grammar.

* **interpreter.c / interpreter.h**
  Executes the AST, managing environments, variables (global and local), function calls, control flow, and input/output.

* **main.c**
  Entry point; loads the program from file, initializes components, and invokes the interpreter.

**File Structure:**

```text
macslang/
├── main.c
├── lexer.h / lexer.c
├── parser.h / parser.c
├── interpreter.h / interpreter.c
├── exemplos/
│   ├── hello.macslang
│   ├── fatorial.macslang
│   ├── for.macslang
│   ├── sum.macslang
│   ├── ifelse.macslang
│   └── all_features.macslang
└── README.md
```

---

## Language Grammar (Formal EBNF)

Below is a simplified EBNF description of MACSLang’s syntax:

```ebnf
program         ::= { statement }

statement       ::= var_decl | assignment | func_decl | if_stmt | while_stmt | for_stmt | print_stmt | input_stmt | return_stmt | expr_stmt

var_decl        ::= "var" identifier ":" type [ "=" expression ] ";"
assignment      ::= identifier "=" expression ";"
func_decl       ::= "func" identifier "(" [ param_list ] ")" ":" type block
param_list      ::= param { "," param }
param           ::= identifier ":" type
block           ::= "{" { statement } "}"

if_stmt         ::= "if" "(" expression ")" block [ "else" block ]
while_stmt      ::= "while" "(" expression ")" block
for_stmt        ::= "for" "(" var_decl expression ";" assignment ")" block
print_stmt      ::= "print" "(" expression ")" ";"
input_stmt      ::= "input" "(" identifier ")" ";"
return_stmt     ::= "return" expression ";"
expr_stmt       ::= expression ";"

type            ::= "int" | "string" | "bool"
expression      ::= literal
                 | identifier
                 | expression binop expression
                 | identifier "(" [ arg_list ] ")"
                 | "(" expression ")"
arg_list        ::= expression { "," expression }
literal         ::= integer | string | "true" | "false"
binop           ::= "+" | "-" | "*" | "/" | "%" | "<" | "<=" | ">" | ">=" | "==" | "!="
```

---

## Supported Features

* **Static typing:** `int`, `string`, `bool`
* **Variable declaration/assignment:** global and local (block) scope
* **Functions:** first-class, with parameters and return, recursion allowed
* **Control structures:** `if/else`, `while`, `for`
* **Operators:** arithmetic, relational, logical, string concatenation
* **Input/output:** `print()`, `input()`
* **Block scoping:** each `{ ... }` introduces a new variable scope

---

## Example Program

```macslang
func factorial(n: int): int {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}
print("Enter your name:");
var name: string;
input(name);
print("Enter a number:");
var num: int;
input(num);
var fact: int = factorial(num);
print("Hello, " + name + "! Factorial of " + num + " is " + fact);
```

---

## Building the Interpreter

**Requirements:**

* GCC or Clang C compiler (Linux or macOS)
* Terminal

**To compile:**

```sh
gcc main.c lexer.c parser.c interpreter.c -o macslang
# or
clang main.c lexer.c parser.c interpreter.c -o macslang
```

**To run:**

```sh
./macslang exemplos/all_features.macslang
```

---

## How the Interpreter Works

1. **Lexical Analysis:**
   The source file is read and tokenized; each token is classified as keyword, identifier, literal, or operator.

2. **Parsing:**
   The parser processes the token sequence, building an Abstract Syntax Tree (AST) that captures the program’s structure and relationships.

3. **Interpretation:**
   The AST is traversed recursively. Variable scope is managed as a stack of symbol tables; function calls push/pop scopes and manage parameters and returns. Expressions and statements are evaluated/executed accordingly.

---

## Extending MACSLang

* Add keywords or types in `lexer.h` and parser logic.
* Extend the AST in `parser.h`/`parser.c` for new constructs.
* Implement new operations or statements in `interpreter.c`.

---

## Troubleshooting

* **Undefined variable/function:** Ensure all identifiers are declared in accessible scope.
* **Syntax error:** Double-check your code matches the MACSLang grammar.
* **Compilation error:** Make sure you are using a C99-compatible compiler and all source files are present.

---

## Credits

Developed by **Vinicius Costa**, based on the MACSLang language definition by **Professor Marco Antônio**, for educational and academic purposes.
