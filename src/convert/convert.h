//
// Created by Connor Reed on 12/9/25.
//

#ifndef ASSEMBLER_CONVERT_H
#define ASSEMBLER_CONVERT_H

#include "../constants/constants.h"

void splitCommand(char line[], char instruction[], char operand1[], char operand2[]); // splits a command into its parts
void convertToMachineCode(FILE *fin, int pass); // Converts a single line of ASM to machine code
void readInstructionPart(char line[], char part[], int *index); // reads the next part of a line of assembly
void convertJumpToMachineCode(char *part1, char *part2, char *part3); // converts any jump command to machine code
void convertFunctionToMachineCode(char line[LINE_SIZE]); // converts function calls to machine code
void changeToLowerCase(char line[]); // Changes each character to lower case
int whichOpperand(char operand[]); // Returns the number of the letter register
Memory convertToNumber(char line[], int start); // converts a sub-string to an int
void handleBXPlus(char part3[LINE_SIZE]); // adds a bxplus field to machine code

#endif //ASSEMBLER_CONVERT_H