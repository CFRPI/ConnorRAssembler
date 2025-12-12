//
// Created by Connor Reed on 12/9/25.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "convert.h"

#include "../map/map.h"

/********************   convertToMachineCode   ***********************
Converts a single line of ASM to machine code

fin - file pointer to read from and converts it to machine code line by line writing to memory.
pass - pass 0 fills labelTable but doesn't write to memory, pass 1 converts it to machine code and moves it into memory
Return Value - void
---------------------------------------------------------------------*/
void convertToMachineCode(FILE *fin, int pass) {
    char line[LINE_SIZE]; // full command
    char part1[LINE_SIZE]; // determines which command
    char part2[LINE_SIZE] = ""; // the first operand, may be empty
    char part3[LINE_SIZE] = ""; // the second operand, may be empty
    Memory machineCode = { 0 }; // One line of converted asm code from the file
    Memory operand1 = { 0 };
    Memory operand2 = { 0 };

    fgets(line, LINE_SIZE, fin); // Takes one line from the asm file

    changeToLowerCase(line);

    // one character then a space or end of line means label
    if (line[0] == '_') {
        int lineIndex = 0; // our current position in line
        char labelName[MAX_LABEL_SIZE];
        while (line[lineIndex] != '\0' && line[lineIndex] != '\n' && line[lineIndex] != ' ' && line[lineIndex] != ':') {
            labelName[lineIndex] = line[lineIndex];
            lineIndex++;
        }
        labelName[lineIndex] = '\0';

        if (pass == 0) {
            Memory label;
            label.integer = address;
            insertLabel(labelName, label);
        }

        if (line[lineIndex] ==  ':') {
                 // a : means we should stop parsing and not increment address
                 // the program will continue on the next line
            return;
        }

        // replace the line removing the label;
        char newLine[LINE_SIZE];
        int newLineIndex = 0;
        int oldLineIndex = lineIndex + 1; // start after the label
        while (line[oldLineIndex] != '\0') {
            newLine[newLineIndex] = line[oldLineIndex];
            newLineIndex++;
            oldLineIndex++;
        }

        strcpy(line, newLine);
    }

    if (line[0] == ';') {
        // If we have a comment, don't look at the rest of the line
        // or change anything
        return;
    }

    splitCommand(line, part1, part2, part3);

    // operands of the command, they are declared in the function scope
    // so we can check for a bxplus at the end

    if (part1[0] == 'h') //halt
    {
        // bitshift it into higher 8 bits
        memory[address].integer = HALT << 8;
        address++;
    }
    if (strcmp(part1, "fun") == 0) {
        convertFunctionToMachineCode(line);
    } else if (strcmp(part1, "mov") == 0) //move into a register or memory location
    {
        // 3 bits for op, 2 bits for reg1, 3 bits for reg2 or constant
        if (part2[0] == '[') {
            // movmem
            machineCode.integer = MOVMEM << 8;
            char temp[LINE_SIZE]; // swap part 2 and 3
            strcpy(temp, part2);
            strcpy(part2, part3);
            strcpy(part3, temp);
        } else {
            // movreg
            machineCode.integer = MOVREG << 8;
        }

        // operand 1 is not the command but the first operand to follow it
        operand1.integer = whichOpperand(part2); // the first operand of the line
        operand2.integer = whichOpperand(part3); // the second operand of the line

        machineCode.integer = machineCode.integer | operand1.integer << 4; // bit shifts 3 to the left and adds it to machine code
        machineCode.integer = machineCode.integer | operand2.integer;

        memory[address] = machineCode;
        address++;
    } else if (strcmp(part1, "addi") == 0) {
        operand1.integer = whichOpperand(part2); // reg
        operand2.integer = whichOpperand(part3); // reg or const
        machineCode.integer = ADDI << 8;

        machineCode.integer |= operand1.integer << 4; // put in target register
        machineCode.integer |= operand2.integer; // put in reg or const to add
        memory[address] = machineCode;

        address++; // increment address
    } else if (strcmp(part1, "addr") == 0) {
        operand1.integer = whichOpperand(part2); // reg
        operand2.integer = whichOpperand(part3); // reg or const
        machineCode.integer = ADDR << 8;

        machineCode.integer |= operand1.integer << 4; // put in target register
        machineCode.integer |= operand2.integer; // put in reg or const to add
        memory[address] = machineCode;

        address++; // increment address
    } else if (strcmp(part1, "subr") == 0) {
        operand1.integer = whichOpperand(part2); // reg
        operand2.integer = whichOpperand(part3); // reg or const
        machineCode.integer = SUBR << 8;

        machineCode.integer |= operand1.integer << 4; // put in target register
        machineCode.integer |= operand2.integer; // put in reg or const to add
        memory[address] = machineCode;

        address++; // increment address
    } else if (strcmp(part1, "subi") == 0) {
        operand1.integer = whichOpperand(part2); // reg
        operand2.integer = whichOpperand(part3); // reg or const
        machineCode.integer = SUBI << 8;

        machineCode.integer |= operand1.integer << 4; // put in target register
        machineCode.integer |= operand2.integer; // put in reg or const to add
        memory[address] = machineCode;

        address++; // increment address
    } else if (strcmp(part1, "put") == 0) // put
    {
        memory[address].integer = PUT << 8;
        address++; // increment address
    } else if (strcmp(part1, "putr") == 0) // put
    {
        memory[address].integer = PUTR << 8;
        address++; // increment address
    } else if (strcmp(part1, "get") == 0) {
        // get
        memory[address].integer = GET << 8;
        address++; // increment address
    } else if (strcmp(part1, "ret") == 0) {
        // ret
        memory[address].integer = RET << 8;
        address++; // increment address
    } else if (strcmp(part1, "reg") == 0) {
        // ret
        memory[address].integer = REG << 8;
        address++; // increment address
    } else if (strcmp(part1, "addrv") == 0) {
        operand1.integer = whichOpperand(part2); // reg
        operand2.integer = whichOpperand(part3); // reg or const
        machineCode.integer = ADDRV << 8;

        machineCode.integer |= operand1.integer << 4; // put in target register
        machineCode.integer |= operand2.integer; // put in reg or const to add
        memory[address] = machineCode;

        address++;
    } else if (line[0] == '\n') {
        memory[address].integer = 0;
        address++;
    } else if (strcmp(part1, "cmp") == 0) {
        // cmp
        operand1.integer = whichOpperand(part2); // must be a register
        operand2.integer = whichOpperand(part3); // any type
        machineCode.integer = CMP << 8;

        machineCode.integer |= operand1.integer << 4; // move operand to the right spot
        machineCode.integer |= operand2.integer;
        memory[address] = machineCode;

        address++;
    } else if (line[0] == 'j') {
        convertJumpToMachineCode(part1, part2, part3);
    } else if (isdigit(part1[0]) || part1[0] == '-' || part1[0] == 'f') {
        // assume it is a number if it is not a command
        memory[address] = convertToNumber(line, 0);
        address++;
    }

    if (pass == 0) {
        memory[address].integer = 0; // erase whatever was here if this is the first pass
    }

    if (operand1.integer == ADDRESS || operand1.integer == CONSTANT) {
        // we only write to memory if it is the second pass
        if (pass == 1) {
            // we need to record the 16 bits after this command
            // convert part3 to a number and store it in the next memory slot
            memory[address] = convertToNumber(part2, 0);
        }
        address++;
    }

    if (operand2.integer == ADDRESS || operand2.integer == CONSTANT) {
        if (pass == 1) {
            // we need to record the 16 bits after this command
            // convert part3 to a number and store it in the next memory slot
            memory[address] = convertToNumber(part3, 0);
        }
        address++;
    }

    if (operand1.integer == BXPLUS) {
        if (pass == 1) {
            handleBXPlus(part2);
        }
        address++;
    }

    if (operand2.integer == BXPLUS) {
        if (pass == 1) {
            handleBXPlus(part3);
        }
        address++;
    }

    //output memory, for debugging, comment out when you don't need it. could use printMemoryDumpHex
    // printf( "\n" );
    // printMemoryDumpHex( );
}

