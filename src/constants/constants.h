//
// Created by Connor Reed on 12/9/25.
//

#ifndef ASSEMBLER_CONSTANTS_H
#define ASSEMBLER_CONSTANTS_H

typedef short int Memory; // sets the type of memory to short int

#define MAX_LABELS 200
#define MAX_LABEL_SIZE 20

#define MAX 500			// strlen of simulators memory can be changed
#define COL 7			// number of columns for output
#define LINE_SIZE 100	// For c-strings

//OPERAND TYPES, REGISTERS AND OTHER
#define AXREG 0
#define BXREG 1
#define CXREG 2
#define DXREG 3
#define CONSTANT 7
#define ADDRESS 6
#define BXADDR 4
#define BXPLUS 5

//commands
#define HALT 4
#define MOVREG 34
#define MOVMEM 35
#define ADDI 36
#define SUBI 37
#define MULI 38
#define DIVI 39
#define ADDR 40
#define SUBR 41
#define MULR 42
#define DIVR 43
#define MODI 44
#define MODR 45
#define OR 46
#define AND 47
#define NOT 48
#define XOR 49
#define SHIFTL 50
#define SHIFTR 51
#define REFV 52
#define PUT 6 // outputs ax
#define PUTC 7
#define PUTR 8
#define MEM 0
#define MEMH 1
#define GET 5
#define CMP 33
#define JE 64
#define JNE 65
#define JB 66
#define JBE 67
#define JA 68
#define JAE 69
#define JMP 70
#define FUN 3
#define RET 2
#define ANYJUMP 64 // matches any jump

//boolean
#define TRUE 1
#define FALSE 0

enum operType { reg, mem, constant, arrayBx, arrayBxPlus, none }; //list of all types of operand types

//Registers and flag add the stack pointer
struct Registers {
    int AX;
    int BX;
    int CX;
    int DX;
    int flag;
} regis;

//GLOBAL VARIABLES
extern Memory memory[MAX]; // global variable the memory of the virtual machine
extern Memory address; // global variable the current address in the virtual machine
extern Memory stackPointer;

#endif //ASSEMBLER_CONSTANTS_H