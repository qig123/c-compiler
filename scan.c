
#include "base.h"

char* user_input;
Token* token;

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
bool at_eof() {
    return token->kind == TK_EOF;
}

bool startswith(char* p, char* q) {
    return memcmp(p, q, strlen(q)) == 0;
}

bool consume(char* op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}
Token* consume_ident() {
    if (token->kind != TK_IDENT) {
        return NULL;
    }
    Token* original_token = token;
    token = token->next;
    return original_token;
}

void expect(char* op) {
    if (token->kind != TK_RESERVED || memcmp(token->str, op, token->len))
        error_at(token->str, "'%c'期待是 + or -", op);
    token = token->next;
}

long expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "期待是一个数字");
    long val = token->val;
    token = token->next;
    return val;
}

Token* tokenize() {
    char* p = user_input;

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
        if (strchr("+-*/()<>;=", *p)) {
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
        if ('a' <= *p && *p <= 'z') {
            cur = new_token(TK_IDENT, cur, p++, 1);
            cur->len = 1;
            continue;
        }
        error_at(p, "非法字符,无法进行扫描token");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
