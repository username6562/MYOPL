#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "FrontEnd/ast.h"

typedef enum { Integer, String } VariableTypes;
typedef struct {
    const char *name;
    union {
        int intVal;
        char *stringVal;
    } VarType;
    VariableTypes type;
} Variable;

int EvaluateNodesInt(ASTNode *nodes);
void SetIntVar(char *name, int value);

int GetIntVar(char *name);

#endif // !INTERPRETER_H
