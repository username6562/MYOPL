#include "ast.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANVAS_ROWS 35
#define CANVAS_COLS 120

ASTNode *CreateNode(NodeType type, const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;

    node->left = NULL;
    node->right = NULL;
    node->childCount = 0;
    if (value != NULL) {
        strncpy(node->value, value, sizeof(node->value));
        node->value[sizeof(node->value) - 1] = '\0';
    } else {
        node->value[0] = '\0';
    }

    // Initialize all child statement pointers to prevent memory corruption
    for (int i = 0; i < 100; i++) {
        node->children[i] = NULL;
    }

    return node;
}
