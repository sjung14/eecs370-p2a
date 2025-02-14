/**
 * Project 2
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);

int
main(int argc, char **argv)
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    int address_num = -1;
    int text_num = 0;
    int data_num = 0;
    int symbol_num = 0;
    int relocation_num = 0;
    char* label_address[MAXLINELENGTH];
    char* globals_[MAXLINELENGTH];

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        // calculated address for all local labels
        address_num++;
        // if there is a label, if there is, 
        if (label[0] != '\0') {
            label_address[address_num] = malloc(strlen(label) + 1);
            strcpy(label_address[address_num], label);
            // counting symbol
            if (label[0] >= 'A' && label[0] <= 'Z') {
                // check if alr in globals_
                int i = 0;
                if (symbol_num == 0) {
                    globals_[symbol_num] = malloc(strlen(label) + 1);
                    strcpy(globals_[symbol_num], label);
                    symbol_num++;
                } else {
                    for (; i < symbol_num && strcmp(label, globals_[i]); i++) {
                        // nothing here haha
                    }
                    // if not, add
                    if (i == symbol_num) {
                        globals_[symbol_num] = malloc(strlen(label) + 1);
                        strcpy(globals_[symbol_num], label);
                        symbol_num++;
                    }
                }
            }
        } else {
            label_address[address_num] = NULL;
        }
        // printf("label: %s, address: %d\n", label_address[address_num], address_num);

        // filling out header

        // count number of instructions
        if (strcmp(opcode, ".fill")) {
            text_num++;
        }

        // count number of data
        if (!strcmp(opcode, ".fill")) {
            //counting symbol
            if (arg0[0] >= 'A' && arg0[0] <= 'Z') {
                // checking if global label
                int i = 0;
                if (symbol_num == 0) {
                    globals_[symbol_num] = malloc(strlen(arg0) + 1);
                    strcpy(globals_[symbol_num], arg0);
                    symbol_num++;
                } else {
                    for (; i < symbol_num && strcmp(arg0, globals_[i]); i++) {
                        // nothing here haha
                    }
                    // if not, add
                    if (i == symbol_num) {
                        globals_[symbol_num] = malloc(strlen(arg0) + 1);
                        strcpy(globals_[symbol_num], arg0);
                        symbol_num++;
                    }
                }
            }
            data_num++;
        }
        // counting symbol
        if (arg2[0] >= 'A' && arg2[0] <= 'Z') {
            // checking if global label
            int i = 0;
            if (symbol_num == 0) {
                globals_[symbol_num] = malloc(strlen(arg2) + 1);
                strcpy(globals_[symbol_num], arg2);
                symbol_num++;
            } else {
                for (; i < symbol_num && strcmp(arg2, globals_[i]); i++) {
                    // nothing here haha
                }
                // if not, add
                if (i == symbol_num) {
                    globals_[symbol_num] = malloc(strlen(arg2) + 1);
                    strcpy(globals_[symbol_num], arg2);
                    symbol_num++;
                }
            }
        }
        if ((!isNumber(arg2) && (!strcmp(opcode, "lw") || !strcmp(opcode, "sw"))) || (!strcmp(opcode, ".fill") && !isNumber(arg0))) {
            relocation_num++;
        }

    } 
    // print file header!!
    fprintf(outFilePtr, "%d %d %d %d\n", text_num, data_num, symbol_num, relocation_num);

    // Duplicate definition of labels
    for (int i = 0; i <= address_num; i++) {
        for (int j = i + 1; j <= address_num; j++) {
            if (label_address[i] != NULL && label_address[j] != NULL 
                && !strcmp(label_address[i], label_address[j])) {
                    printf("Duplicate definition of labels\n");
                exit(1);
            }
        }
    }
    
    // /* here is an example for how to use readAndParse to read a line from
    //     inFilePtr */
    // if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
    //     /* reached end of file */
    // }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);

    // translate to machine code!!
    // int machine_code[address_num];
    for (int i = 0; i <= address_num; i++) {
        readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2);

        //unused alr in
        int one_line = 0b0;
        one_line = one_line << 6;
        if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")) {
            // Non-integer register arguments
            if (!isNumber(arg0) || !isNumber(arg1) || !isNumber(arg2)) {
                printf("Non-integer register arguments\n");
                exit(1);
            }
            // Registers outside the range [0, 7]
            if (atoi(arg0) < 0 || atoi(arg0) > 7 ||
                atoi(arg1) < 0 || atoi(arg1) > 7 ||
                atoi(arg2) < 0 || atoi(arg2) > 7) {
                    printf("Registers outside the range [0, 7]\n");
                    exit(1);
            }
            // opcode
            if (!strcmp(opcode, "add")) {
                one_line = one_line | 0b000;
            } else {
                one_line = one_line | 0b001;
            }
            // regA
            one_line = one_line << 3;
            one_line = one_line | (7 & atoi(arg0));
            //regB
            one_line = one_line << 3;
            one_line = one_line | (7 & atoi(arg1));
            // unused
            one_line = one_line << 13;
            // destR
            one_line = one_line << 3;
            if (isNumber(arg2)) {
                one_line = one_line | (7 & atoi(arg2));
            } else {
                int j = 0;
                while (j <= address_num && (label_address[j] == NULL || strcmp(arg2, label_address[j]))) {
                    j++;
                }
                if (label_address[j] == NULL) {
                    // use of undefined labels
                    printf("use of undefined labels\n");
                    printf("1\n");
                    exit(1);
                } else if (!strcmp(label_address[j], arg2)) {
                    one_line = one_line | (7 & (j - i));
                } else {
                    // use of undefined labels
                    printf("use of undefined labels\n");
                    printf("2\n");
                    exit(1);
                }
            }
        } else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
            // Non-integer register arguments
            if (!isNumber(arg0) || !isNumber(arg1)) {
                printf("Non-integer register arguments\n");
                exit(1);
            }
            // Registers outside the range [0, 7]
            if (atoi(arg0) < 0 || atoi(arg0) > 7 ||
                atoi(arg1) < 0 || atoi(arg1) > 7) {
                    printf("Registers outside the range [0, 7]\n");
                    exit(1);
            }
            // offsetFields that don’t fit in 16 bits
            if (atoi(arg2) > 32767 || atoi(arg2) < -32768) {
                printf("offsetFields that don’t fit in 16 bits\n");
                exit(1);
            }
            // opcode
            if (!strcmp(opcode, "lw")) {
                one_line = one_line | 0b010;
            } else if (!strcmp(opcode, "sw")) {
                one_line = one_line | 0b011;
            } else {
                one_line = one_line | 0b100;
            }
            // regA
            one_line = one_line << 3;
            one_line = one_line | (7 & atoi(arg0));
            //regB
            one_line = one_line << 3;
            one_line = one_line | (7 & atoi(arg1));
            // offset
            one_line = one_line << 16; 
            if (isNumber(arg2)) {
                one_line = one_line | (65535 & atoi(arg2));
                // printf("looks like %s is int!\n", arg2);
            } else if (!strcmp(opcode, "beq")) {
                int j = 0;
                while (j <= address_num && (label_address[j] == NULL || strcmp(arg2, label_address[j]))) {
                    j++;
                }
                if (label_address[j] == NULL) {
                    // use of undefined labels
                    printf("use of undefined labels\n");
                    printf("3\n");
                    exit(1);
                } else {
                    one_line = one_line | (65535 & (j - i - 1));
                    // printf("%s wrong way\n", opcode);
                }
            } else { // opcode != "beq"
                int j = 0;
                while (j <= address_num && (label_address[j] == NULL || strcmp(arg2, label_address[j]))) {
                    j++;
                }
                if (label_address[j] == NULL && !(arg2[0] >= 'A' && arg2[0] <= 'Z')) {
                    // use of undefined labels
                    printf("use of undefined labels\n");
                    printf("4\n");
                    exit(1);
                } else if (label_address[j] == NULL && (arg2[0] >= 'A' && arg2[0] <= 'Z')) {
                    one_line = one_line | 0;
                } else if (!strcmp(label_address[j], arg2)) {
                    one_line = one_line | (65535 & j);
                } else {
                    // use of undefined labels
                    printf("use of undefined labels: %s\n", arg2);
                    printf("5\n");
                    exit(1);
                }
                // printf("arg2: %s, label: %s, address: %d\n", arg2, label_address[j], j);
            }
        } else if (!strcmp(opcode, "jalr")) {
            // Non-integer register arguments
            if (!isNumber(arg0) || !isNumber(arg1)) {
                printf("Non-integer register arguments\n");
                exit(1);
            }
            // Registers outside the range [0, 7]
            if (atoi(arg0) < 0 || atoi(arg0) > 7 ||
                atoi(arg1) < 0 || atoi(arg1) > 7) {
                    printf("Registers outside the range [0, 7]\n");
                    exit(1);
            }
            one_line = one_line | 0b101;
            // regA
            one_line = one_line << 3;
            one_line = one_line | (7 & atoi(arg0));
            //regB
            one_line = one_line << 3;
            one_line = one_line | (7 & atoi(arg1));
            // unused
            one_line = one_line << 16; 
        } else if (!strcmp(opcode, "halt")) {
            one_line = one_line | 0b110;
            one_line = one_line << 22; 
        } else if (!strcmp(opcode, "noop")) {
            one_line = one_line | 0b111;
            one_line = one_line << 22; 
        }  else if (!strcmp(opcode, ".fill")) {
            if (isNumber(arg0)) {
                one_line = atoi(arg0);
                // printf(".fill int %d", atoi(arg0));
            } else {
                int j = 0;
                while (j <= address_num && (label_address[j] == NULL || strcmp(arg0, label_address[j]))) {
                    j++;
                }
                if (label_address[j] == NULL && !(arg0[0] >= 'A' && arg0[0] <= 'Z')) {
                    // use of undefined labels
                    printf("use of undefined labels\n");
                    printf("6\n");
                    exit(1);
                } else if (label_address[j] == NULL && (arg0[0] >= 'A' && arg0[0] <= 'Z')) {
                    one_line = 0;
                } else {
                    one_line = j;
                }
            }
        } else {
            // unrecognized opcodes
            printf("unrecognized opcodes\n");
            exit(1);
        }
        // machine_code[i] = one_line;
        printHexToFile(outFilePtr, one_line);
    }
    // /* after doing a readAndParse, you may want to do the following to test the
    //     opcode */
    // if (!strcmp(opcode, "add")) {
    //     /* do whatever you need to do for opcode "add" */
    //     int a0 = atoi(arg0);
    //     int a1 = atoi(arg1);
    //     int a2 = atoi(arg2);
    //     add 
        
    // }
    // /* here is an example of using isNumber. "5" is a number, so this will
    //    return true */
    // if(isNumber("5")) {
    //     printf("It's a number\n");
    // }
    // /* here is an example of using printHexToFile. This will print a
    //    machine code word / number in the proper hex format to the output file */
    // printHexToFile(outFilePtr, 123);



    // now make symbol table
    rewind(inFilePtr);
    // finding locally defined labels
    char * locally_defined_globals_[MAXLINELENGTH]; // sorry wrong name -- started using for undefined too....
    int locally_defined_num = 0;
    for (int i = 0; i <= address_num; i++) {
        readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2);

        // locally defined labels
        if (label[0] >= 'A' && label[0] <= 'Z') {
            locally_defined_globals_[locally_defined_num] = malloc(strlen(label) + 1);
            strcpy(locally_defined_globals_[locally_defined_num], label);
            locally_defined_num++;
            if (!strcmp(opcode, ".fill")) {
                fprintf(outFilePtr, "%s D %d\n", label, i - text_num);
            } else {
                fprintf(outFilePtr, "%s T %d\n", label, i);
            }
        } 
    }
    rewind(inFilePtr);

    // finding undefined global labels
    for (int i = 0; i <= address_num; i++) {
        readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2);

        if (!strcmp(opcode, ".fill") && (arg0[0] >= 'A' && arg0[0] <= 'Z')) {
            int j = 0;
            while (j < locally_defined_num && strcmp(arg0, locally_defined_globals_[j])) {
                    j++;
            }
            if (j == locally_defined_num) {
                // undefined global
                locally_defined_globals_[locally_defined_num] = malloc(strlen(arg0) + 1);
                strcpy(locally_defined_globals_[locally_defined_num], arg0);
                locally_defined_num++;
                fprintf(outFilePtr, "%s U %d\n", arg0, 0);
            } 
        } else if (strcmp(opcode, "beq") && (arg2[0] >= 'A' && arg2[0] <= 'Z')) {
            int j = 0;
            while (j < locally_defined_num && strcmp(arg2, locally_defined_globals_[j])) {
                    j++;
            }
            if (j == locally_defined_num) {
                // undefined global
                locally_defined_globals_[locally_defined_num] = malloc(strlen(arg2) + 1);
                strcpy(locally_defined_globals_[locally_defined_num], arg2);
                locally_defined_num++;
                fprintf(outFilePtr, "%s U %d\n", arg2, 0);
            }
        }
    }

    // now make relocation table
    rewind(inFilePtr);
    for (int i = 0; i <= address_num; i++) {
        readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2);
        if ((!strcmp(opcode, "lw") || !strcmp(opcode, "sw")) && !isNumber(arg2)) {
            fprintf(outFilePtr, "%d %s %s\n", i, opcode, arg2);
        } else if (!strcmp(opcode, ".fill") && !isNumber(arg0)) {
            fprintf(outFilePtr, "%d %s %s\n", i - text_num, opcode, arg0);
        }
    }

    return(0);
}

// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line) {
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
        int line_char_is_whitespace = 0;
        for(int whitespace_idx = 0; whitespace_idx < 4; ++ whitespace_idx) {
            if(line[line_idx] == whitespace[whitespace_idx]) {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if(!line_char_is_whitespace) {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH-1) {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if(lineIsBlank(line)) {
            if(!blank_line_encountered) {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        } else {
            if(blank_line_encountered) {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}


/*
* NOTE: The code defined below is not to be modifed as it is implimented correctly.
*/

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
    }

    // Ignore blank lines at the end of the file.
    if(lineIsBlank(line)) {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);

    return(1);
}

static inline int
isNumber(char *string)
{
    int num;
    char c;
    return((sscanf(string, "%d%c",&num, &c)) == 1);
}


// Prints a machine code word in the proper hex format to the file
static inline void 
printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}