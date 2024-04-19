
#include "base.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        error("参数数量无效");
        return 1;
    }
    user_input = argv[1];

    token = tokenize();
    Node* node = expr();
    codegen(node);
    return 0;
}
