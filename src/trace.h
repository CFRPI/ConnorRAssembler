//
// Created by Connor Reed on 10/20/25.
//

#ifndef AssemblyRecorder
#define AssemblyRecorder

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// the total number of steps in the program this will record,
// change as you need
#define MAX_STEPS 200
// The memory size in your program, must always equal your MAX value defined
// at the top of your program
#define MEMORY_SIZE 150

// points to the template file you have copied
char TEMPLATE_FILENAME[] = "src/template.html";
// the path of the output file to be made
char PLAYBACK_FILENAME[] = "playback.html";

int hasWritten = 0;

// ================================================
// You should not need to touch anything below here
// ================================================

typedef struct Playback_ {
    int current_step;
    short int memory_history[MAX_STEPS * MEMORY_SIZE];
    short int regis_history[MAX_STEPS * 5]; // 4 registers + flag
    short int address_history[MAX_STEPS];
} Trace;

typedef struct RegistersClone
{
    int AX;
    int BX;
    int CX;
    int DX;
    int flag;
} regis_clone;

// records the content of an array to the html file
void saveArray(FILE *output, short int arr[], int length) {
    for (int i = 0; i < length; i++) {
        char num_str[15];
        sprintf(num_str, "%d", arr[i]);
        char comma[] = ",";
        strcat(num_str, comma);
        fprintf(output, "%s", num_str);
    }
}

// records the data portion of the html file
void savePlaybackData(FILE *output, Trace *trace) {
    fprintf(output, "let memoryHistory = [");
    saveArray(output, trace->memory_history, MAX_STEPS * MEMORY_SIZE);
    fprintf(output, "];\n");

    fprintf(output, "\tlet registerHistory = [");
    saveArray(output, trace->regis_history, MAX_STEPS * 5);
    fprintf(output, "];\n");

    fprintf(output, "\tlet addressHistory = [");
    saveArray(output, trace->address_history, MAX_STEPS);
    fprintf(output, "];\n");

    fprintf(output, "\tlet totalSteps = ");
    char steps[6];
    sprintf(steps, "%d", trace->current_step);
    fprintf(output, "%s", steps);
    fprintf(output, ";\n");

    fprintf(output, "\tlet memorySize = ");
    char memorySize[5];
    sprintf(steps, "%d", MEMORY_SIZE);
    fprintf(output, "%s", steps);
    fprintf(output, ";\n");

    fprintf(output, "\tlet maxSteps = ");
    char maxSteps[5];
    sprintf(maxSteps, "%d", MAX_STEPS);
    fprintf(output, "%s", maxSteps);
    fprintf(output, ";");
}

// creates an html file from trace
void saveTrace(Trace *trace) {
    FILE *input = fopen(TEMPLATE_FILENAME, "rb");
    if (!input) {
        printf("Failed to open template file\n");
        return;
    }
    fseek(input, 0, SEEK_END);
    long length = ftell(input);
    fseek(input, 0, SEEK_SET);
    char *template = malloc(length);
    fread(template, 1, length, input);
    fclose(input);

    FILE *output = fopen(PLAYBACK_FILENAME, "w");
    for (int i = 0; i < length; i++) {
        if (i < length - 4 && template[i] == '{' && template[i + 1] == '{' && template[i + 2] == '}' && template[i + 3] == '}') {
            savePlaybackData(output, trace);
            i += 4;
        }

        char c[3];
        if (template[i] == '%') {
            c[0] = '%';
            c[1] = '\0';
        } else {
            c[0] = template[i];
            c[1] = '\0';
        }
        fprintf(output, "%s", c);
    }

    fclose(output);
    free(template);
}

void recordStep(Trace *playback, void *registers, short int memory[MEMORY_SIZE], short int address) {
    if (playback->current_step >= MAX_STEPS) {
        if (hasWritten == 0) {
            // automatically save in case we exceed maximum steps
            // this could help debug infinite loops
            saveTrace(playback);
            hasWritten = 1;
        }
        return; // we are limited by the array size allocated in the beginning
    }

    int starting_memory_history_position = MEMORY_SIZE * playback->current_step;
    for (int i = 0; i < MEMORY_SIZE; i++) {
        playback->memory_history[i + starting_memory_history_position] = memory[i];
    }

    int starting_regis_history = 5 * playback->current_step;
    regis_clone *typed_registers = (regis_clone*) registers;
    playback->regis_history[starting_regis_history + 0] = typed_registers->AX;
    playback->regis_history[starting_regis_history + 1] = typed_registers->BX;
    playback->regis_history[starting_regis_history + 2] = typed_registers->CX;
    playback->regis_history[starting_regis_history + 3] = typed_registers->DX;
    playback->regis_history[starting_regis_history + 4] = typed_registers->flag;

    // record the address
    playback->address_history[playback->current_step] = address - 1;

    playback->current_step += 1;
}

#endif