#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool isInteger(char src[MAXTOKENLEN]) {
    if (src[0] == '\0') {
        return false;
    }
    for (int i = 0; src[i] != '\0'; i++) {
        if (!isdigit(((unsigned char)src[i]))) {
            return false;
        }
    }

    return true;
}

Token Tokenize(char string[MAXTOKENLEN], bool isString) {
    Token t;
    strncpy(t.value, string, MAXTOKENLEN - 1);
    t.value[MAXTOKENLEN - 1] = '\0';
    static bool expectingIdentifier;

    if (isString) {
        t.type = StringToken;
        expectingIdentifier = false;

    } else if (strcmp(t.value, "+") == 0) {
        t.type = BinaryOpToken;
        expectingIdentifier = false;
    }

    else if (strcmp(t.value, "=") == 0) {
        t.type = EqualsToken;
        expectingIdentifier = false;
    } else if (strcmp(t.value, "-") == 0 || strcmp(t.value, "*") == 0 ||
               strcmp(t.value, "/") == 0) {
        t.type = BinaryOpToken;
        expectingIdentifier = false;
    } else if (isInteger(t.value)) {
        t.type = IntToken;
        expectingIdentifier = false;
    } else if (strcmp(t.value, "int") == 0 || strcmp(t.value, "char") == 0) {
        t.type = KeyWordToken;
        expectingIdentifier = true;
    } else if (expectingIdentifier && isalpha(t.value[0])) {
        t.type = IdentifierToken;
        expectingIdentifier = false;
    } else if (strcmp(t.value, "(") == 0) {
        t.type = OpenParenthesis;
        expectingIdentifier = false;
    } else if (strcmp(t.value, ")") == 0) {
        t.type = CloseParenthesis;
        expectingIdentifier = false;
    } else if (strcmp(t.value, ";") == 0) {
        t.type = SemiColonToken;
        expectingIdentifier = false;
    } else {
        t.type = IdentifierToken;
    }

    return t;
}

Token GetToken(FILE *file) {

    if (file == NULL) {
        printf("Error couldn't open file\n");
    }
    Token tokens[MAXTOKEN];

    int tokenCount = 0;

    int ch = fgetc(file);

    while (ch != EOF) {
        if (isspace(ch)) {
            ch = fgetc((file));
            continue;
        }

        char lexerBuffer[MAXTOKENLEN] = "";
        int bufferIndex = 0;
        if (isdigit(ch)) {
            while (isdigit(ch)) {
                lexerBuffer[bufferIndex++] = ch;
                ch = fgetc(file);
            }

            lexerBuffer[bufferIndex] = '\0';
            ungetc(ch, file);
            return Tokenize(lexerBuffer, false);
        } else if (ch == '+' || ch == '=' || ch == '(' || ch == ')' ||
                   ch == '-' || ch == ';') {
            lexerBuffer[0] = ch;
            lexerBuffer[1] = '\0';
            ch = fgetc(file);
            ungetc(ch, file);
            return Tokenize(lexerBuffer, false);
        } else if (isalpha(ch)) {
            while (isalpha(ch)) {
                lexerBuffer[bufferIndex++] = ch;
                ch = fgetc(file);
            }
            lexerBuffer[bufferIndex] = '\0';
            ungetc(ch, file);

            return Tokenize(lexerBuffer, false);
        } else if (ch == '\'' || ch == '\"') {
            while (ch != '\"' || ch != '\'') {
                lexerBuffer[bufferIndex] = ch;
                ch = fgetc(file);
            }
            lexerBuffer[bufferIndex] = '\0';
            ungetc(ch, file);
            return Tokenize(lexerBuffer, true);
        }

        else {
            lexerBuffer[0] = ch;
            lexerBuffer[1] = '\0';
            ch = fgetc(file);
            ungetc(ch, file);
            return Tokenize(lexerBuffer, false);
        }
        tokenCount++;
    }
    Token eofToken;
    eofToken.type = EOFToken;
    eofToken.value[0] = '\0';
    return eofToken;
}
