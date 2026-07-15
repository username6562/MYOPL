#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
    } else if (token.type == ComparisonOpToken) {
        return 15;
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
    if (t.type == OpenParenthesisToken) {
        ConsumeToken(file);
        ASTNode *innerExpression = ParseExpression(file, 0);

        Token peek = PeekToken(file);
        if (peek.type == CloseParenthesisToken) {
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
        // nextToken.type);
        int nextBindingPower = GetBindingPower(nextToken);

        if (currentBindingPower >= nextBindingPower) {
            break;
        }

        NodeType opType;

        if (nextToken.type == BinaryOpToken ||
            nextToken.type == ComparisonOpToken) {
            ConsumeToken(file);
            if (strcmp(nextToken.value, "+") == 0) {
                opType = AddNode;
            } else if (strcmp(nextToken.value, "-") == 0) {
                opType = SubtractNode;
            } else if (strcmp(nextToken.value, "*") == 0) {
                opType = MultiplyNode;
            } else if (strcmp(nextToken.value, "/") == 0) {
                opType = DivideNode;
            } else if (strcmp(nextToken.value, "<") == 0) {
                opType = LessThanNode;
            } else if (strcmp(nextToken.value, ">") == 0) {
                opType = GreaterThanNode;
            } else if (strcmp(nextToken.value, "==") == 0) {
                opType = EqualsToNode;
            } else if (strcmp(nextToken.value, "<=") == 0) {
                opType = LessThanOrEqualsNode;
            } else if (strcmp(nextToken.value, ">=") == 0) {
                opType = GreaterThanOrEqualsNode;
            }
        } else {
            // printf("Could not find expected operator\n");
            break;
        }

        ASTNode *opNode = CreateNode(opType, nextToken.value);
        opNode->left = left;

        opNode->right = ParseExpression(file, nextBindingPower);
        left = opNode;
    }

    return left;
}

ASTNode *ParseStatement(FILE *file, bool isExpectingElif) {

    Token token = PeekToken(file);

    if (token.type == IfToken) {
        ConsumeToken(file);
        ASTNode *if_node = CreateNode(IfNode, "if");
        ASTNode *conditionEvalNode =
            CreateNode(BlockNode, "ConditionEvaluator");

        conditionEvalNode->right = NULL;

        if_node->right = conditionEvalNode;
        Token openParenToken = PeekToken(file);
        if (openParenToken.type == OpenParenthesisToken) {
            ConsumeToken(file);
            ASTNode *conditionNode = ParseExpression(file, 0);
            Token closeParenToken = PeekToken(file);

            if (closeParenToken.type == CloseParenthesisToken) {
                ConsumeToken(file);
                Token openCurlyTok = PeekToken(file);

                if (openCurlyTok.type == OpenCurlyBracket) {
                    ConsumeToken(file);

                    ASTNode *ifNode = CreateNode(BlockNode, "if");

                    conditionEvalNode->left = ifNode;

                    while (true) {
                        Token closeParenTok = PeekToken(file);

                        if (closeParenTok.type == EOFToken ||
                            closeParenTok.type == CloseCurlyBracket) {
                            break;
                        }

                        ASTNode *currentLine = ParseStatement(file, false);

                        if (currentLine != NULL) {
                            ifNode->children[ifNode->childCount] = currentLine;
                            ifNode->childCount++;

                        } else {
                            ConsumeToken(file);
                        }
                    }
                    ConsumeToken(file);

                    Token nextToken = PeekToken(file);

                    if (nextToken.type == ElifToken) {
                        ASTNode *elifNode = ParseStatement(file, true);
                        return elifNode;
                    }

                    else if (nextToken.type == ElseToken) {
                        ConsumeToken(file);
                        ASTNode *elseNode = CreateNode(BlockNode, "else");
                        conditionEvalNode->right = elseNode;
                        Token checkOpenCurly = PeekToken(file);

                        if (checkOpenCurly.type == OpenCurlyBracket) {
                            ConsumeToken(file);
                            while (true) {
                                Token closeParenTok = PeekToken(file);

                                if (closeParenTok.type == EOFToken ||
                                    closeParenTok.type == CloseCurlyBracket) {
                                    break;
                                }

                                ASTNode *currentLine =
                                    ParseStatement(file, false);

                                if (currentLine != NULL) {
                                    elseNode->children[elseNode->childCount] =
                                        currentLine;
                                    elseNode->childCount++;

                                } else {
                                    ConsumeToken(file);
                                }
                            }
                            ConsumeToken(file);
                        }
                    }
                    if_node->left = conditionNode;
                    if_node->right = conditionEvalNode;

                    return if_node;
                }
            }
        }
    }

    if (token.type == ElifToken) {

        if (isExpectingElif == false) {
            printf("Syntax Error : Cannot Make An Elif Block without an If "
                   "block\n");
            exit(269);

            return NULL;
        }
        ConsumeToken(file);

        ASTNode *elif_node = CreateNode(IfNode, "if");
        ASTNode *conditionEvalNode =
            CreateNode(BlockNode, "ConditionEvaluator");

        conditionEvalNode->right = NULL;
        elif_node->right = conditionEvalNode;

        Token openParenToken = PeekToken(file);

        if (openParenToken.type == OpenParenthesisToken) {
            ConsumeToken(file);

            ASTNode *conditionExpr = ParseExpression(file, 0);
            elif_node->left = conditionExpr;
            Token closeParenToken = PeekToken(file);

            if (closeParenToken.type == CloseParenthesisToken) {
                ConsumeToken(file);
                Token openCurlyTok = PeekToken(file);

                if (openCurlyTok.type == OpenCurlyBracket) {
                    ConsumeToken(file);
                    ASTNode *elifNode = CreateNode(BlockNode, "elif");

                    conditionEvalNode->left = elifNode;
                    while (true) {
                        Token lineTok = PeekToken(file);

                        if (lineTok.type == CloseCurlyBracket ||
                            lineTok.type == EOFToken) {
                            break;
                        }

                        ASTNode *currentLine = ParseStatement(file, false);

                        if (currentLine != NULL) {
                            elifNode->children[elifNode->childCount] =
                                currentLine;
                            elifNode->childCount++;
                        } else {
                            ConsumeToken(file);
                        }
                    }
                    ConsumeToken(file);

                    Token checkElseToken = PeekToken(file);

                    if (checkElseToken.type == ElseToken) {
                        ConsumeToken(file);

                        ASTNode *elseNode = CreateNode(BlockNode, "else");

                        Token openCurlyTok = PeekToken(file);

                        if (openCurlyTok.type == OpenCurlyBracket) {
                            ConsumeToken(file);

                            while (true) {
                                Token lineTok = PeekToken(file);

                                if (lineTok.type == CloseCurlyBracket ||
                                    lineTok.type == EOFToken) {
                                    break;
                                }

                                ASTNode *currentLine =
                                    ParseStatement(file, false);

                                if (currentLine != NULL) {
                                    elseNode->children[elseNode->childCount] =
                                        currentLine;
                                    elseNode->childCount++;
                                } else {
                                    ConsumeToken(file);
                                }
                            }
                            ConsumeToken(file);
                        }
                        conditionEvalNode->right = elseNode;
                    }
                    elif_node->left = conditionExpr;
                    elif_node->right = conditionEvalNode;

                    return elif_node;
                }
            }
        }
    }

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
        printf("Syntax Error: SemiColon expected\n");
        ConsumeToken(file);
    }
    return exprNode;
}
ASTNode *Parse(FILE *file) {
    hasLookAhead = false;

    ASTNode *programRoot = CreateNode(BlockNode, "Program");

    while (true) {
        Token t = PeekToken(file);
        if (t.type == EOFToken) {
            break;
        }
        ASTNode *currentLine = ParseStatement(file, false);
        if (currentLine != NULL) {
            programRoot->children[programRoot->childCount] = currentLine;
            programRoot->childCount++;
        } else {
            ConsumeToken(file);
        }
    }

    return programRoot;
}
