
#include "base.h"
static void print(void);
static void printAst(Node* node, int depth);
static void printTokenList(Token* head);

int main(int argc, char** argv) {
    if (argc != 2) {
        error("参数数量无效");
        return 1;
    }
    user_input = argv[1];
    // user_input = "foo=5;too=6;foo+too;";
    token = tokenize();
    // printTokenList(token);
    program();
    // print();
    codegen();
    return 0;
}
static void printTokenList(Token* head) {
    Token* current = head;
    while (current != NULL) {
        printf("Token: kind=%d, val=%ld, str=%s\n", current->kind, current->val,
               current->str);
        current = current->next;
    }
}
// Function to print the tree-like structure of the AST
static void printAst(Node* node, int depth) {
    if (node == NULL) {
        return;
    }

    // Indentation based on depth
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Print node information
    switch (node->kind) {
        case ND_NUM:
            printf("Number: %d\n", node->val);
            break;
        case ND_ADD:
            printf("Addition (+)\n");
            break;
        case ND_SUB:
            printf("Subtraction (-)\n");
            break;
        case ND_MUL:
            printf("Multiplication (*)\n");
            break;
        case ND_DIV:
            printf("Division (/)\n");
            break;
        case ND_ASSIGN:
            printf("Assignment (=)\n");
            break;
        case ND_LVAR:
            printf("Local Variable (offset: %d)\n", node->offset);
            break;
        case ND_EQ:
            printf("Equal (==)\n");
            break;
        case ND_NOT_EQ:
            printf("Not Equal (!=)\n");
            break;
        case ND_LESS:
            printf("Less Than (<)\n");
            break;
        case ND_LESS_EQ:
            printf("Less Than or Equal (<=)\n");
            break;
        default:
            printf("Unknown Node\n");
            break;
    }

    // Recursively print left and right child nodes
    printAst(node->lhs, depth + 1);
    printAst(node->rhs, depth + 1);
}
static void print() {
    for (int i = 0; i < 100; i++) {
        if (code[i] == NULL) {
            break;
        }
        printAst(code[i], 0);
        printf("------------------------\n");
    }
}
