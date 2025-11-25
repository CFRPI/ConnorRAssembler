// name: Connor Reed
// CSC2025 Assembler Project
// date: 11/25/25
// i/o files: part7CR.asm part7FibCR.asm
// description: Reads an assembly file and generates machine code then executes that machine code on a virtual machine
// currently implemented: Mov with registers, constants, and memory, halt, add, put, get, all jumps

#define _CRT_SECURE_NO_WARNINGS  // lets us use deprecated code

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// part7CR.asm sums up 10 numbers with a loop
// part7FibCR.asm calculates the inputted amount of Fibonacci numbers
char ASM_FILE_NAME[ ] = "part7FibCR.asm";

#define MAX 150			// strlen of simulators memory can be changed
#define COL 7			// number of columns for output
#define LINE_SIZE 100	// For c-strings

//OPERAND TYPES, REGISTERS AND OTHER
#define AXREG 0
#define BXREG 1
#define CXREG 2
#define DXREG 3
#define CONSTANT 7
#define ADDRESS 6

//commands
#define HALT 5
#define MOVREG 192
#define MOVMEM 224
#define ADD 160
#define PUT 7 // outputs ax
#define GET 6
#define CMP 96
#define JE 8
#define JNE 9
#define JB 10
#define JBE 11
#define JA 12
#define JAE 13
#define JMP 14
#define ANYJUMP 8 // matches any jump

//boolean
#define TRUE 1
#define FALSE 0

enum operType { reg, mem, constant, arrayBx, arrayBxPlus, none };  //list of all types of operand types

//Registers and flag add the stack pointer
struct Registers
{
	int AX;
	int BX;
	int CX;
	int DX;
	int flag;
}regis;

//GLOBAL VARIABLES
typedef short int Memory;  // sets the type of memory to short int
Memory memory[MAX] = { 0 };   // global variable the memory of the virtual machine
Memory address;     // global variable the current address in the virtual machine

//function prototypes
void runMachineCode( );	// Executes the machine code
void splitCommand( char line[ ], char instruction[ ], char operand1[ ], char operand2[ ] ); // splits a command into its parts
void convertToMachineCode( FILE *fin );	// Converts a single line of ASM to machine code
void assembler( );			// Converts the entire ASM file and stores it in memory
void printMemoryDump( );	// Prints memory with commands represented as integers

// Helper functions prototypes
int convertToNumber( char line[ ], int start );	// converts a sub-string to an int
int whichOpperand( char operand[]);			// Returns the number of the letter register
void changeToLowerCase( char line[ ] );	// Changes each character to lower case
void printMemoryDumpHex( );				// Prints memory in hexadecimal
void putValue( int reg, Memory value ); // puts a value into a register
Memory getValue( int operand ); // gets a value from a register or constant at address
void readInstructionPart(char line[], char part[], int *index); // reads the next part of a line of assembly
void convertJumpToMachineCode(char *part1, char *part2, char *part3); // converts any jump command to machine code
void runJumpCommand(Memory command); // runs any jump command

int main( )
{
	assembler( );
	printf("=================================================\n");
	printf("Memory after program is converted to machine code\n");
	printf("=================================================\n");
	printMemoryDump();
	runMachineCode( );

	printf("================================\n");
	printf("Memory after program is finished\n");
	printf("================================\n");

	printMemoryDump( );  //displays memory with final values

	printf( "\n" );
	system( "pause" );
	return 0;
}

/********************   assembler   ***********************
changes the assembly code to machine code and places the
commands into the memory.
parameters: none
return value: none
-----------------------------------------------------------*/
void assembler( )
{
	address = 0;
	FILE* fin;		// File pointer for reading in the assembly code.
	     //recommend changing so you can type in file name
	fin = fopen(ASM_FILE_NAME, "r" );
	if ( fin == NULL )
	{
		printf( "Error, file didn't open\n\nExiting program...\n\n" );
		system( "pause" );
		exit( 1 );
	}
	for ( int i = 0; i < MAX && !feof( fin ); i++ )
	{
		convertToMachineCode( fin );
	}
}

