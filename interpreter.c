#include "interpreter.h"
#include "FrontEnd/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARIABLE_NO 1000

static NumberVariable variableArray[MAX_VARIABLE_NO];
static int numberOfVariable = 0;
void SetIntVar(char *name, int value) {
    variableArray[numberOfVariable] =
        (NumberVariable){.name = strdup(name), .value = value};
    numberOfVariable++;
}

int EvaluateNodesInt(ASTNode *nodes) {
    if (nodes->type == BlockNode) {
        int lastLineResult = 0;

        for (int i = 0; i < nodes->childCount; i++) {
            if (nodes->children[i] != NULL) {
                lastLineResult = EvaluateNodesInt(nodes->children[i]);
            }
        }
        return lastLineResult; // Returns the evaluation of the last line
    }
    if (nodes->type == NumberNode) {
        return atoi(nodes->value);
    }
    if (nodes->type == IdentifierNode) {
        int savedInt = GetIntVar(nodes->value);

        return savedInt;
    }
    if (nodes->type == AddNode) {
        return EvaluateNodesInt(nodes->left) + EvaluateNodesInt(nodes->right);
    }
    if (nodes->type == SubtractNode) {
        return EvaluateNodesInt(nodes->left) - EvaluateNodesInt(nodes->right);
    }
    if (nodes->type == MultiplyNode) {
        return EvaluateNodesInt(nodes->left) * EvaluateNodesInt(nodes->right);
    }
    if (nodes->type == DivideNode) {
        return EvaluateNodesInt(nodes->left) / EvaluateNodesInt(nodes->right);
    }

    if (nodes->type == AssignmentOpNode) {
        char *variableName = nodes->left->value;
        ASTNode *rightNode = nodes->right;
        int result = EvaluateNodesInt(rightNode);
        printf("REsult %d\n", result);
        SetIntVar(variableName, result);
        return result;
    }
    return 0;
}

int GetIntVar(char *name) {
    for (int i = 0; i < numberOfVariable; i++) {
        if (variableArray[i].name != NULL &&
            strcmp(name, variableArray[i].name) == 0) {
            return variableArray[i].value;
        }
    }
    printf("Variable Name wasn't %s found  \n", name);

    return 0;
}