void handleBXPlus(char part[LINE_SIZE]) {
    // we need a field for bxplus
    char plusValue[LINE_SIZE]; // the string containing the plus value
    int part3Index = 4; // index in part3 while copying, start right after the +
    int plusValueIndex = 0; // index in plusValue while copying
    // copy into plusValue until we hit a ']', starting after the +
    while (part[part3Index] != ']') {
        plusValue[plusValueIndex] = part[part3Index];
        part3Index++;
        plusValueIndex++;
    }
    plusValue[plusValueIndex] = '\0'; // terminate string
    memory[address] = convertToNumber(plusValue, 0);
}

void convertFunctionToMachineCode(char line[LINE_SIZE]) {
    int lineIndex = 0; // points to the char in line we are reading at
    char addressStr[LINE_SIZE]; // will eventually hold the function address as a string
    // the FUN part of the command will be written to addressStr
    // this is just temporary and will be overwritten later, we don't really need it
    // we just need to increment line index
    readInstructionPart(line, addressStr, &lineIndex);
    memory[address].integer = FUN << 8; // Write fun command to memory
    address++;

    lineIndex++; // step over space

    // now address str actually contains the address as a string
    readInstructionPart(line, addressStr, &lineIndex);
    memory[address] = convertToNumber(addressStr, 0); // write function address to memory
    address++;

    lineIndex++; // step over space

    char numArgsStr[LINE_SIZE]; // a string containing the number of arguments
    readInstructionPart(line, numArgsStr, &lineIndex);
    int numArgs = convertToNumber(numArgsStr, 0).integer;
    memory[address].integer = numArgs; // write to memory
    address++;
    lineIndex++; // step over space

    // for each argument, parse it and write it to memory
    for (int i = 0; i < numArgs; i++) {
        char argumentStr[LINE_SIZE];
        readInstructionPart(line, argumentStr, &lineIndex);
        memory[address] = convertToNumber(argumentStr, 0);
        address++;
        lineIndex++; // step over space
    }

    address++; // blank line
}

