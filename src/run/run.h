//
// Created by Connor Reed on 12/9/25.
//

#ifndef ASSEMBLER_RUN_H
#define ASSEMBLER_RUN_H

#include "../constants/constants.h"

void printMemoryDump(); // Prints memory with commands represented as integers
void printMemoryDumpHex(); // Prints memory in hexadecimal
void putValue(int reg, Memory value); // puts a value into a register
Memory getValue(int operand); // gets a value from a register or constant at address
void push(Memory value); // pushes a value onto the stack
Memory pop(); // pops a value off of the stack
void runMachineCode(); // Executes the machine code
void runJumpCommand(Memory command); // runs any jump command

#endif //ASSEMBLER_RUN_H