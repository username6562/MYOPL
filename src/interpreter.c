#include "../include/interpreter.h"
#include "../include/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARIABLE_NO 1000

static Variable variableArray[MAX_VARIABLE_NO];
static int numberOfVariable = 0;

void SetVar(Variable variable) {
    for (int i = 0; i < numberOfVariable; i++) {
        if (strcmp(variable.name, variableArray[i].name) == 0 &&
            variable.name != NULL) {

            if (variableArray[i].type != variable.type) {
                printf(
                    "RunTime error cannot reassign variable to another type\n");
                exit(1);
            }

            if (variableArray[i].type == String &&
                variableArray[i].stringVal != NULL) {
                free(variableArray[i].stringVal);
            }

            if (variable.type == String) {
                variableArray[i].stringVal = strdup(variable.stringVal);
            } else if (variable.type == Boolean) {
                variableArray[i].boolVal = variable.boolVal;
            } else {
                variableArray[i].intVal = variable.intVal;
            }

            return;
        }
    }

    variableArray[numberOfVariable] = variable;
    numberOfVariable++;
}

void PrintVariableRegistry() {
    printf("\n--- Variable Storage Registry Shelf ---\n");
    for (int i = 0; i < numberOfVariable; i++) {
        if (variableArray[i].name == NULL) {
            printf("[%d]: UNINITIALIZED NULL DATA\n", i);
            continue;
        }

        if (variableArray[i].type == Integer) {
            printf("Name: %s | Type: int    | Value: %d\n",
                   variableArray[i].name, variableArray[i].intVal);
        } else if (variableArray[i].type == String) {
            printf("Name: %s | Type: string | Value: \"%s\"\n",
                   variableArray[i].name, variableArray[i].stringVal);
        } else if (variableArray[i].type == Boolean) {
            printf("Name: %s | Type: bool | Value: %d\n", variableArray[i].name,
                   variableArray[i].boolVal);
        }
    }
    printf("----------------------------------------\n");
}

int EvaluateNodesInt(ASTNode *nodes) {

    if (nodes == NULL) {
        return 0;
    }
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
        Variable savedVal = GetVar(nodes->value);
        if (savedVal.type == Boolean) {
            return savedVal.boolVal;
        }
        return savedVal.intVal;
    }
    if (nodes->type == BoolNode) {
        return EvaluateNodesBool(nodes);
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
    if (nodes->type == GreaterThanNode || nodes->type == LessThanNode ||
        nodes->type == EqualsToNode || nodes->type == LessThanOrEqualsNode ||
        nodes->type == GreaterThanOrEqualsNode) {
        return EvaluateNodesComp(nodes);
    }

    if (nodes->type == IfNode) {
        int condition = EvaluateNodesInt(nodes->left);

        ASTNode *conditionEval = nodes->right;
        if (condition != 0) {
            return EvaluateNodesInt(conditionEval->left);
        } else if (conditionEval != NULL && conditionEval->right != NULL) {
            return EvaluateNodesInt(conditionEval->right);
        }

        return 0;
    }

    if (nodes->type == AssignmentOpNode) {

        char *variableName = nodes->left->value;
        ASTNode *rightNode = nodes->right;

        Variable variable;
        variable.name = variableName;
        variable.name = strdup(variableName);

        if (rightNode->type == StringNode) {
            variable.stringVal = EvaluateNodesChar(rightNode);
            variable.type = String;
            return 0;
        } else if (rightNode->type == BoolNode ||
                   (rightNode->type == IdentifierNode &&
                    GetVar(rightNode->value).type == Boolean) ||
                   rightNode->type == GreaterThanNode ||
                   rightNode->type == LessThanNode ||
                   rightNode->type == EqualsToNode ||
                   rightNode->type == LessThanOrEqualsNode ||
                   rightNode->type == GreaterThanOrEqualsNode) {
            variable.boolVal = EvaluateNodesBool(rightNode);
            variable.type = Boolean;
        } else {
            variable.intVal = EvaluateNodesInt(rightNode);
            variable.type = Integer;
        }

        SetVar(variable);
        if (variable.type == Integer) {
            return variable.intVal;
        } else if (variable.type == Boolean) {
            return variable.boolVal;
        } else {
            return 0;
        }
    }
    return 0;
}

int EvaluateNodesBool(ASTNode *nodes) {
    if (nodes->type == BoolNode) {
        if (strcmp(nodes->value, "true") == 0) {
            return 1;
        }
        if (strcmp(nodes->value, "false") == 0) {
            return 0;
        }
    }
    if (nodes->type == GreaterThanNode || nodes->type == LessThanNode ||
        nodes->type == EqualsToNode || nodes->type == LessThanOrEqualsNode ||
        nodes->type == GreaterThanOrEqualsNode) {
        return EvaluateNodesComp(nodes);
    }

    if (nodes->type == AssignmentOpNode) {
        char *varName = nodes->left->value;
        ASTNode *rightNode = nodes->right;

        int result = EvaluateNodesBool(rightNode);

        Variable variable;
        variable.name = varName;
        variable.boolVal = result;
        variable.type = Boolean;

        SetVar(variable);
        return result;
    }
    return 0;
}
char *EvaluateNodesChar(ASTNode *nodes) {
    if (nodes->type == StringNode) {
        return nodes->value;
    }

    if (nodes->type == AssignmentOpNode) {
        char *variableName = nodes->left->value;
        ASTNode *rightNode = nodes->right;
        char *result = EvaluateNodesChar(rightNode);
        printf("Chat REsult %s \n", result);
        Variable variable;
        variable.name = variableName;
        variable.stringVal = result;
        variable.type = String;

        SetVar(variable);
        return result;
    }

    return NULL;
}

int EvaluateNodesComp(ASTNode *nodes) {
    {
        if (nodes->type == NumberNode) {
            int number = atoi(nodes->value);
            return number;
        }

        if (nodes->type == GreaterThanNode) {
            int leftSide = EvaluateNodesInt(nodes->left);
            int rightSide = EvaluateNodesInt(nodes->right);

            return leftSide > rightSide;
        }
        if (nodes->type == LessThanNode) {
            int leftSide = EvaluateNodesInt(nodes->left);
            int rightSide = EvaluateNodesInt(nodes->right);

            return leftSide < rightSide;
        }
        if (nodes->type == EqualsToNode) {
            int leftSide = EvaluateNodesInt(nodes->left);
            int rightSide = EvaluateNodesInt(nodes->right);

            return leftSide == rightSide;
        }
        if (nodes->type == LessThanOrEqualsNode) {
            int leftSide = EvaluateNodesInt(nodes->left);
            int rightSide = EvaluateNodesInt(nodes->right);

            return leftSide <= rightSide;
        }
        if (nodes->type == GreaterThanOrEqualsNode) {
            int leftSide = EvaluateNodesInt(nodes->left);
            int rightSide = EvaluateNodesInt(nodes->right);

            return leftSide >= rightSide;
        }
        return 0;
    }
}
Variable GetVar(char *name) {

    for (int i = 0; i < numberOfVariable; i++) {
        if (variableArray[i].name != NULL &&
            strcmp(name, variableArray[i].name) == 0) {
            return variableArray[i];
        }
    }
    printf("Variable Name wasn't %s found  \n", name);
    return (Variable){0};
}

void EvaluateNodeFunc(ASTNode nodes) {}
