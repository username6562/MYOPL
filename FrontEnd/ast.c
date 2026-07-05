#include "ast.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode *CreateNode(NodeType type, const char value[256]) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
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

static char asciiGrid[32][128];
static int maxRowUsed = 0;

static void FillASCIIGrid(ASTNode *node, int row, int col, int spacing) {
    if (node == NULL || row >= 32 || col < 0 || col >= 128) {
        return;
    }

    if (row > maxRowUsed) {
        maxRowUsed = row;
    }

    int len = strlen(node->value);
    int startCol = col - (len / 2);
    for (int i = 0; i < len; i++) {
        if (startCol + i >= 0 && startCol + i < 128) {
            asciiGrid[row][startCol + i] = node->value[i];
        }
    }

    if (node->left != NULL) {
        asciiGrid[row + 1][col - (spacing / 2)] = '/';
        FillASCIIGrid(node->left, row + 2, col - spacing, spacing / 2);
    }

    if (node->right != NULL) {
        asciiGrid[row + 1][col + (spacing / 2)] = '\\';
        FillASCIIGrid(node->right, row + 2, col + spacing, spacing / 2);
    }
}

void PrintASTASCII(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    // 1. If it is a block container, loop through and print each individual
    // line tree
    if (node->type == BlockNode) {
        for (int i = 0; i < node->childCount; i++) {
            if (node->children[i] == NULL) {
                continue;
            }

            // Clear the 2D workspace for the current line
            maxRowUsed = 0;
            for (int r = 0; r < 32; r++) {
                for (int c = 0; c < 128; c++) {
                    asciiGrid[r][c] = ' ';
                }
            }

            // Plot the current syntax tree structure into the grid array
            FillASCIIGrid(node->children[i], 0, 64, 12);

            // Flush the completed grid layout to the terminal window
            for (int r = 0; r <= maxRowUsed + 1; r++) {
                int lastCharCol = 127;
                while (lastCharCol >= 0 && asciiGrid[r][lastCharCol] == ' ') {
                    lastCharCol--;
                }

                for (int c = 0; c <= lastCharCol; c++) {
                    putchar(asciiGrid[r][c]);
                }
                putchar('\n');
            }
            printf("--------------------------------------------------\n");
        }
        return; // Exit completely so it doesn't fall through to single-tree
                // printing
    }

    // 2. Fallback execution path for rendering a single standalone math node
    maxRowUsed = 0;
    for (int r = 0; r < 32; r++) {
        for (int c = 0; c < 128; c++) {
            asciiGrid[r][c] = ' ';
        }
    }

    FillASCIIGrid(node, 0, 64, 12);

    for (int r = 0; r <= maxRowUsed + 1; r++) {
        int lastCharCol = 127;
        while (lastCharCol >= 0 && asciiGrid[r][lastCharCol] == ' ') {
            lastCharCol--;
        }

        for (int c = 0; c <= lastCharCol; c++) {
            putchar(asciiGrid[r][c]);
        }
        putchar('\n');
    }
}
