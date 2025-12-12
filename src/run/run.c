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
    short int mask1 = 0xFF00; //111 00 000
    short int mask2 = 0x00F0; //000 11 000
    short int mask3 = 0x000F; //000 00 111
    short int part1, part2, part3; //command, operand1,

    address = 0;
    Memory fullCommand = memory[address]; // read the first command from memory
    address++;
    while (part1 != HALT) {
        // parts of the command
        part1 = (fullCommand.integer & mask1) >> 8;
        part2 = (fullCommand.integer & mask2) >> 4;
        part3 = fullCommand.integer & mask3;
        if (part1 == MOVREG) {
            Memory value = getValue(part3);
            putValue(part2, value);
        } else if (part1 == MOVMEM) {
            Memory value = getValue(part2);
            putValue(part3, value);
        } else if (part1 == ADDI) {
            // the sum of the reg and reg/const to be moved into part2
            Memory sum;
            sum.integer = getValue(part2).integer + getValue(part3).integer; // sum of two operands
            putValue(part2, sum); // write to target register
        } else if (part1 == SUBI) {
            // the sum of the reg and reg/const to be moved into part2
            Memory diff;
            diff.integer = getValue(part2).integer - getValue(part3).integer; // sum of two operands
            putValue(part2, diff); // write to target register
        } else if (part1 == ADDR) {
            // the sum of the reg and reg/const to be moved into part2
            Memory sum;
            sum.real = getValue(part2).real + getValue(part3).real; // sum of two operands
            putValue(part2, sum); // write to target register
        } else if (part1 == SUBR) {
            // the sum of the reg and reg/const to be moved into part2
            Memory diff;
            diff.real = getValue(part2).real - getValue(part3).real; // sum of two operands
            putValue(part2, diff); // write to target register
        } else if (part1 == CMP) {
            // get the value at the register operand1 and the value operand2
            int operand1 = getValue(part2).integer; //
            int operand2 = getValue(part3).integer;

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
            int functionAddress = memory[address].integer;
            address++;
            // the address where the number of arguments are stored
            Memory numArgsAddress;
            numArgsAddress.integer = address;

            memory[functionAddress - 1] = numArgsAddress;
            address = functionAddress;

            Memory numArgs = memory[numArgsAddress.integer];
            Memory returnAddress;
            returnAddress.integer = numArgsAddress.integer + numArgs.integer + 2;

            push(regis.AX);
            push(regis.BX);
            push(regis.CX);
            push(regis.DX);
            Memory flagMemoryValue;
            flagMemoryValue.integer = regis.flag;
            push(flagMemoryValue);
            push(returnAddress);
        } else if (part1 == RET) {
            // return value
            address = pop().integer;
            memory[address - 1] = regis.AX; // return value

            // restore values
            regis.flag = pop().integer;
            regis.DX = pop();
            regis.CX = pop();
            regis.BX = pop();
            regis.AX = pop();
        } else if (part1 == PUT) // PUT command is in the last 3 bits
        {
            printf("%d\n", regis.AX.integer);
        } else if (part1 == PUTR) {
            printf("%f\n", (float) regis.GX.real);
        } else if (part1 == GET) {
            int input = 0;
            printf("Enter an Integer > ");
            scanf("%d", &input);
            regis.AX.integer = input;
        } else if (part1 == ADDRV) {
            putValue(part2, memory[address]);
        } else if (part1 == REG) {
            printRegisters();
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
void runJumpCommand(short int command) {
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
        address = targetAddress.integer;
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
            return regis.AX;
        case BXREG:
            return regis.BX;
        case CXREG:
            return regis.CX;
        case DXREG:
            return regis.DX;
        case EXREG:
            return regis.EX;
        case FXREG:
            return regis.FX;
        case GXREG:
            return regis.GX;
        case HXREG:
            return regis.HX;
        case IXREG:
            return regis.IX;
        case JXREG:
            return regis.JX;
        case KXREG:
            return regis.KX;
        case LXREG:
            return regis.LX;
        case ADDRESS:
            address++; // step over the value to retrieve
            Memory ptr = memory[address - 1]; // the pointer to the memory address to retrieve
            return memory[ptr.integer]; // get the memory value at ptr
        case CONSTANT:
            address++;
            return memory[address - 1];
        case BXADDR:
            return memory[regis.BX.integer];
        case BXPLUS:
            address++;
            short int offset = memory[address - 1].integer;
            return memory[regis.BX.integer + offset];
        default: // nonexistent register
            printf("Unknown register: %d at address %d", reg, address);
            system("pause");
            exit(1);
    }
}

/*
 * Puts an integer into a register
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
        case EXREG:
            regis.EX = value;
            break;
        case FXREG:
            regis.FX = value;
            break;
        case GXREG:
            regis.GX = value;
            break;
        case HXREG:
            regis.HX = value;
            break;
        case IXREG:
            regis.IX = value;
            break;
        case JXREG:
            regis.JX = value;
            break;
        case KXREG:
            regis.KX = value;
            break;
        case LXREG:
            regis.LX = value;
            break;
        case ADDRESS:
            address++; // step over the memory address to store at
            Memory ptr = memory[address - 1]; // a pointer to where to store value
            memory[ptr.integer] = value;
            break;
        case BXADDR:
            memory[regis.BX.integer] = value;
            break;
        case BXPLUS:
            address++; // step over the offset
            short int offset = memory[address - 1].integer;
            memory[regis.BX.integer + offset] = value;
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
    printf("\nAX:%d\t", regis.AX.integer);
    printf("BX:%d\t", regis.BX.integer);
    printf("CX:%d\t", regis.CX.integer);
    printf("DX:%d\t", regis.DX.integer);
    printf("EX:%d\t", regis.EX.integer);
    printf("FX:%d\t", regis.FX.integer);
    printf("GX:%f\t", (float) regis.GX.real);
    printf("HX:%f\t", (float) regis.HX.real);
    printf("IX:%f\t", (float) regis.IX.real);
    printf("JX:%f\t", (float) regis.JX.real);
    printf("KX:%f\t", (float) regis.KX.real);
    printf("LX:%f\t", (float) regis.LX.real);
    printf("Address: %d\n", address);
    printf("Flag: %d\n", regis.flag);
    printf("===Labels Table===\n");

    printLabels();

    printf("\n");
    printf("\n");
}

void printRegisters() {
    printf("\nAX:%d\t", regis.AX.integer);
    printf("BX:%d\t", regis.BX.integer);
    printf("CX:%d\t", regis.CX.integer);
    printf("DX:%d\t", regis.DX.integer);
    printf("EX:%d\t", regis.EX.integer);
    printf("FX:%d\t", regis.FX.integer);
    printf("GX:%f\t", (float) regis.GX.real);
    printf("HX:%f\t", (float) regis.HX.real);
    printf("IX:%f\t", (float) regis.IX.real);
    printf("JX:%f\t", (float) regis.JX.real);
    printf("KX:%f\t", (float) regis.KX.real);
    printf("LX:%f\t", (float) regis.LX.real);
    printf("Address: %d\n", address);
    printf("Flag: %d\n", regis.flag);
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