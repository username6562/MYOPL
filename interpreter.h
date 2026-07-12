#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "FrontEnd/ast.h"

typedef enum { Integer, String, Boolean } VariableTypes;
typedef struct {
    const char *name;
    union {
        int intVal;
        int boolVal;
        char *stringVal;
    };
    VariableTypes type;
} Variable;

int EvaluateNodesBool(ASTNode *nodes);
int EvaluateNodesInt(ASTNode *nodes);
char *EvaluateNodesChar(ASTNode *nodes);
void SetIntVar(char *name, int value);

int EvaluateNodesComp(ASTNode *nodes);
void PrintVariableRegistry();
Variable GetVar(char *name);

#endif // !INTERPRETER_H
