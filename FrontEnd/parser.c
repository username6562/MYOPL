#include "parser.h"
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
    if (t.type == EqualsToken) {
        ConsumeToken(file);
        return CreateNode(AssignmentOpNode, t.value);
    }
    if (t.type == OpenParenthesis) {
        ConsumeToken(file);
        ASTNode *innerExpression = ParseExpression(file, 0);

        Token peek = PeekToken(file);
        if (peek.type == CloseParenthesis) {
            ConsumeToken(file);
            return innerExpression;
        }
    }
    if (t.type == StringToken) {
        ConsumeToken(file);
        return CreateNode(StringNode, t.value);
    }
    if (t.type == UnknownToken) {
        ConsumeToken(file);
        return CreateNode(UnknownNode, t.value);
    }
    if (t.type == BoolToken) {
        ConsumeToken(file);
        return CreateNode(BoolNode, t.value);
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

    if (token.type == IfToken) {
        ConsumeToken(file);
        ASTNode *if_node = CreateNode(IfNode, "if");
        Token openParenToken = PeekToken(file);
        if (openParenToken.type == OpenParenthesis) {
            ConsumeToken(file);
            ASTNode *conditionNode = ParseExpression(file, 0);
            Token closeParenToken = PeekToken(file);

            if (closeParenToken.type == CloseParenthesis) {
                ConsumeToken(file);
                Token openCurlyTok = PeekToken(file);

                if (openCurlyTok.type == OpenCurlyBracket) {
                    ConsumeToken(file);

                    ASTNode *ifInterface = CreateNode(BlockNode, "if");

                    while (true) {
                        Token closeParenTok = PeekToken(file);

                        if (closeParenTok.type == EOFToken ||
                            closeParenTok.type == CloseCurlyBracket) {
                            break;
                        }

                        ASTNode *currentLine = ParseStatement(file);

                        if (currentLine != NULL) {
                            ifInterface->children[ifInterface->childCount] =
                                currentLine;
                            ifInterface->childCount++;

                        } else {
                            ConsumeToken(file);
                        }
                    }
                    ConsumeToken(file);

                    if_node->left = conditionNode;
                    if_node->right = ifInterface;

                    return if_node;
                }
            }
        }
    }

    // Checks if Variable is going to be initialized
    if (token.type == KeyWordToken && (strcmp(token.value, "int") == 0 ||
                                       strcmp(token.value, "string") == 0 ||
                                       strcmp(token.value, "bool") == 0)) {
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

                Token semi = PeekToken(file);
                if (semi.type == SemiColonToken) {
                    ConsumeToken(file);
                } else {
                    printf("Syntax Error Semi-Colon Expected\n");
                    ConsumeToken(file);
                }

                return assignmentNode;
            }
        }
    }

    // Checks if Variable is About To Be Reassigned
    if (token.type == IdentifierToken) {
        ConsumeToken(file);
        Token eqToken = PeekToken(file);
        ASTNode *variableNode = CreateNode(IdentifierNode, token.value);
        if (eqToken.type == EqualsToken) {
            ConsumeToken(file);
            ASTNode *assignmentNode = CreateNode(AssignmentOpNode, "=");
            ASTNode *rightNode = ParseExpression(file, 0);

            Token semi = PeekToken(file);
            if (semi.type == SemiColonToken) {
                ConsumeToken(file);
            } else {
                printf("Syntax Error Semi-Colon Expected\n");
            }
            assignmentNode->left = variableNode;
            assignmentNode->right = rightNode;

            return assignmentNode;
        }
    }
    ASTNode *exprNode = ParseExpression(file, 0);
    Token semi = PeekToken(file);
    if (semi.type == SemiColonToken) {
        ConsumeToken(file);
    } else {
        printf("Syntax Error: Semi-Colon expected\n");
        ConsumeToken(file);
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
