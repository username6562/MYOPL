#include "../include/ast.h"
#include "../include/interpreter.h"
#include "../include/parser.h"
#include <stdbool.h>
#include <stdio.h>
#define MAXTOKEN 1000
int main(void) {
    FILE *file = fopen("source.txt", "r");
    ASTNode *node = Parse(file);
    EvaluateNodesInt(node);
    PrintVariableRegistry();
    fclose(file);
}
