// name: Connor Reed
// CSC2025 Assembler Project
// date: 9/12/25
// i/o files: part2CR.asm
// description: Reads an assembly file and generates machine code then executes that machine code on a virtual machine
// currently implemented: Mov with registers and constants, halt

#define _CRT_SECURE_NO_WARNINGS  // lets us use deprecated code

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char ASM_FILE_NAME[ ] = "part2CR.asm";

#define MAX 150			// strlen of simulators memory can be changed
#define COL 7			// number of columns for output
#define LINE_SIZE 20	// For c-strings

//OPERAND TYPES, REGISTERS AND OTHER
#define AXREG 0
#define BXREG 1
#define CXREG 2
#define DXREG 3
#define CONSTANT 7

//commands
#define HALT 5
#define MOVREG 192

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
Memory address;     // global variable the current addres s in the virtual machine

//function prototypes
void runMachineCode( );	// Executes the machine code	****NEEDS WORK***
void splitCommand( char line[ ], char instruction[ ], char operand1[ ], char operand2[ ] );
void convertToMachineCode( FILE *fin );	// Converts a single line of ASM to machine code	***NEEDS WORK***
void assembler( );			// Converts the entire ASM file and stores it in memory
void printMemoryDump( );	// Prints memory with commands represented as integers

// Helper functions prototypes
int convertToNumber( char line[ ], int start );	// converts a sub-string to an int
int whichOpperand( char operand[]);			// Returns the number of the letter register
void changeToLowerCase( char line[ ] );	// Changes each character to lower case
void printMemoryDumpHex( );				// Prints memory in hexadecimal
void putValue( int operand, int value );
Memory getValue( Memory operand );
void readInstructionPart(char line[], char part[], int *index);
Memory readRegister(int reg);
void writeRegister(int reg, Memory value);
int main( )
{
	assembler( );
	runMachineCode( );
	printMemoryDumpHex( );  //displays memory with final values
	
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

Needs work, comment must be corrected
---------------------------------------------------------------------*/
void convertToMachineCode( FILE *fin )
{
	char line[LINE_SIZE];		// full command
	char part1[LINE_SIZE];	// the asm command
	char part2[ LINE_SIZE ] = "";// the two operands, could be empty
	char part3[ LINE_SIZE ] = "";	
	Memory machineCode = 0;			// One line of converted asm code from the file

	fgets( line, LINE_SIZE, fin );		// Takes one line from the asm file
	changeToLowerCase( line );
	
	splitCommand( line, part1, part2, part3 );

		// determines whether it will read in a value to put in the 16 bit slot
	int requires_16_bit_field = 0; // 1 if 16 bits are required at end of command for a constant or memory address
	if ( part1[0] == 'h' )  //halt
	{
		memory[address] = HALT;
		address++;
	}
	else if ( part1[0] == 'm' )  //move into a register
	{
			// 3 bits for op, 2 bits for reg1, 3 bits for reg2 or constant

			// operand 1 is not the command but the first operand to follow it
		int operand1 = whichOpperand(part2); // the first operant of the line
		int operand2 = whichOpperand(part3); // the second operand of the line
		machineCode=192;
		machineCode = machineCode | operand1 << 3; // bit shifts 3 to the left and adds it to machine code
		machineCode = machineCode | operand2;
		memory[address] = machineCode;

		requires_16_bit_field = operand2 == CONSTANT; // we need the 16 bit field if we have a constant
		address++;
	}

	if (requires_16_bit_field) { // we need to record the 16 bits after this command
			// convert part3 to a number and store it in the next memory slot
		memory[address] = (Memory) convertToNumber(part3, 0);
		address += 1;
	}

	     //output memory, for debugging, comment out when you don't need it. could use printMemoryDumpHex
	// printf( "\n" );
	// printMemoryDumpHex( );
}


/********************   splitCommand   ***********************
splits a line of asm into it's parts

Needs work, comment must be corrected
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

Needs to be written
-----------------------------------------------------------*/
void runMachineCode( )
{
	Memory mask1 = 224;   //111 00 000
	Memory mask2 = 24;    //000 11 000
	Memory mask3 = 7;	  //000 00 111
	Memory part1, part2, part3; //command, operand1, 
	int value1, value2;   //the actual values in the registers or constants
	
	address = 0;
	Memory fullCommand = memory[ address ];
	address++;
	while ( fullCommand != HALT )
	{
		// parts of the command
		part1 = fullCommand & mask1;
		part2 = fullCommand & mask2;
		part3 = fullCommand & mask3;
		if ( part1 == MOVREG )
		{
			Memory value = 0; // the value to put into the target register
			if (part3 == CONSTANT) {
				value = memory[address]; // fetch memory
				address ++; // step over memory
			} else {
				value = readRegister(part3);
			}
			int target_register = part2 >> 3;
			writeRegister(target_register, value);
		}
		fullCommand = memory[ address ];  //the next command
		address ++; // next command
		//debugging, comment out when you don't need it
		// printMemoryDumpHex( );
	}
}

/*********************************************************************************
/****************************   HELPER FUNCTIONS   *******************************
/*********************************************************************************/

/***************************  readInstructionPart  *******************************
reads and returns the next "word" of an instruction (command or operands)

line is the line containing the instruction to read from
part is a char[] that will contain the word after the function is called
lineIndex is a pointer to the current location we are reading from on the line as an int
it is an int* because we will increment it until we hit a space or end of line.
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

/*
 * Reads the register with bit value of reg ex: reg=000 for AX
 * You pass in the register and it returns the value stored there as Memory
 *
 * This function will fatally error if you pass in a nonexistent register
 */
Memory readRegister(int reg) {
	switch ( reg ) { // read the value of either the register or constant specified
		case AXREG:
			return (Memory) regis.AX;
		case BXREG:
			return (Memory) regis.BX;
		case CXREG:
			return (Memory) regis.CX;
		case DXREG:
			return (Memory) regis.DX;
		default: // nonexistent register
			printf("Unknown register: %d", reg);
			system("pause");
			exit(1);
	}
}

/*
 * Writes a value of type Memory into the register specified by reg
 * reg is the binary value of the register you want to target, 000 for AX, 001 for BX etc.
 * This function will give a fatal error if you pass in a bad register
 */
void writeRegister(int reg, Memory value) {
	switch (reg) { // move value into the proper register
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
		default: // if the machine code tells it to put it into a non-existent register
			printf("Error, register %d not recognized", reg);
			system("pause");
			exit(1);
	}
}

/*********************   whichOpperand   ***************************
/* changes the letter of the register to a number parameters:
/* letter - the first letter of the operand, register, number, [
/* return value - the number of the register
/*--------------------------------------------------------------*/
int whichOpperand( char operand[LINE_SIZE] )
{
	char letter = operand[ 0 ];
	if ( letter == 'a' )
	{
		return AXREG;
	}
	else if ( letter == 'b' )
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
	else if ( isdigit( letter ) )
	{
		return CONSTANT;
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
> Part 1: None
> Part 1: I had to do some debugging when abstracting the reading and
writing of registers into their own functions, I have it figured out now.
*/