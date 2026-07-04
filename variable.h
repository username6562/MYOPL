#ifndef VARIABLE_H
#define VARIABLE_H

typedef struct {
    char name[256];
    int value;
} Variable;

void SetVariable(char name[256], int value);

#endif
