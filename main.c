#include "FrontEnd/ast.h"
#include "FrontEnd/parser.h"
#include <stdbool.h>
#include <stdio.h>
#define MAXTOKEN 1000

int main() {
    FILE *file = fopen("source.txt", "r");
    ASTNode *node = Parse(file);
    PrintASTASCII(node);
}
