#include "ast.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANVAS_ROWS 35
#define CANVAS_COLS 120

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

static void DrawNodesOnCanvas(ASTNode *node,
                              char canvas[CANVAS_ROWS][CANVAS_COLS], int row,
                              int col, int spacing, const char *overrideTxt) {
    if (node == NULL || row >= CANVAS_ROWS || col < 0 || col >= CANVAS_COLS)
        return;

    // 1. FORMAT VALUES INTO EXPLICIT GEOMETRIC BOX FRAMES
    char txt[64] = "";
    if (overrideTxt != NULL) {
        strcpy(txt, overrideTxt); // 🌟 FORCE CUSTOM LABELS ([ true ] / [ false
                                  // ]) WHEN PASSED!
    } else if (node->type == IfNode) {
        strcpy(txt, "[ if ]");
    } else if (node->type == AssignmentOpNode) {
        strcpy(txt, "[ = ]");
    } else if (node->type == AddNode || node->type == SubtractNode ||
               node->type == MultiplyNode || node->type == DivideNode) {
        snprintf(txt, sizeof(txt), "[ %s ]", node->value);
    } else if (node->type == NumberNode) {
        snprintf(txt, sizeof(txt), "( %s )", node->value);
    } else if (node->type == IdentifierNode) {
        snprintf(txt, sizeof(txt), "[ %s ]", node->value);
    } else if (node->type == BoolNode) {
        snprintf(txt, sizeof(txt), "( %s )", node->value);
    } else if (node->type == BlockNode) {
        snprintf(txt, sizeof(txt), "[ Helper: %s ]", node->value);
    } else {
        snprintf(txt, sizeof(txt), "[ %s ]", node->value);
    }

    int len = strlen(txt);
    int startCol = col - (len / 2);

    // Copy characters directly into active row coordinates
    for (int i = 0; i < len; i++) {
        if (startCol + i >= 0 && startCol + i < CANVAS_COLS) {
            canvas[row][startCol + i] = txt[i];
        }
    }

    // 2. DEFINE DEEP CODE PATH BRANCH LANES
    ASTNode *leftArm = node->left;
    ASTNode *rightArm = node->right;

    // 🌟 SECTOR A: Master IfNode parent coordinate splitter
    if (node->type == IfNode && rightArm != NULL &&
        strcmp(rightArm->value, "ConditionEvaluator") == 0) {

        // Draw Left branch condition expression under standard slash marker
        if (leftArm != NULL) {
            int slashCol = col - (spacing / 2);
            if (slashCol >= 0 && slashCol < CANVAS_COLS)
                canvas[row + 1][slashCol] = '/';
            DrawNodesOnCanvas(leftArm, canvas, row + 2, col - spacing,
                              (spacing / 2 == 0) ? 1 : spacing / 2, NULL);
        }

        // Draw Right branch helper block node straight under backslash marker
        if (rightArm != NULL) {
            int backslashCol = col + (spacing / 2);
            if (backslashCol >= 0 && backslashCol < CANVAS_COLS)
                canvas[row + 1][backslashCol] = '\\';
            DrawNodesOnCanvas(rightArm, canvas, row + 2, col + spacing,
                              (spacing / 2 == 0) ? 1 : spacing / 2, NULL);
        }
        return; // Exit early since our custom parent structural split is
                // handled
    }

    // 🌟 SECTOR B: Specialized ConditionEvaluator split block router (REPLACES
    // [ = ] WITH [ true ] / [ false ])
    if (node->type == BlockNode &&
        strcmp(node->value, "ConditionEvaluator") == 0) {
        ASTNode *ifBodyBlock = node->left;
        ASTNode *elseBodyBlock = node->right;

        // Trace Left Branch (Unpack inner statement and label the block box as
        // [ true ])
        if (ifBodyBlock != NULL && ifBodyBlock->childCount > 0) {
            leftArm = ifBodyBlock->children[0];

            int slashCol = col - (spacing / 2);
            if (slashCol >= 0 && slashCol < CANVAS_COLS)
                canvas[row + 1][slashCol] = '/';

            // 🔍 FIX: Pass "[ true ]" string override parameter to clear out
            // the [ = ] label!
            DrawNodesOnCanvas(leftArm, canvas, row + 2, col - spacing,
                              (spacing / 2 == 0) ? 1 : spacing / 2, "[ true ]");
        }

        // Trace Right Branch (Unpack inner statement and label the block box as
        // [ false ])
        if (elseBodyBlock != NULL && elseBodyBlock->childCount > 0) {
            rightArm = elseBodyBlock->children[0];

            int backslashCol = col + (spacing / 2);
            if (backslashCol >= 0 && backslashCol < CANVAS_COLS)
                canvas[row + 1][backslashCol] = '\\';

            // 🔍 FIX: Pass "[ false ]" string override parameter to clear out
            // the [ = ] label!
            DrawNodesOnCanvas(rightArm, canvas, row + 2, col + spacing,
                              (spacing / 2 == 0) ? 1 : spacing / 2,
                              "[ false ]");
        }

        return; // Exit early since sub-branches are manually mapped and
                // explicitly overridden
    }

    // 3. GENERAL FALLBACK ROUTING PATHS (For arithmetic chains and assignment
    // operations)
    if (leftArm != NULL) {
        int slashCol = col - (spacing / 2);
        if (slashCol >= 0 && slashCol < CANVAS_COLS) {
            canvas[row + 1][slashCol] = '/';
        }
        DrawNodesOnCanvas(leftArm, canvas, row + 2, col - spacing,
                          (spacing / 2 == 0) ? 1 : spacing / 2, NULL);
    }
    if (rightArm != NULL) {
        int backslashCol = col + (spacing / 2);
        if (backslashCol >= 0 && backslashCol < CANVAS_COLS) {
            canvas[row + 1][backslashCol] = '\\';
        }
        DrawNodesOnCanvas(rightArm, canvas, row + 2, col + spacing,
                          (spacing / 2 == 0) ? 1 : spacing / 2, NULL);
    }
}

// 🏁 Master Interface Function called from main.c
void PrintAST(ASTNode *root) {
    if (root == NULL)
        return;

    if (root->type == BlockNode && strcmp(root->value, "Program") == 0) {
        printf("\n--- Abstract Syntax Tree Diagram Canvas ---\n");
        for (int i = 0; i < root->childCount; i++) {
            char canvas[CANVAS_ROWS][CANVAS_COLS];
            memset(canvas, ' ', sizeof(canvas)); // Reset grid spaces completely

            // Load and plot child elements starting from coordinates 0, center
            // with no override
            DrawNodesOnCanvas(root->children[i], canvas, 0, CANVAS_COLS / 2, 20,
                              NULL);

            // Compute dynamic height bounds to hide trailing empty lines
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

            // Flush out the active populated matrix rows to screen terminal
            // canvas
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
