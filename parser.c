#include "base.h"

static Node* equality(void);
static Node* relational(void);
static Node* add(void);
static Node* mul(void);
static Node* unary(void);
static Node* primary(void);

static Token* new_token(TokenKind kind, Token* cur, char* str, int len) {
    // 创建一个新的Token
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    //  把当前的token的next变量赋值给新的token
    cur->next = tok;
    return tok;  // 返回新的token
}
static Node* new_node(NodeKind kind) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}
static Node* new_binary(NodeKind kind, Node* lhs, Node* rhs) {
    Node* node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}
static Node* new_num(int val) {
    Node* node = new_node(ND_NUM);
    node->val = val;
    return node;
}

// expr       = equality
Node* expr() {
    Node* node = equality();
    return node;
}
// equality   = relational ("==" relational | "!=" relational)*
static Node* equality() {
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

static Node* relational() {
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
static Node* add() {
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
static Node* mul() {
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
static Node* primary() {
    if (consume("(")) {
        Node* node = expr();
        expect(')');
        return node;
    }

    return new_num(expect_number());
}
// unary   = ("+" | "-")? primary
static Node* unary() {
    if (consume("+"))
        return unary();
    if (consume("-"))
        return new_binary(ND_SUB, new_num(0), unary());
    return primary();
}