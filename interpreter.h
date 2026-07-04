#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "FrontEnd/ast.h"

typedef struct {
    const char *name;
    int value;
} NumberVariable;
int EvaluateNodesInt(ASTNode *nodes);
void SetIntVar(char *name, int value);
int GetIntVar(char *name);

#endif // !INTERPRETER_H
