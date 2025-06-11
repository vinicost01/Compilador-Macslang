# MACSLang

**MACSLang** é um interpretador desenvolvido para a disciplina de compiladores na UNI-BH, inspirado na definição original proposta pelo Professor Marco Antônio. O projeto implementa uma linguagem imperativa, estruturada, com tipagem estática e sintaxe enxuta, capaz de expressar programas educacionais, exemplos didáticos e algoritmos clássicos.

---

## Créditos

* **Definição original da linguagem:** Prof. Marco Antônio (UNI-BH)
* **Desenvolvimento do interpretador:** Vinicius Costa

---

## Sumário

* [Descrição](#descrição)
* [Características da Linguagem](#características-da-linguagem)
* [Gramática](#gramática)
* [Exemplos de código MACSLang](#exemplos-de-código-macslang)
* [Arquitetura do Projeto](#arquitetura-do-projeto)
* [Como compilar e executar](#como-compilar-e-executar)
* [Detalhes de implementação](#detalhes-de-implementação)

---

## Descrição

MACSLang é uma linguagem de programação imperativa e estruturada, de sintaxe clara e amigável, desenvolvida para fins acadêmicos. O projeto inclui **analisador léxico, sintático e interpretador**, suportando variáveis, controle de fluxo, funções, entrada/saída, operações aritméticas, booleanas e manipulação de strings.

---

## Características da Linguagem

* **Paradigma:** Imperativo, estruturado
* **Tipagem:** Estática
* **Tipos primitivos:** `int`, `string`, `bool`
* **Operadores:** Aritméticos, relacionais, lógicos e concatenação de strings
* **Controle de fluxo:** `if`, `else`, `while`, `for`
* **Funções:** Definição, chamada, recursão, parâmetros e retorno
* **Entrada e saída:** `input()`, `print()`
* **Comentários:** Suporte a `//` para comentários de linha

---

## Gramática

```ebnf
program           = { statement } ;
statement         = var_decl | assignment | func_decl | func_call | print_stmt | input_stmt
                  | if_stmt | while_stmt | for_stmt | return_stmt ;

var_decl          = "var" identifier ":" type [ "=" expr ] ";" ;
assignment        = identifier "=" expr ";" ;
func_decl         = "func" identifier "(" [ param_list ] ")" ":" type block ;
param_list        = identifier ":" type { "," identifier ":" type } ;
func_call         = identifier "(" [ expr { "," expr } ] ")" ";" ;

print_stmt        = "print" "(" expr ")" ";" ;
input_stmt        = "input" "(" identifier ")" ";" ;
return_stmt       = "return" expr ";" ;

if_stmt           = "if" "(" expr ")" block [ "else" block ] ;
while_stmt        = "while" "(" expr ")" block ;
for_stmt          = "for" "(" (var_decl | assignment) expr ";" assignment ")" block ;

block             = "{" { statement } "}" ;
expr              = ... (expressões aritméticas, relacionais, booleanas, concatenação) ...

type              = "int" | "string" | "bool" ;
identifier        = [a-zA-Z_][a-zA-Z0-9_]* ;
```

---

## Exemplos de código MACSLang

**Hello World e leitura de nome**

```macslang
print("Hello, world!");
print("What is your name?");
var name: string;
input(name);
print("Welcome, " + name + "!");
```

**Fatorial (recursivo)**

```macslang
func factorial(n: int): int {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

print("Enter a number:");
var num: int;
input(num);
var fact: int = factorial(num);
print("Factorial of " + num + " is " + fact);
```

**For e if/else**

```macslang
print("Counting from 1 to 5:");
for (var i: int = 1; i <= 5; i = i + 1) {
    print(i);
}

print("Enter a number:");
var n: int;
input(n);
if (n % 2 == 0) {
    print("Even");
} else {
    print("Odd");
}
```

**All features demo**

```macslang
// Factorial (recursion)
func factorial(n: int): int {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

// String repetition
func repeat(s: string, times: int): string {
    var result: string = "";
    var i: int = 0;
    while (i < times) {
        result = result + s;
        i = i + 1;
    }
    return result;
}

// Is even
func is_even(x: int): bool {
    if (x % 2 == 0) {
        return true;
    } else {
        return false;
    }
}

print("=== MACSLang All Features Demo ===");

print("Type your name:");
var name: string;
input(name);

print("Type an integer:");
var num: int;
input(num);

var fact: int = factorial(num);
print("Hello, " + name + "! The factorial of " + num + " is " + fact);

if (is_even(num)) {
    print(num + " is even!");
} else {
    print(num + " is odd!");
}

print("Printing your name 3 times with for:");
for (var i: int = 0; i < 3; i = i + 1) {
    print((i + 1) + ": " + name);
}

print("Repeating '*' character:");
var stars: string = repeat("*", num);
print(stars);

// Boolean test and print
var flag: bool = false;
if (fact > 100) {
    flag = true;
}
print("Is factorial greater than 100? " + flag);
```

---

## Arquitetura do Projeto

```
macslang/
│
├── main.c            # Ponto de entrada, gerencia fluxo principal
├── lexer.h/.c        # Analisador léxico (tokeniza o código fonte)
├── parser.h/.c       # Analisador sintático (constrói AST)
├── interpreter.h/.c  # Interpretador (executa AST)
├── exemplos/         # Exemplos de códigos MACSLang
│     └── *.macslang
└── README.md         # Este arquivo
```

---

## Como compilar e executar

1. **Pré-requisitos:**
   GCC ou Clang (Linux, macOS, Windows WSL, Apple Silicon OK)

2. **Compilação:**

   ```sh
   clang main.c lexer.c parser.c interpreter.c -o macslang
   # ou
   gcc main.c lexer.c parser.c interpreter.c -o macslang
   ```

3. **Execução:**
   
   
![](macslang.gif)
   ```sh
   ./macslang exemplos/hello.macslang
   ./macslang exemplos/fatorial.macslang
   ./macslang exemplos/all_features.macslang
   ```

---

## Detalhes de Implementação

* **Lexer:**
  Ignora espaços, tabulações e comentários (`//`). Reconhece palavras-chave, identificadores, números, strings, operadores, delimitadores.

* **Parser:**
  Constrói uma árvore sintática abstrata (AST) a partir dos tokens. Permite declarações de variáveis, funções, expressões e controle de fluxo em qualquer ordem. Tipos suportados: `int`, `string`, `bool`.

* **Interpretador:**
  Executa a AST em tempo real, realizando avaliação de expressões, controle de variáveis, chamadas de função, recursão, controle de fluxo, entrada/saída e manipulação de strings e booleanos.

---

## Observações Acadêmicas

* **O projeto foi construído com foco didático**, priorizando clareza de implementação e extensibilidade.
* O interpretador é modular, facilitando inclusão de novos tipos, operadores e comandos.
* **Agradecimento ao Prof. Marco Antônio** pela inspiração na definição da linguagem.

---

## Licença

Uso acadêmico, livre para fins educacionais e aprimoramento.

---
