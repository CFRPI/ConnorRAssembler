//
// Created by Connor Reed on 12/9/25.
//

#include <stdio.h>
#include "run.h"

#include <stdlib.h>

#include "../constants/constants.h"
#include "../map/map.h"

/********************   runMachineCode   ***********************
Executes the machine code that is in memory, the virtual machine
Parameters: none
Return Value: none
-----------------------------------------------------------*/
void runMachineCode() {
    Memory mask1 = 0xFF00; //111 00 000
    Memory mask2 = 0x00F0; //000 11 000
    Memory mask3 = 0x000F; //000 00 111
    Memory part1, part2, part3; //command, operand1,

    address = 0;
    Memory fullCommand = memory[address]; // read the first command from memory
    address++;
    while (part1 != HALT) {
        printf("running %d\n", fullCommand);
        // parts of the command
        part1 = (fullCommand & mask1) >> 8;
        part2 = (fullCommand & mask2) >> 4;
        part3 = fullCommand & mask3;
        if (part1 == MOVREG) {
            Memory value = getValue(part3);
            putValue(part2, value);
        } else if (part1 == MOVMEM) {
            Memory value = getValue(part2);
            putValue(part3, value);
        } else if (part1 == ADDI) {
            // the sum of the reg and reg/const to be moved into part2
            Memory sum = getValue(part2) + getValue(part3); // sum of two operands
            putValue(part2, sum); // write to target register
        } else if (part1 == SUBI) {
            // the sum of the reg and reg/const to be moved into part2
            Memory sum = getValue(part2) - getValue(part3); // sum of two operands
            putValue(part2, sum); // write to target register
        } else if (part1 == CMP) {
            // get the value at the register operand1 and the value operand2
            int operand1 = getValue(part2); //
            int operand2 = getValue(part3);

            if (operand1 > operand2) {
                regis.flag = 1;
            } else if (operand1 < operand2) {
                regis.flag = -1;
            } else {
                // operand1 == operand2
                regis.flag = 0;
            }
        } else if ((part1 & ANYJUMP) > 0) {
            runJumpCommand(part1);
            // we dont go to the next command after a jump
            // instead we go to wherever the jump command states
        } else if (part1 == FUN) {
            // the address of the function
            int functionAddress = memory[address];
            address++;
            // the address where the number of arguments are stored
            int numArgsAddress = address;

            memory[functionAddress - 1] = numArgsAddress;
            address = functionAddress;

            Memory numArgs = memory[numArgsAddress];
            Memory returnAddress = numArgsAddress + numArgs + 2;

            push(regis.AX);
            push(regis.BX);
            push(regis.CX);
            push(regis.DX);
            push(regis.flag);
            push(returnAddress);
        } else if (part1 == RET) {
            // return value
            address = pop();
            memory[address - 1] = regis.AX; // return value

            // restore values
            regis.flag = pop();
            regis.DX = pop();
            regis.CX = pop();
            regis.BX = pop();
            regis.AX = pop();
        } else if (part1 == PUT) // PUT command is in the last 3 bits
        {
            printf("		REG AX: %d\n", regis.AX);
        } else if (part1 == GET) {
            int input = 0;
            printf("Enter an Integer > ");
            scanf("%d", &input);
            regis.AX = input;
        }

        fullCommand = memory[address]; //the next command
        //debugging, comment out when you don't need it
        // printMemoryDumpHex( );
        address++; // next command
    }
}

/*
 * Pushes a value onto the stack and decrements the stack pointer
 *
 * Arguments:
 *  - value: the value to push onto the stack
 *
 * Return: void
 */
void push(Memory value) {
    memory[stackPointer--] = value;
}

/*
 * pops a value off of the stack and increments the stack pointer
 *
 * Parameters: none
 * Return: the value popped from the stack
 */
Memory pop() {
    return memory[++stackPointer];
}


/*
 * Runs any jump command from the machine code
 * it handles the logic of which command is being called
 * and whether it should jump or not
 *
 * Parameters:
 * command - the command of which jump it is
 *
 * Return Value - void
 */
void runJumpCommand(Memory command) {
    Memory targetAddress = memory[address];
    address++;

    int performJump = 0;
    if (command == JMP) {
        performJump = 1; // JMP always jumps regardless
    } else if (command == JA && regis.flag == 1) {
        performJump = 1; // JA jumps when flag is 1
    } else if (command == JE && regis.flag == 0) {
        performJump = 1; // JE jumps when flag is 0
    } else if (command == JB && regis.flag == -1) {
        performJump = 1; // JB jumps when flag is -1
    } else if (command == JAE && regis.flag >= 0) {
        performJump = 1; // JAE jumps when flag is 0 or 1
    } else if (command == JBE && regis.flag <= 0) {
        performJump = 1; // JBE jumps when flag is 0 or -1
    } else if (command == JNE && regis.flag != 0) {
        performJump = 1; // JE jumps when flag is not 0
    }

    if (performJump == 1) {
        address = targetAddress;
    }
}

