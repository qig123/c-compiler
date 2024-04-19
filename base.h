#ifndef BASE_H
#define BASE_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

// token type
typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token* next;
    long val;
    char* str;
    int len;
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar* next;  // 次の変数かNULL
    char* name;  // 変数の名前
    int len;     // 名前の長さ
    int offset;  // RBPからのオフセット
};

void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);
bool consume(char* op);
Token* consume_ident(void);
void expect(char* op);
long expect_number(void);
bool at_eof(void);
Token* tokenize(void);

extern char* user_input;
extern Token* token;
extern LVar* locals;

// parser.c
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQ,
    ND_NOT_EQ,
    ND_LESS,
    ND_LESS_EQ,
    ND_MORE,
    ND_ASSIGN,
    ND_LVAR,
} NodeKind;

// AST node type
typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    int val;
    int offset;
};
extern Node* code[100];  // 声明变量

void program(void);

// codegen.c
void codegen();

#endif