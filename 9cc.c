#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
    TK_RESERVED,  // 記号
    TK_NUM,       // 整数トークン
    TK_EOF,       // 入力の終わりを表すトークン
} TokenKind;
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
} NodeKind;

typedef struct Token Token;

// トークン型
struct Token {
    TokenKind kind;  // トークンの型
    Token* next;     // 次の入力トークン
    int val;         // kindがTK_NUMの場合、その数値
    char* str;       // 源字符串还没扫描的部分
    int len;
};
typedef struct Node Node;  // AST node type
struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    int val;
};

// 現在着目しているトークン
Token* token;
char* user_input;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
void error_at(char* loc, char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char* op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "'%c'期待是 + or -", op);
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "期待是一个数字");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token* new_token(TokenKind kind, Token* cur, char* str, int len) {
    // 创建一个新的Token
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    // fprintf(stderr, "token的str=%s\n", str);
    //  把当前的token的next变量赋值给新的token
    cur->next = tok;
    return tok;  // 返回新的token
}
Node* new_node(NodeKind kind) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}
Node* new_binary(NodeKind kind, Node* lhs, Node* rhs) {
    Node* node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}
Node* new_num(int val) {
    Node* node = new_node(ND_NUM);
    node->val = val;
    return node;
}
Node* expr();
Node* mul();
Node* primary();
Node* unary();
Node* equality();
Node* relational();
Node* add();

// expr       = equality
Node* expr() {
    Node* node = equality();
    return node;
}
// equality   = relational ("==" relational | "!=" relational)*
Node* equality() {
    Node* node = relational();
    for (;;) {
        if (consume("==")) {
            node = new_binary(ND_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_binary(ND_NOT_EQ, node, relational());
        } else {
            return node;
        }
    }
}
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*

Node* relational() {
    Node* node = add();
    for (;;) {
        if (consume("<")) {
            node = new_binary(ND_LESS, node, add());
        } else if (consume("<=")) {
            node = new_binary(ND_LESS_EQ, node, add());
        } else if (consume(">")) {
            node = new_binary(ND_LESS, add(), node);
        } else if (consume(">=")) {
            node = new_binary(ND_LESS_EQ, add(), node);
        } else {
            return node;
        }
    }
}
// add        = mul ("+" mul | "-" mul)*
Node* add() {
    Node* node = mul();
    for (;;) {
        if (consume("+")) {
            node = new_binary(ND_ADD, node, mul());
        } else if (consume("-")) {
            node = new_binary(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

// mul     = unary ("*" unary | "/" unary)*
Node* mul() {
    Node* node = unary();

    for (;;) {
        if (consume("*"))
            node = new_binary(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// primary = num | "(" expr ")"
Node* primary() {
    if (consume("(")) {
        Node* node = expr();
        expect(')');
        return node;
    }

    return new_num(expect_number());
}
// unary   = ("+" | "-")? primary
Node* unary() {
    if (consume("+"))
        return unary();
    if (consume("-"))
        return new_binary(ND_SUB, new_num(0), unary());
    return primary();
}
void gen(Node* node) {
    if (node->kind == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NOT_EQ:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LESS:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LESS_EQ:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
    }

    printf("  push rax\n");
}
bool startswith(char* p, char* q) {
    return memcmp(p, q, strlen(q)) == 0;
}

// 入力文字列pをトークナイズしてそれを返す
Token* tokenize(char* p) {
    Token head;
    head.next = NULL;
    Token* cur = &head;  // cur指向第一个token，这个token的next的值是NULL

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        // Multi-letter punctuator
        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") ||
            startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }
        if (strchr("+-*/()<>", *p)) {
            cur = new_token(TK_RESERVED, cur, p++,
                            1);  // p++,会先求出表达式值p，再进行p++
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char* q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }
        error_at(p, "非法字符,无法进行扫描token");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        error("参数数量无效");
        return 1;
    }
    user_input = argv[1];
    // user_input = "5+6";

    token = tokenize(user_input);

    // 生成表达式
    Node* node = expr();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // 抽象構文木を下りながらコード生成
    gen(node);

    // スタックトップに式全体の値が残っているはずなので
    // それをRAXにロードして関数からの返り値とする
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}