// name: Connor Reed
// CSC2025 Assembler Project
// date: 11/25/25
// i/o files: part8CR.asm, part8TestCR.asm, part8class11_10Wyatt.asm, part8classLoopWyatt.asm
// description: Reads an assembly file and generates machine code then executes that machine code on a virtual machine
// currently implemented: Mov with registers, constants, and memory, halt, add, put, get, all jumps

#define _CRT_SECURE_NO_WARNINGS  // lets us use deprecated code

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "convert/convert.h"
#include "constants/constants.h"
#include "map/map.h"
#include "run/run.h"

// part7CR.asm sums up 10 numbers with a loop
// part7FibCR.asm calculates the inputted amount of Fibonacci numbers
char ASM_FILE_NAME[] = "test.asm";

//function prototypes
void assembler(); // Converts the entire ASM file and stores it in memory

int main() {
    stackPointer = MAX - 1;
    address = 0;

    assembler();
    printf("=================================================\n");
    printf("Memory after program is converted to machine code\n");
    printf("=================================================\n");
         // allocate memory for registers
    printMemoryDump();
    runMachineCode();

    printf("================================\n");
    printf("Memory after program is finished\n");
    printf("================================\n");

    printMemoryDump(); //displays memory with final values

    printf("\n");
    system("pause");
    return 0;
}

/********************   assembler   ***********************
changes the assembly code to machine code and places the
commands into the memory.
parameters: none
return value: none
-----------------------------------------------------------*/
void assembler() {
    address = 0;
    FILE *fin; // File pointer for reading in the assembly code.
    //recommend changing so you can type in file name
    fin = fopen(ASM_FILE_NAME, "r");
    if (fin == NULL) {
        printf("Error, file didn't open\n\nExiting program...\n\n");
        system("pause");
        exit(1);
    }

    // pass 0
    for (int i = 0; i < MAX && !feof(fin); i++) {
        convertToMachineCode(fin, 0);
    }

    rewind(fin); // reset file pointer
    address = 0; // reset address
    // pass 1
    for (int i = 0; i < MAX && !feof(fin); i++) {
        convertToMachineCode(fin, 1);
    }
}

/*********************   changeToLowerCase   ********************
Changes each character to lower case
*parameters:
* line - the string that was entered the line is completely changed to lower case
* return value: none
----------------------------------------------------------------*/
void changeToLowerCase(char line[]) {
    int index = 0;
    while (index < strlen(line)) {
        line[index] = tolower(line[index]);
        index++;
    }
}