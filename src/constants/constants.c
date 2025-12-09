//
// Created by Connor Reed on 12/9/25.
//

#include "constants.h"

Memory memory[MAX] = { 0 };
Memory stackPointer = MAX - 1;
Memory address = 0;
Memory labelTable[26] = { -1 };