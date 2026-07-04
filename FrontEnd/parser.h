#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include <stdio.h>

ASTNode *Parse(FILE *file);
Token PeekToken(FILE *file);
ASTNode *ParsePrimary(FILE *file);
#endif // PARSER_H;
