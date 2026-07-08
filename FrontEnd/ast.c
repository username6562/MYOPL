#include "ast.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANVAS_ROWS 20
#define CANVAS_COLS 80

ASTNode *CreateNode(NodeType type, const char value[256]) {
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

// 🔍 Complete 2D Plotting Engine
static void DrawNodesOnCanvas(ASTNode *node,
                              char canvas[CANVAS_ROWS][CANVAS_COLS], int row,
                              int col, int spacing) {
    if (node == NULL || row >= CANVAS_ROWS || col < 0 || col >= CANVAS_COLS)
        return;

    // 1. Format the node value string to print inside map cells
    char txt[16] = "";
    if (node->type == IfNode)
        strcpy(txt, "if");
    else if (node->type == AssignmentOpNode)
        strcpy(txt, "=");
    else if (node->type == AddNode)
        strcpy(txt, "+");
    else if (node->type == SubtractNode)
        strcpy(txt, "-");
    else if (node->type == MultiplyNode)
        strcpy(txt, "*");
    else if (node->type == DivideNode)
        strcpy(txt, "/");
    else
        strncpy(txt, node->value, 15);

    int len = strlen(txt);
    int startCol = col - (len / 2);

    // Write text directly into the canvas row matrix
    for (int i = 0; i < len; i++) {
        if (startCol + i >= 0 && startCol + i < CANVAS_COLS) {
            canvas[row][startCol + i] = txt[i];
        }
    }

    // 2. Setup left and right branch paths cleanly
    ASTNode *leftArm = node->left;
    ASTNode *rightArm = node->right;

    // 🌟 THE FIX: If this is an 'if' node, skip the BlockNode wrapper
    // and jump straight to the real statement line sitting inside index 0!
    if (node->type == IfNode && rightArm != NULL &&
        rightArm->type == BlockNode) {
        if (rightArm->childCount > 0) {
            rightArm = rightArm->children[0];
        } else {
            rightArm = NULL; // Block is empty, print nothing on right
        }
    }

    // 3. Draw diagonal connection branch lines
    if (leftArm != NULL) {
        int slashCol = col - (spacing / 2);
        if (slashCol >= 0 && slashCol < CANVAS_COLS) {
            canvas[row + 1][slashCol] = '/';
        }
        DrawNodesOnCanvas(leftArm, canvas, row + 2, col - spacing,
                          (spacing / 2 == 0) ? 1 : spacing / 2);
    }
    if (rightArm != NULL) {
        int backslashCol = col + (spacing / 2);
        if (backslashCol >= 0 && backslashCol < CANVAS_COLS) {
            canvas[row + 1][backslashCol] = '\\';
        }
        DrawNodesOnCanvas(rightArm, canvas, row + 2, col + spacing,
                          (spacing / 2 == 0) ? 1 : spacing / 2);
    }
}

// 🏁 Master Function called from your main file
void PrintAST(ASTNode *root) {
    if (root == NULL)
        return;

    if (root->type == BlockNode && strcmp(root->value, "Program") == 0) {
        printf("\n--- Abstract Syntax Tree Diagram Canvas ---\n");
        for (int i = 0; i < root->childCount; i++) {
            char canvas[CANVAS_ROWS][CANVAS_COLS];
            memset(canvas, ' ', sizeof(canvas)); // Clear board with spaces

            // Plot the node structures from center column row 0
            DrawNodesOnCanvas(root->children[i], canvas, 0, CANVAS_COLS / 2,
                              14);

            // Find max active height row boundary
            int maxRowWithContent = 0;
            for (int r = CANVAS_ROWS - 1; r >= 0; r--) {
                for (int c = 0; c < CANVAS_COLS; c++) {
                    if (canvas[r][c] != ' ') {
                        maxRowWithContent = r;
                        break;
                    }
                }
                if (maxRowWithContent > 0)
                    break;
            }

            // Print the populated grid map out to the screen
            for (int r = 0; r <= maxRowWithContent; r++) {
                int endCol = CANVAS_COLS - 1;
                while (endCol >= 0 && canvas[r][endCol] == ' ')
                    endCol--;

                for (int c = 0; c <= endCol; c++) {
                    putchar(canvas[r][c]);
                }
                putchar('\n');
            }
            printf("\n--------------------------------------------\n");
        }
        return;
    }
}
