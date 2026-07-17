#ifndef AST_H
#define AST_H
#include <stdbool.h>
typedef enum {
    NumberNode,
    AddNode,
    SubtractNode,
    MultiplyNode,
    DivideNode,
    EqualsNode,
    AssignmentOpNode,
    IdentifierNode,
    IntNode,
    BlockNode,
    UnknownNode,
    StringNode,
    BoolNode,
    IfNode,
    ElseNode,
    GreaterThanNode,
    LessThanNode,
    EqualsToNode,
    LessThanOrEqualsNode,
    GreaterThanOrEqualsNode,
    FunctionDeclNode
} NodeType;
typedef struct ASTNode ASTNode;

struct ASTNode {
    char value[256];
    NodeType type;
    ASTNode *left;
    ASTNode *right;
    ASTNode *children[100];
    int childCount;
};
ASTNode *CreateNode(NodeType type, const char *value);

void PrintAST(ASTNode *node);
void PrintASTASCII(ASTNode *node);
#endif
