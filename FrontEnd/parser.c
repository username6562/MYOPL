#include "parser.h"
#include "../interpreter.h"
#include "ast.h"
#include "lexer.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static Token token;
static bool hasLookAhead;
ASTNode *ParseExpression(FILE *file, int currentBindingPower);
Token PeekToken(FILE *file) {
    if (hasLookAhead == false) {
        token = GetToken(file);
        hasLookAhead = true;
    }
    return token;
}

Token ConsumeToken(FILE *file) {
    if (hasLookAhead == false) {
        return GetToken((file));
    }
    hasLookAhead = false;
    return token;
}

int GetBindingPower(Token token) {
    if (token.type == EOFToken) {
        return 0;
    } else if (token.type == EqualsToken) {
        return 10;
    } else if (token.type == BinaryOpToken) {
        if (strcmp(token.value, "+") == 0 || strcmp(token.value, "-") == 0) {
            return 20;
        } else if (strcmp(token.value, "*") == 0 ||
                   strcmp(token.value, "/") == 0) {
            return 30;
        }
    }
    return 0;
}
ASTNode *ParsePrimary(FILE *file) {
    Token t = PeekToken(file);

    if (t.type == IntToken) {
        ConsumeToken(file);
        return CreateNode(NumberNode, t.value);
    }
    if (t.type == IdentifierToken) {
        ConsumeToken(file);
        return CreateNode(IdentifierNode, t.value);
    }
    //
    // if (t.type == IdentifierToken) {
    //     ConsumeToken(file);
    //     ASTNode *varLeft = CreateNode(IdentifierNode, t.value);
    //
    //     Token peek = PeekToken(file);
    //     if (peek.type == EqualsToken) {
    //         ConsumeToken(file);
    //         ASTNode *assignmentOp = CreateNode(AssignmentOpNode, "=");
    //
    //         ASTNode *mathExpression = ParseExpression(file, 0);
    //
    //         assignmentOp->left = varLeft;
    //         assignmentOp->right = mathExpression;
    //         return assignmentOp;
    //     }
    // }
    if (t.type == EqualsToken) {
        ConsumeToken(file);
        return CreateNode(EqualsNode, t.value);
    }
    if (t.type == OpenParenthesis) {
        ConsumeToken(file);
        ASTNode *innerExpression = ParseExpression(file, 0);

        Token peek = PeekToken(file);
        if (peek.type == CloseParenthesis) {
            return innerExpression;
        }
    }
    if (t.type == UnknownToken) {
        ConsumeToken(file);
        return CreateNode(UnknownNode, t.value);
    }
    return NULL;
}

ASTNode *ParseExpression(FILE *file, int currentBindingPower) {

    ASTNode *left = ParsePrimary(file);
    Token t = PeekToken(file);

    if (t.type == EOFToken) {

        return left;
    }
    while (true) {
        Token nextToken = PeekToken(file);
        int nextBindingPower = GetBindingPower(nextToken);

        if (currentBindingPower >= nextBindingPower) {
            break;
        }

        NodeType opType;

        if (nextToken.type == BinaryOpToken) {
            ConsumeToken(file);
            if (strcmp(nextToken.value, "+") == 0) {
                opType = AddNode;
            } else if (strcmp(nextToken.value, "-") == 0) {
                opType = SubtractNode;
            } else if (strcmp(nextToken.value, "*") == 0) {
                opType = MultiplyNode;
            } else if (strcmp(nextToken.value, "/") == 0) {
                opType = DivideNode;
            } else {
                break;
            }
            ASTNode *opNode = CreateNode(opType, nextToken.value);
            opNode->left = left;

            opNode->right = ParseExpression(file, nextBindingPower);
            left = opNode;
        }
    }
    return left;
}

ASTNode *ParseStatement(FILE *file) {

    Token token = PeekToken(file);
    if (token.type == KeyWordToken && strcmp(token.value, "int") == 0) {
        ConsumeToken(file);
        Token nextToken = PeekToken(file);

        if (nextToken.type == IdentifierToken) {
            ConsumeToken(file);
            ASTNode *varLeft = CreateNode(IdentifierNode, nextToken.value);

            Token eqToken = PeekToken(file);
            if (eqToken.type == EqualsToken) {
                ConsumeToken(file);
                ASTNode *assignmentNode = CreateNode(AssignmentOpNode, "=");
                ASTNode *rightNode = ParseExpression(file, 0);
                assignmentNode->left = varLeft;
                assignmentNode->right = rightNode;

                int rightNodeResult = EvaluateNodesInt(rightNode);

                Token semi = PeekToken(file);
                if (semi.type == SemiColonToken) {
                    ConsumeToken(file);
                } else {
                    printf("Syntax Error Semi Colon Expected\n");
                }

                return assignmentNode;
            }
        }
    }
    ASTNode *exprNode = ParseExpression(file, 0);
    Token semi = PeekToken(file);
    if (semi.type == SemiColonToken) {
        ConsumeToken(file);
    } else {
        printf("Syntax Error: Semi-Colon expected\n");
    }
    return exprNode;
}
ASTNode *Parse(FILE *file) {
    hasLookAhead = false;

    ASTNode *programRoot = CreateNode(BlockNode, "Program");

    while (true) {
        Token t = PeekToken(file);
        // 3. Stop signal: If we hit the absolute end of the file, break!
        if (t.type == EOFToken) {
            break;
        }
        printf("LEXER DEBUG: Type = %d, Value = '%s'\n", t.type, t.value);
        ASTNode *currentLine = ParseStatement(file);

        if (currentLine != NULL) {
            programRoot->children[programRoot->childCount] = currentLine;
            programRoot->childCount++;
        } else {
            ConsumeToken(file);
        }
    }

    return programRoot;
}
