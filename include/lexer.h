#ifndef LEXER_H
#define LEXER_H
#include <stdbool.h>
#include <stdio.h>
#define MAXTOKENLEN 256
#define MAXTOKEN 1000
typedef enum {
    IntToken,
    BinaryOpToken,
    EqualsToken,
    KeyWordToken,
    IdentifierToken,
    UnknownToken,
    OpenParenthesisToken,
    CloseParenthesisToken,
    EOFToken,
    SemiColonToken,
    StringToken,
    BoolToken,
    IfToken,
    ElifToken,
    ElseToken,
    OpenCurlyBracket,
    CloseCurlyBracket,
    ComparisonOpToken,
    FunctionDeclToken
} TokenType;

typedef struct {
    char value[MAXTOKENLEN];
    TokenType type;
} Token;

Token Tokenize(char string[MAXTOKENLEN], bool isString);
Token GetToken(FILE *file);
#endif // !LEXER_H
