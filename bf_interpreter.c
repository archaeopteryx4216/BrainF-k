#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#define _DEBUG
//#define _STEP

#define bool unsigned char
#define true 1
#define false 0

#define DATA_SIZE 300000  // Size of the char data array
#define LOOP_SIZE 1000   // Maximum number of nested loops
#define CODE_SIZE 10000  // Maximum number of brainfuck characters
                        // in input script.

short int array[DATA_SIZE] = {0};
int data_ptr = 0;
int max_data = 0;

unsigned int loop_stack[LOOP_SIZE] = {0};
int loop_stack_ptr = 0;

char code[CODE_SIZE] = {0};
int code_ptr = 0;

void reset_state() {
    int i;
    data_ptr = 0;
    loop_stack_ptr = 0;
    code_ptr = 0;
    max_data = 0;
    for (i=0; i<DATA_SIZE; i++) {
        array[i] = 0;
    }
    for (i=0; i<LOOP_SIZE; i++) {
        loop_stack[i] = 0;
    }
    for (i=0; i<CODE_SIZE; i++) {
        code_ptr = 0;
    }
}

void print_state() {
    int i;
    fprintf(stderr,"Data:\n|");
    for (i=0; i<=max_data; i++) {
        if (i == data_ptr) {
            fprintf(stderr,"(%d)|",array[i]);
        }
        else {
            fprintf(stderr," %d |",array[i]);
        }
    }
    fprintf(stderr,"\nLoop Stack:\n|");
    for (i=0; i<=loop_stack_ptr; i++) {
        if (i == loop_stack_ptr) {
            fprintf(stderr,"(%d)|",loop_stack[i]);
        }
        else {
            fprintf(stderr," %d |",loop_stack[i]);
        }
    }
    fprintf(stderr,"\nCode:\n");
    for (i=0; (i < CODE_SIZE) && (i < strlen(code)); i++) {
        if (i == code_ptr) {
            fprintf(stderr,"(%d:%c)|",i,code[i]);
        }
        else {
            fprintf(stderr," %d:%c |",i,code[i]);
        }
    }
    fprintf(stderr,"\n\n");
}

bool bf_char(char c) {
    if (c == '+' || c == '-' 
        || c == '<' || c == '>' 
        || c == ',' || c == '.' 
        || c == '[' || c == ']'
       )
        return true;
    else
        return false;
}

void parse_input(char bf[], int s, FILE* fn) {
    int i=0;
    char c;
    for (i=0; i<(s-1);) {
        c = fgetc(fn);
        if (c == EOF) {
            break;
        }
        if (bf_char(c)) {
            bf[i++] = c;
        }
    }
    bf[i] = 0;
#ifdef _DEBUG
    for (i=0; i<strlen(bf); i++) {
        fprintf(stderr,"%c",bf[i]);
    }
    fprintf(stderr,"\n");
#endif
}

int main(int argc, char * argv[]) {
    char io = 0;
    int arg_ptr = 1;
    FILE* infile;
    if (argc == 1) {
        printf("usage: %s one-or-more-input-filenames\n",argv[0]);
        return 0;
    }
    for (arg_ptr = 1; arg_ptr < argc; arg_ptr++) {
        reset_state();
        infile = fopen(argv[arg_ptr],"r");
        if (infile == NULL) {
            fprintf(stderr,"Error, file %s does not exist.\n",argv[arg_ptr]);
            exit(1);
        }
        parse_input(code,CODE_SIZE,infile);
        while ((io = code[code_ptr]) != '\0') {
#ifdef _STEP
            fprintf(stderr,"Press enter to continue:");
            getchar();
#endif
            if (bf_char(io)) {
#ifdef _DEBUG
                print_state();
#endif
                switch (io) {
                case '+':
                    array[data_ptr]++;
                    break;
                case '-':
                    array[data_ptr]--;
                    break;
                case '<':
                    data_ptr = (data_ptr == 0) ? DATA_SIZE - 1 : data_ptr - 1;
                    break;
                case '>':
                    data_ptr = (data_ptr == DATA_SIZE - 1) ? 0 : data_ptr + 1;
                    if (data_ptr > max_data)
                        max_data = data_ptr;
                    break;
                case '.':
#ifdef _DEBUG
                    fputc(array[data_ptr],stderr);
#else
                    fputc(array[data_ptr],stdout);
#endif
                    break;
                case ',':
                    array[data_ptr] = fgetc(stdin);
                    break;
                case '[':
                    if (loop_stack_ptr >= LOOP_SIZE) {
                        fprintf(stderr,"Error, too many nested loops! Exiting now.\n");
                        exit(1);
                    }
                    if (array[data_ptr] == 0) {
                        int count = 0;
                        while (true) {
                            code_ptr++;
                            if (code[code_ptr] == 0) {
                                fprintf(stderr,"Error: missing closing '['.\n");
                                exit(1);
                            }
                            if (code[code_ptr] == '[') {
                                count++;
                            }
                            if (code[code_ptr] == ']') {
                                if (count == 0)
                                    break;
                                else
                                    count--;
                            }
                        }
                    }
                    else {
                        loop_stack[loop_stack_ptr++] = code_ptr;
                    }
                    break;
                case ']':
                    if (loop_stack_ptr < 0) {
                        fprintf(stderr,"Error, ']' without preceding '['! Exiting now.\n");
                        exit(1);
                    }
                    if (array[data_ptr] != 0) {
                        code_ptr = loop_stack[loop_stack_ptr-1];
                    }
                    else {
                        loop_stack[--loop_stack_ptr] = 0;
                    }
                    break;
                default:
                    break;
                }
            }
            code_ptr++;
        }
    }
    return 0;
}