/*
 * This function is called as soon as we know we are converting a jump command
 * It handles all the conditions for the different if's needed to convert jumps
 * It writes the command and destination to memory
 *
 * part1 - the first part of the command from splitCommand
 * part2 - the second part of the command from splitCommand
 * part3 - the third part of the command from splitCommand
 *
 * Return Value - void
 */
void convertJumpToMachineCode(char *part1, char *part2, char *part3) {
    // we match the jump command and put the correct command into memory
    if (part1[1] == 'm' && part1[2] == 'p') {
        // JMP
        memory[address].integer = JMP << 8;
    } else if (part1[1] == 'a') {
        // e differentiates JAE & JA
        if (part1[2] == 'e') {
            memory[address].integer = JAE << 8;
        } else {
            memory[address].integer = JA << 8;
        }
    } else if (part1[1] == 'b') {
        // e differentiates JBE & JB
        if (part1[2] == 'e') {
            memory[address].integer = JBE << 8;
        } else {
            memory[address].integer = JB << 8;
        }
    } else if (part1[1] == 'e') {
        // JE
        memory[address].integer = JE << 8;
    } else if (part1[1] == 'n' && part1[2] == 'e') {
        // JNE
        memory[address].integer = JNE << 8;
    }

    // jump over the field so we don't overwrite it in the next line
    address++;

    // write to the address field
    memory[address] = (Memory) convertToNumber(part2, 0);
    address++; // next line
}

/********************   splitCommand   ***********************
splits a line of asm into it's parts

Line - the line to split
instruction - a pointer to the string where the instruction part of the line will end up
operand1 - a pointer to the string where the first operand in the line will end up
operand2 - a pointer to the string where the second operand in the line will end up

Return Value - void
-----------------------------------------------------------*/
void splitCommand(char line[], char instruction[], char operand1[], char operand2[]) {
    int lineIndex = 0; //the character location in the string line

    // read the instruction part of line
    readInstructionPart(line, instruction, &lineIndex);

    if (line[lineIndex] == '\0' || line[lineIndex] == '\n') //no space, command has no other parts
    {
        strcpy(operand1, "\0");
        strcpy(operand2, "\0");
    } else {
        lineIndex++; // step over the space

        readInstructionPart(line, operand1, &lineIndex);

        // there is a second operand to read
        if (line[lineIndex] != '\0' && line[lineIndex] != '\n') {
            lineIndex++; // step over the space
            readInstructionPart(line, operand2, &lineIndex);
        }
    }
    //for debugging, comment out when you don't need it
    // printf( "\nCommand = %s %s %s", instruction, operand1, operand2 );
}

