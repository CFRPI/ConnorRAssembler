//
// Created by Connor Reed on 12/9/25.
//

#ifndef ASSEMBLER_MAP_H
#define ASSEMBLER_MAP_H

#include "../constants/constants.h"

int getIndex(char key[MAX_LABEL_SIZE]);
void insertLabel(char key[MAX_LABEL_SIZE], Memory value);
Memory getLabel(char key[MAX_LABEL_SIZE]);
void printLabels();

#endif //ASSEMBLER_MAP_H