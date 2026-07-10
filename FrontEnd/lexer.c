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
    } else if (strcmp(t.value, "int") == 0 || strcmp(t.value, "string") == 0 ||
               strcmp(t.value, "bool") == 0) {
        t.type = KeyWordToken;
        expectingIdentifier = true;
    } else if (expectingIdentifier && isalpha(t.value[0])) {
        t.type = IdentifierToken;
        expectingIdentifier = false;
    } else if (strcmp(t.value, "true") == 0 || strcmp(t.value, "false") == 0) {
        t.type = BoolToken;
        expectingIdentifier = false;
    } else if (strcmp(t.value, "if") == 0) {
        t.type = IfToken;
        expectingIdentifier = false;

    } else if (strcmp(t.value, "(") == 0) {
        t.type = OpenParenthesisToken;
        expectingIdentifier = false;
    } else if (strcmp(t.value, ")") == 0) {
        t.type = CloseParenthesisToken;
        expectingIdentifier = false;
    } else if (strcmp(t.value, ";") == 0) {
        t.type = SemiColonToken;
        expectingIdentifier = false;
    } else if (strcmp(t.value, "{") == 0) {
        t.type = OpenCurlyBracket;
    } else if (strcmp(t.value, "}") == 0) {
        t.type = CloseCurlyBracket;
    } else if (strcmp(t.value, "else") == 0) {
        t.type = ElseToken;
    } else if (strcmp(t.value, "==") == 0 || strcmp(t.value, "<") == 0 ||
               strcmp(t.value, ">") == 0) {
        t.type = ComparisonOpToken;
    }

    else {
        t.type = IdentifierToken;
    }

    return t;
}

Token GetToken(FILE *file) {

    if (file == NULL) {
        printf("Error couldn't open file\n");
    }

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
        } else if (ch == '+' || ch == '(' || ch == ')' || ch == '-' ||
                   ch == ';') {
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
        } else if (ch == '=') {
            int nextCh = fgetc(file);

            if (nextCh == '=') {
                Token tok;
                tok.type = ComparisonOpToken;
                strcpy(tok.value, "==");

                return tok;
            } else {
                ungetc(nextCh, file);
                Token tok;

                tok.type = EqualsToken;
                strcpy(tok.value, "=");
            }
        } else if (ch == '\'' || ch == '\"') {
            char quote = ch;
            ch = fgetc(file);
            while (ch != quote && ch != EOF) {
                lexerBuffer[bufferIndex++] = ch;
                ch = fgetc(file);
            }

            lexerBuffer[bufferIndex] = '\0';
            if (ch == quote) {
                ch = fgetc(file);
            }
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