/***************************  readInstructionPart  *******************************
reads and returns the next "word" of an instruction (command or operands)

line - the line containing the instruction to read from
part - a char[] that will contain the word after the function is called
lineIndex - a pointer to the current location we are reading from on the line as an int
it is an int* because we will increment it until we hit a space or end of line.

Return Value - void
---------------------------------------------------------------------------------*/

void readInstructionPart(char line[], char part[], int *lineIndex) {
    int partIndex = 0; // the index in the part array where the current character is being written

    // copy until we hit a space or end of line/string
    while (line[*lineIndex] != ' ' && line[*lineIndex] != '\0' && line[*lineIndex] != '\n') {
        part[partIndex] = line[*lineIndex];
        (*lineIndex)++;
        partIndex++;
    }

    part[partIndex] = '\0'; // terminate string
}

/*********************   whichOpperand   ***************************
/* changes the letter of the register to a number parameters:
/* letter - the first letter of the operand, register, number, [
/* Return Value - the number of the register
/*--------------------------------------------------------------*/
int whichOpperand(char operand[LINE_SIZE]) {
    // the first letter of the operand
    char letter = operand[0];
    if (letter == 'a') {
        return AXREG;
    } else if (letter == 'b') {
        return BXREG;
    } else if (letter == 'c') {
        return CXREG;
    } else if (letter == 'd') {
        return DXREG;
    } else if (letter == 'e') {
        return EXREG;
    } else if (strcmp(operand, "fx") == 0) {
        return FXREG;
    } else if (letter == 'g') {
        return GXREG;
    } else if (letter == 'h') {
        return HXREG;
    } else if (letter == 'i') {
        return IXREG;
    } else if (letter == 'j') {
        return JXREG;
    } else if (letter == 'k') {
        return KXREG;
    } else if (letter == 'l') {
        return LXREG;
    } else if (isdigit(letter) || letter == '-' || letter == 'f') {
        return CONSTANT;
    } else if (letter == '[') {
        if (operand[1] == 'b' && operand[2] == 'x') // bx or bxplus
        {
            if (operand[3] == '+') {
                // BXPLUS
                return BXPLUS;
            }
            // otherwise it is BXADDR
            return BXADDR;
        }

        return ADDRESS;
    }
    return -1; //something went wrong if -1 is returned
}

/*********************ConvertToNumber ********************/
/*  takes in a line and converts digits to a integer
/*  line - is the string of assembly code to convert
/*  start - is the location where the line is being converted,
/*--------------------------------------------------------------*/
Memory convertToNumber(char line[], int start) {
    char number[16]; //just the digits
    int negative = 0; //negative or positive number
    int isFloat = 0;

    int i = 0;
    while (line[start] == '[' || line[start] == ' ') {
        start++;
    }

    if (line[start] == 'f') {
        isFloat = 1;
        start++;
    }

    // this is a label
    if (line[start] == '_') {
        char labelName[MAX_LABEL_SIZE];
        int lineIndex = start; // current position in line for copying
        int labelNameIndex = 0; // current position in labelName
        while (line[lineIndex] !=']') {
            labelName[labelNameIndex] = line[lineIndex];
            lineIndex++;
            labelNameIndex++;
        }
        labelName[labelNameIndex] = '\0'; // terminate string
        Memory value = getLabel(labelName);

        if (value.integer == -1) {
            // for my sanity
            printf("=====Label %c not found!=====", line[start]);
        }

        return value;
    }

    if (line[start] == '-') {
        start++;
        negative = 1;
    }
    while (i < 16 && isdigit(line[start]) || line[start] == '.') {
        number[i] = line[start];
        i++;
        start++;
    }
    number[i] = '\0';

    Memory result;
    if (isFloat == 0) {
        int value; // is the integer value of the digits in the code
        value = atoi(number);

        if (negative == 1) {
            value = -value;
        }
        result.integer = value;
    } else {
        _Float16 value = (_Float16) atof(number);

        if (negative == 1) {
            value = -value;
        }
        result.real = value;
    }

    return result;
} //end convertToNumber

Memory convertFloatToNumber(char line[]) {
    int start = 1;
    int negative = 1;
    char number[16];
    if (line[start] == '-') {
        start++;
        negative = -1;
    }
    int i = start;
    while (i < 16 && isdigit(line[start])) {
        number[i] = line[start];
        i++;
        start++;
    }
    number[i] = '\0';
    Memory res;
    res.real = atof(number);
    return res;
}