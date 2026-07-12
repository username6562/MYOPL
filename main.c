#include "FrontEnd/ast.h"
#include "FrontEnd/parser.h"
#include "interpreter.h"
#include <stdbool.h>
#include <stdio.h>
#define MAXTOKEN 1000

int main() {
    FILE *file = fopen("source.txt", "r");
    ASTNode *node = Parse(file);
    EvaluateNodesInt(node);
    PrintVariableRegistry();
    // PrintAST(node);
    fclose(file);
}