/*********************************************************************************
/****************************   HELPER FUNCTIONS   *******************************
/*********************************************************************************/

/*
 * Gets a value based on a binary code for a register or constant
 *
 * operand - the value of an operand, binary for a constant or register
 * Return Value - the value of the register or the value of the current memory address for constant
 *
 * Note: if this function gets a constant, it increments address to step over the value
 */
Memory getValue(int operand) {
    switch (operand) {
        // read the value of either the register or constant specified
        case AXREG:
            return (Memory) regis.AX;
        case BXREG:
            return (Memory) regis.BX;
        case CXREG:
            return (Memory) regis.CX;
        case DXREG:
            return (Memory) regis.DX;
        case ADDRESS:
            address++; // step over the value to retrieve
            Memory ptr = memory[address - 1]; // the pointer to the memory address to retrieve
            return memory[ptr]; // get the memory value at ptr
        case CONSTANT:
            address++;
            return memory[address - 1];
        case BXADDR:
            return memory[regis.BX];
        case BXPLUS:
            address++;
            Memory offset = memory[address - 1];
            return memory[regis.BX + offset];
        default: // nonexistent register
            printf("Unknown register: %d at address %d", reg, address);
            system("pause");
            exit(1);
    }
}

/*
 * Puts a value into a register
 *
 * reg - the binary code of the register who's value we will set
 * value - the value to put into reg
 *
 * Return Value - void
 */
void putValue(int reg, Memory value) {
    // move value into the proper register
    switch (reg) {
        case AXREG:
            regis.AX = value;
            break;
        case BXREG:
            regis.BX = value;
            break;
        case CXREG:
            regis.CX = value;
            break;
        case DXREG:
            regis.DX = value;
            break;
        case ADDRESS:
            address++; // step over the memory address to store at
            Memory ptr = memory[address - 1]; // a pointer to where to store value
            memory[ptr] = value;
            break;
        case BXADDR:
            memory[regis.BX] = value;
            break;
        case BXPLUS:
            address++; // step over the offset
            Memory offset = memory[address - 1];
            memory[regis.BX + offset] = value;
            break;
        default: // if the machine code tells it to put it into a non-existent register
            printMemoryDump();
            printf("Error, register %d not recognized address %d", reg, address);
            system("pause");
            exit(1);
    }
}

/****************************   printMemoryDump   ********************************
prints memory by number
MAX is the amount of elements in the memory array (Vicki used 100)
COL is the number of columns that are to be displayed (Vicki used 7)
parameters: none
return value: none
---------------------------------------------------------------------------------*/
void printMemoryDump() {
    int numRows = MAX / COL + 1; //number of rows that will print
    int carryOver = MAX % COL; //number of columns on the bottom row
    int location; //the current location being called
    for (int row = 0; row < numRows; row++) {
        location = row;
        for (int column = 0; location < MAX && column < COL; column++) {
            if (!(numRows - 1 == row && carryOver - 1 < column)) {
                printf("%5d.%5d", location, memory[location]);
                location += (numRows - (carryOver - 1 < column));
            }
        }
        printf("\n");
    }
    printf("\nAX:%d\t", regis.AX);
    printf("BX:%d\t", regis.BX);
    printf("CX:%d\t", regis.CX);
    printf("DX:%d\n\n", regis.DX);
    printf("Address: %d\n", address);
    printf("Flag: %d\n", regis.flag);
    printf("===Labels Table===\n");

    printLabels();

    printf("\n");
    printf("\n");
}


/*********************   printMemoryDumpHex   ********************
Prints memory in
*parameters: none
*return value: none
----------------------------------------------------------------*/
void printMemoryDumpHex() {
    int numRows = MAX / COL + 1; //number of rows that will print
    int carryOver = MAX % COL; //number of columns on the bottom row
    int location; //the current location being called
    for (int row = 0; row < numRows; row++) {
        location = row;
        for (int column = 0; location < MAX && column < COL; column++) {
            if (!(numRows - 1 == row && carryOver - 1 < column)) {
                printf("%5d.%3x", location, memory[location]);
                location += (numRows - (carryOver - 1 < column));
            }
        }
        printf("\n");
    }
    printf("\nAX:%d\t", regis.AX);
    printf("BX:%d\t", regis.BX);
    printf("CX:%d\t", regis.CX);
    printf("DX:%d\n", regis.DX);
    printf("Address: %d\n", address);
    printf("Flag: %d\n\n", regis.flag);
}