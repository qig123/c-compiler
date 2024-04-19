#include "base.h"
static Node* stmt(void);
static Node* expr(void);
static Node* assign(void);
static Node* equality(void);
static Node* relational(void);
static Node* add(void);
static Node* mul(void);
static Node* unary(void);
static Node* primary(void);

Node* code[100];

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
// program    = stmt*
void program() {
    int i = 0;
    while (!at_eof()) {
        code[i++] = stmt();
    }
    code[i] = NULL;
}
// stmt       = expr ";"

Node* stmt() {
    Node* node = expr();
    expect(";");
    return node;
}

// expr       = assign
Node* expr() {
    Node* node = assign();
    return node;
}
// assign     = equality ("=" assign)?
Node* assign() {
    Node* node = equality();
    if (consume("=")) {
        node = new_binary(ND_ASSIGN, node, assign());
    }
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

// unary   = ("+" | "-")? primary
static Node* unary() {
    if (consume("+"))
        return unary();
    if (consume("-"))
        return new_binary(ND_SUB, new_num(0), unary());
    return primary();
}

// primary    = num | ident | "(" expr ")"
static Node* primary() {
    if (consume("(")) {
        Node* node = expr();
        expect(")");
        return node;
    }
    Token* tok = consume_ident();
    if (tok) {
        Node* node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }

    return new_num(expect_number());
}