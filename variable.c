#include "variable.h"

static int variableCount;
static Variable variableTable[100];

void SetVariable(char name[256], int value) {
    variableTable[variableCount] =
        (Variable){.name = name[256], .value = value};
    variableCount++;
}