/********************   convertToMachineCode   ***********************
Converts a single line of ASM to machine code

fin - file pointer to read from and converts it to machine code line by line writing to memory.

Return Value - void
---------------------------------------------------------------------*/
void convertToMachineCode( FILE *fin )
{
	char line[LINE_SIZE];		// full command
	char part1[LINE_SIZE];	// determines which command
	char part2[ LINE_SIZE ] = ""; // the first operand, may be empty
	char part3[ LINE_SIZE ] = ""; // the second operand, may be empty
	Memory machineCode = 0;			// One line of converted asm code from the file

	fgets( line, LINE_SIZE, fin );		// Takes one line from the asm file

	if (line[0] == ';')
	{
		     // If we have a comment, don't look at the rest of the line
		     // or change anything
		return;
	}

	printf("Processing:\n%s\n", line);

	changeToLowerCase( line );
	
	splitCommand( line, part1, part2, part3 );

	     // determines whether it will read in a value to put in the 16 bit slot
	int requires_16_bit_field = 0; // 1 if 16 bits are required at end of command for a constant or memory address

	if ( part1[0] == 'h' ) //halt
	{
		memory[address] = HALT;
		address++;
	}
	else if ( part1[0] == 'm' )  //move into a register or memory location
	{
		     // 3 bits for op, 2 bits for reg1, 3 bits for reg2 or constant
		if (part2[0] == '[')
			{ // movmem
			machineCode = MOVMEM;
			char temp[LINE_SIZE]; // swap part 2 and 3
			strcpy(temp, part2);
			strcpy(part2, part3);
			strcpy(part3, temp);
		} else
			{ // movreg
			machineCode = MOVREG;
		}

		     // operand 1 is not the command but the first operand to follow it
		int operand1 = whichOpperand(part2); // the first operand of the line
		int operand2 = whichOpperand(part3); // the second operand of the line

		machineCode = machineCode | operand1 << 3; // bit shifts 3 to the left and adds it to machine code
		machineCode = machineCode | operand2;

		memory[address] = machineCode;

		requires_16_bit_field = operand2 == CONSTANT || operand2 == ADDRESS; // we need the 16 bit field if we have a constant
		address++;
	} else if (part1[0] == 'a')
	{
		int operand1 = whichOpperand(part2); // reg
		int operand2 = whichOpperand(part3); // reg or const
		machineCode = ADD;

		machineCode |= operand1 << 3; // put in target register
		machineCode |= operand2; // put in reg or const to add
		memory[address] = machineCode;

		requires_16_bit_field = operand2 == CONSTANT || operand2 == ADDRESS; // we need a 16 bit field if we are adding a constant or address
		address++; // increment address
	} else if (part1[0] == 'p') // put
	{
		memory[address] = PUT;
		address ++; // increment address
	} else if (part1[0] == 'g')
		{
		     // get
		memory[address] = GET;
		address ++; // increment address
	} else if (line[0] == '\n')
	{
		memory[address] = 0;
		address++;
	} else if (line[0] == 'c')
		{
		     // cmp
		int operand1 = whichOpperand(part2); // must be a register
		int operand2 = whichOpperand(part3); // any type
		machineCode = CMP;

		machineCode |= operand1 << 3; // move operand to the right spot
		machineCode |= operand2;
		memory[address] = machineCode;

		requires_16_bit_field = operand2 == CONSTANT || operand2 == ADDRESS; // we need a 16 bit field if we are comparing a constant or address
		address++;
	} else if (line[0] == 'j')
	{
		convertJumpToMachineCode(part1, part2, part3);
	} else if (isdigit(part1[0]))
	{
		     // assume it is a number if it is not a command
		memory[address] = convertToNumber(part1, 0);
		address++;
	}

	if (requires_16_bit_field)
	{ // we need to record the 16 bits after this command
			// convert part3 to a number and store it in the next memory slot
		memory[address] = (Memory) convertToNumber(part3, 0);
		address ++;
	}

	     //output memory, for debugging, comment out when you don't need it. could use printMemoryDumpHex
	// printf( "\n" );
	// printMemoryDumpHex( );
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
void convertJumpToMachineCode(char *part1, char *part2, char *part3)
{
	     // we match the jump command and put the correct command into memory
	if (part1[1] == 'm' && part1[2] == 'p')
	{
		     // JMP
		memory[address] = JMP;
	} else if (part1[1] == 'a')
	{
		     // e differentiates JAE & JA
		if (part1[2] == 'e')
		{
			memory[address] = JAE;
		} else
		{
			memory[address] = JA;
		}
	} else if (part1[1] == 'b')
	{
		     // e differentiates JBE & JB
		if (part1[2] == 'e')
		{
			memory[address] = JBE;
		} else
		{
			memory[address] = JB;
		}
	} else if (part1[1] == 'e')
	{
		     // JE
		memory[address] = JE;
	} else if (part1[1] == 'n' && part1[2] == 'e')
	{
		     // JNE
		memory[address] = JNE;
	}

	     // jump over the field so we don't overwrite it in the next line
	address++;

	     // write to the address field
	memory[address] = (Memory) convertToNumber(part2, 0);
	address ++; // next line
}

/********************   splitCommand   ***********************
splits a line of asm into it's parts

Line - the line to split
instruction - a pointer to the string where the instruction part of the line will end up
operand1 - a pointer to the string where the first operand in the line will end up
operand2 - a pointer to the string where the second operand in the line will end up

Return Value - void
-----------------------------------------------------------*/
void splitCommand( char line[ ], char instruction[ ], char operand1[ ], char operand2[ ] )
{
	int lineIndex = 0;           //the character location in the string line

	// read the instruction part of line
	readInstructionPart(line, instruction, &lineIndex);

	if ( line[ lineIndex ] == '\0' || line[ lineIndex ] == '\n')  //no space, command has no other parts
	{
		strcpy( operand1, "\0" );
		strcpy( operand2, "\0" );
	}
	else
	{
		lineIndex++; // step over the space

		readInstructionPart(line, operand1, &lineIndex);

		// there is a second operand to read
		if (line[lineIndex] != '\0' && line[lineIndex] != '\n')
		{
			lineIndex++; // step over the space
			readInstructionPart(line, operand2, &lineIndex);
		}
	}
	//for debugging, comment out when you don't need it
	// printf( "\nCommand = %s %s %s", instruction, operand1, operand2 );
}

/********************   runMachineCode   ***********************
Executes the machine code that is in memory, the virtual machine
Parameters: none
Return Value: none
-----------------------------------------------------------*/
void runMachineCode( )
{
	Memory mask1 = 224;   //111 00 000
	Memory mask2 = 24;    //000 11 000
	Memory mask3 = 7;	  //000 00 111
	Memory part1, part2, part3; //command, operand1, 

	address = 0;
	Memory fullCommand = memory[ address ]; // read the first command from memory
	address++;
	while ( fullCommand != HALT )
	{
		     // parts of the command
		part1 = fullCommand & mask1;
		part2 = fullCommand & mask2;
		part3 = fullCommand & mask3;
		if ( part1 == MOVREG)
		{
			Memory value = getValue(part3);
			int target = part2 >> 3;
			putValue(target, value);
		} else if (part1 == MOVMEM)
		{
			Memory value = getValue(part2 >> 3);
			int target = part3;
			putValue(target, value);
		} else if (part1 == ADD)
		{
				// the sum of the reg and reg/const to be moved into part2
			int target_register = part2 >> 3;
			Memory sum = getValue(target_register) + getValue(part3); // sum of two operands
			putValue(target_register, sum); // write to target register
		} else if (part1 == CMP)
		{
			     // get the value at the register operand1 and the value operand2
			int operand1 = getValue(part2 >> 3); //
			int operand2 = getValue(part3);

			if (operand1 > operand2)
			{
				regis.flag = 1;
			} else if (operand1 < operand2)
			{
				regis.flag = -1;
			} else
			{
				     // operand1 == operand2
				regis.flag = 0;
			}
		} else if (part2 == ANYJUMP)
		{
			runJumpCommand(fullCommand);
			     // we dont go to the next command after a jump
			     // instead we go to wherever the jump command states
		} else if (part3 == PUT) // PUT command is in the last 3 bits
		{
			printf("		REG AX: %d\n", regis.AX);
		} else if (part3 == GET)
		{
			int input = 0;
			printf("Enter an Integer > ");
			scanf("%d", &input);
			regis.AX = input;
		}

 		fullCommand = memory[ address ];  //the next command
		//debugging, comment out when you don't need it
		// printMemoryDumpHex( );
		address ++; // next command
	}
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
void runJumpCommand(Memory command)
{
	Memory targetAddress = memory[address];
	address++;

	int performJump = 0;
	if (command == JMP)
	{
		performJump = 1; // JMP always jumps regardless
	} else if (command == JA && regis.flag == 1)
	{
		performJump = 1; // JA jumps when flag is 1
	} else if (command == JE && regis.flag == 0)
	{
		performJump = 1; // JE jumps when flag is 0
	} else if (command == JB && regis.flag == -1)
	{
		performJump = 1; // JB jumps when flag is -1
	} else if (command == JAE && regis.flag >= 0)
	{
		performJump = 1; // JAE jumps when flag is 0 or 1
	} else if (command == JBE && regis.flag <= 0)
	{
		performJump = 1; // JBE jumps when flag is 0 or -1
	} else if (command == JNE && regis.flag != 0)
	{
		performJump = 1; // JE jumps when flag is not 0
	}

	if (performJump == 1)
	{
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
Memory getValue(int operand)
{
	switch ( operand )
	{
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
void putValue(int reg, Memory value)
{
	     // move value into the proper register
	switch (reg)
	{
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
		default: // if the machine code tells it to put it into a non-existent register
			printf("Error, register %d not recognized address %d", reg, address);
			system("pause");
			exit(1);
	}
}


/***************************  readInstructionPart  *******************************
reads and returns the next "word" of an instruction (command or operands)

line - the line containing the instruction to read from
part - a char[] that will contain the word after the function is called
lineIndex - a pointer to the current location we are reading from on the line as an int
it is an int* because we will increment it until we hit a space or end of line.

Return Value - void
---------------------------------------------------------------------------------*/

void readInstructionPart(char line[], char part[], int *lineIndex)
{
	int partIndex = 0; // the index in the part array where the current character is being written

	     // copy until we hit a space or end of line/string
	while (line[*lineIndex] != ' ' && line[*lineIndex] != '\0' && line[*lineIndex] != '\n')
	{
		part[partIndex] = line[*lineIndex];
		(*lineIndex)++;
		partIndex++;
	}

	part[partIndex] = '\0'; // terminate string
}

/****************************   printMemoryDump   ********************************
prints memory by number
MAX is the amount of elements in the memory array (Vicki used 100)
COL is the number of columns that are to be displayed (Vicki used 7)
parameters: none
return value: none
---------------------------------------------------------------------------------*/
void printMemoryDump( )
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	for ( int row = 0; row < numRows; row++ )
	{
		location = row;
			for ( int column = 0; location < MAX && column < COL; column++ )
		{
			if ( !(numRows - 1 == row&&carryOver - 1 < column) )
			{
				printf( "%5d.%5d", location, memory[location] );
				location += (numRows - (carryOver - 1 < column));
			}
		}
		printf( "\n" );
	}
	printf( "\nAX:%d\t", regis.AX );
	printf( "BX:%d\t", regis.BX );
	printf( "CX:%d\t", regis.CX );
	printf( "DX:%d\n\n", regis.DX );
	printf( "Address: %d\n", address );
	printf( "Flag: %d\n\n", regis.flag );
}


/*********************   printMemoryDumpHex   ********************
Prints memory in
*parameters: none
*return value: none
----------------------------------------------------------------*/
void printMemoryDumpHex( )
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	for ( int row = 0; row < numRows; row++ )
	{
		location = row;
		for ( int column = 0; location < MAX && column < COL; column++ )
		{
			if ( !( numRows - 1 == row && carryOver - 1 < column ) )
			{
				printf( "%5d.%3x", location, memory[ location ] );
				location += ( numRows - ( carryOver - 1 < column ) );
			}
		}
		printf( "\n" );
	}
	printf( "\nAX:%d\t", regis.AX );
	printf( "BX:%d\t", regis.BX );
	printf( "CX:%d\t", regis.CX );
	printf( "DX:%d\n", regis.DX );
	printf( "Address: %d\n", address );
	printf( "Flag: %d\n\n", regis.flag );
}

/*****************************************************************************/
/*********** helper function for converting to machine code ******************/
/*****************************************************************************/

/*********************   whichOpperand   ***************************
/* changes the letter of the register to a number parameters:
/* letter - the first letter of the operand, register, number, [
/* Return Value - the number of the register
/*--------------------------------------------------------------*/
int whichOpperand( char operand[LINE_SIZE] )
{
	     // the first letter of the operand
	char letter = operand[ 0 ];
	if ( letter == 'a' )
	{
		return AXREG;
	} else if ( letter == 'b' )
	{
		return BXREG;
	}
	else if ( letter == 'c' )
	{
		return CXREG;
	}
	else if ( letter == 'd' )
	{
		return DXREG;
	}
	else if ( isdigit( letter ) || letter == '-' )
	{
		return CONSTANT;
	} else if (letter == '[')
	{
		return ADDRESS;
	}
	return -1;  //something went wrong if -1 is returned
}

/*********************ConvertToNumber ********************/
/*  takes in a line and converts digits to a integer          
/*  line - is the string of assembly code to convert           
/*  start - is the location where the line is being converted, 
/*--------------------------------------------------------------*/
int convertToNumber( char line[ ], int start )
{
	int value; // is the integer value of the digits in the code
	char number[ 16 ];  //just the digits
	int negative = 0;  //negative or positive number

	int i = 0;
	while ( line[ start ] == '[' || line[ start ] == ' ' )
	{
		start++;
	}
	if ( line[ start ] == '-' )
	{
		start++;
		negative = 1;
	}
	while ( i < 16 && isdigit( line[ start ] ) )
	{
		number[ i ] = line[ start ];
		i++;
		start++;
	}
	number[ i ] = '\0';
	value = atoi( number );
	if ( negative == 1 )
	{
		value = -value;
	}
	return value;
}  //end convertToNumber

/*********************   changeToLowerCase   ********************
Changes each character to lower case
*parameters:
* line - the string that was entered the line is completely changed to lower case
* return value: none
----------------------------------------------------------------*/
void changeToLowerCase( char line[ ] )
{
	int index = 0;
	while ( index < strlen( line ) )
	{
		line[index] = tolower( line[index] );
		index++;
	}
}

/* Problems:
> Part 6: I had some trouble converting and running JMP but I figured it out, I had various bugs
I found a bug in my whichOperand function that made it unable to handle negative numbers

I also had the weird bug with long comments breaking convertToMachine code but I figured that one out
and set LINE_SIZE to 100 after talking to you
*/