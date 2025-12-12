//
// Created by Connor Reed on 12/9/25.
//

#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int size = 0;
char labelsKeys[MAX_LABELS][MAX_LABEL_SIZE];
Memory labelsValues[MAX_LABELS];

int getIndex(char key[MAX_LABEL_SIZE]) {
    for (int i = 0; i < MAX_LABELS; i++) {
        if (strcmp(labelsKeys[i], key) == 0) {
            return i;
        }
    }

    return -1;
}

void insertLabel(char key[MAX_LABEL_SIZE], Memory value) {
    int index = getIndex(key);

    if (index == -1) {
        // element does not exist, insert it at end
        strcpy(labelsKeys[size], key);
        labelsValues[size] = value;
        size++;
    } else {
        labelsValues[index] = value;
    }
}

Memory getLabel(char key[MAX_LABEL_SIZE]) {
    int index = getIndex(key);

    if (index != -1) {
        return labelsValues[index];
    }

    Memory noResult;
    noResult.integer = -1;
    return noResult;
}

void printLabels() {
    for (int i = 0; i < size; i++) {
        printf("> %s - %d\n", labelsKeys[i], labelsValues[i]);
    }
}