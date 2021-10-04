/**
 * Project 2
 * Assembler code fragment for LC-2K 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

struct labelInfo
{
    char label[7];
    int addr;
};

struct symbolInfo
{
    char symbol[7];
    char section;
    int addr;
};

struct relocationTable
{
    char symbol[7];
    int addr;
    char operation[7];
};

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3)
    {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
               argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL)
    {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL)
    {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    int trace = 0;
    int num_label = 0;

    int text_size = 0, data_size = 0, global_symbol_size = 0, relocation_size = 0;

    struct labelInfo labels[MAXLINELENGTH];
    struct symbolInfo symbols[MAXLINELENGTH];
    struct relocationTable relocations[MAXLINELENGTH];

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
        //* Update labels present
        if (strcmp(label, "") != 0)
        {

            //Check whether there are duplciate labels(global and local)
            for (int i = 0; i < num_label; i++)
            {
                if (strcmp(labels[i].label, label) == 0)
                    exit(1);
            }
            for (int i = 0; i < global_symbol_size; i++)
            {
                if (strcmp(symbols[i].symbol, label) == 0)
                    exit(1);
            }

            if (label[0] >= 'A' && label[0] <= 'Z')
            {
                strcpy(symbols[global_symbol_size].symbol, label);
                symbols[global_symbol_size].addr = trace;
                global_symbol_size++;
            }

            strcpy(labels[num_label].label, label);
            labels[num_label].addr = trace;

            num_label++;
        }

        //* Update the size of data and text
        // When the line is a directive
        if (strcmp(opcode, ".fill") == 0)
            data_size++;
        // Or if the line is a instruction
        else
            text_size++;

        //* Update the size of relocation table
        if (strcmp(opcode, ".fill") == 0 &&
            !isNumber(arg0))
        {
            strcpy(relocations[relocation_size].symbol, arg0);
            relocations[relocation_size].addr = trace;
            strcpy(relocations[relocation_size].operation, ".fill");
            relocation_size++;
        }
        if (strcmp(opcode, "lw") == 0 &&
            !isNumber(arg2))
        {
            strcpy(relocations[relocation_size].symbol, arg2);
            relocations[relocation_size].addr = trace;
            strcpy(relocations[relocation_size].operation, "lw");
            relocation_size++;
        }
        if (strcmp(opcode, "sw") == 0 &&
            !isNumber(arg2))
        {
            strcpy(relocations[relocation_size].symbol, arg2);
            relocations[relocation_size].addr = trace;
            strcpy(relocations[relocation_size].operation, "sw");
            relocation_size++;
        }

        trace++;
    }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);

    trace = 0;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
        //* Update the size of symbol table part 2

        if ((strcmp(opcode, ".fill") == 0 &&
             !isNumber(arg0)))
        {
            if (arg0[0] >= 'A' && arg0[0] <= 'Z')
            {
                int defined = 0;
                for (int i = 0; i < global_symbol_size; i++)
                {
                    if (strcmp(symbols[i].symbol, arg0) == 0)
                    {
                        defined = 1;
                        break;
                    }
                }

                if (!defined)
                {

                    strcpy(symbols[global_symbol_size].symbol, arg0);
                    symbols[global_symbol_size].addr = -1;
                    global_symbol_size++;
                }
            }
        }
        else if (strcmp(opcode, "lw") == 0 &&
                 !isNumber(arg2))
        {
            if (arg2[0] >= 'A' && arg2[0] <= 'Z')
            {
                int defined = 0;
                for (int i = 0; i < global_symbol_size; i++)
                {
                    if (strcmp(symbols[i].symbol, arg2) == 0)
                    {
                        defined = 1;
                        break;
                    }
                }

                if (!defined)
                {

                    strcpy(symbols[global_symbol_size].symbol, arg2);
                    symbols[global_symbol_size].addr = -1;
                    global_symbol_size++;
                }
            }
        }

        else if (strcmp(opcode, "sw") == 0 &&
                 !isNumber(arg2))
        {
            if (arg2[0] >= 'A' && arg2[0] <= 'Z')
            {
                int defined = 0;
                for (int i = 0; i < global_symbol_size; i++)
                {
                    if (strcmp(symbols[i].symbol, arg2) == 0)
                    {
                        defined = 1;
                        break;
                    }
                }

                if (!defined)
                {

                    strcpy(symbols[global_symbol_size].symbol, arg2);
                    symbols[global_symbol_size].addr = -1;
                    global_symbol_size++;
                }
            }
        }
        else if (strcmp(opcode, "beq") == 0 &&
                 !isNumber(arg2))
        {
            if (arg2[0] >= 'A' && arg2[0] <= 'Z')
            {
                int defined = 0;
                for (int i = 0; i < global_symbol_size; i++)
                {
                    if (strcmp(symbols[i].symbol, arg2) == 0)
                    {
                        defined = 1;
                        break;
                    }
                }

                if (!defined)
                {

                    strcpy(symbols[global_symbol_size].symbol, arg2);
                    symbols[global_symbol_size].addr = -1;
                    global_symbol_size++;
                }
            }
        }
    }
    fprintf(outFilePtr, "%d %d %d %d\n", text_size, data_size, global_symbol_size, relocation_size);

    rewind(inFilePtr);

    int instruction;
    int PC = 0;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
        /* after doing a readAndParse, you may want to do the following to test the
        opcode */
        if (!strcmp(opcode, "add"))
        {
            /* do whatever you need to do for opcode "add" */
            instruction = 0b000 << 3;
            instruction += atoi(arg0);
            instruction <<= 3;
            instruction += atoi(arg1);
            instruction <<= 16;

            instruction += atoi(arg2);
        }
        else if (!strcmp(opcode, "nor"))
        {
            instruction = 0b001 << 3;
            instruction += atoi(arg0);
            instruction <<= 3;
            instruction += atoi(arg1);
            instruction <<= 16;

            instruction += atoi(arg2);
        }
        else if (!strcmp(opcode, "lw"))
        {
            instruction = 0b010 << 3;
            instruction += atoi(arg0);
            instruction <<= 3;
            instruction += atoi(arg1);
            instruction <<= 16;

            if (isNumber(arg2))
            {
                if (atoi(arg2) <= 32767 && atoi(arg2) >= -32768)
                    instruction += (atoi(arg2) & 0xFFFF);
                else
                    exit(1);
            }
            else
            {
                int found = 0;
                for (int i = 0; i < num_label; i++)
                {
                    if (strcmp(labels[i].label, arg2) == 0)
                    {
                        instruction += labels[i].addr;
                        found = 1;
                        break;
                    }
                }
                if (!found && (arg2[0] < 'A' && arg2[0] > 'Z'))
                    exit(1);
            }
        }
        else if (!strcmp(opcode, "sw"))
        {
            instruction = 0b011 << 3;
            instruction += atoi(arg0);
            instruction <<= 3;
            instruction += atoi(arg1);
            instruction <<= 16;

            if (isNumber(arg2))
            {
                if (atoi(arg2) <= 32767 && atoi(arg2) >= -32768)
                    instruction += (atoi(arg2) & 0xFFFF);
                else
                    exit(1);
            }
            else
            {
                int found = 0;
                for (int i = 0; i < num_label; i++)
                {
                    if (strcmp(labels[i].label, arg2) == 0)
                    {
                        instruction += labels[i].addr;
                        found = 1;
                        break;
                    }
                }
                if (!found && (arg2[0] < 'A' && arg2[0] > 'Z'))
                    exit(1);
            }
        }
        else if (!strcmp(opcode, "beq"))
        {
            instruction = 0b100 << 3;
            instruction += atoi(arg0);
            instruction <<= 3;
            instruction += atoi(arg1);
            instruction <<= 16;

            if (isNumber(arg2))
            {
                if (atoi(arg2) <= 32767 && atoi(arg2) >= -32768)
                    instruction += (atoi(arg2) & 0xFFFF);
                else
                    exit(1);
            }
            else
            {
                if (arg2[0] < 'A' && arg2[0] > 'Z')
                {
                    int found = 0;
                    for (int i = 0; i < global_symbol_size; i++)
                    {

                        if (strcmp(symbols[i].symbol, arg2) == 0)
                        {
                            instruction += ((symbols[i].addr - PC - 1) & 0xFFFF);
                            found = 1;

                            break;
                        }
                    }
                    if (!found)
                        break;
                }
                else
                {
                    int found = 0;
                    for (int i = 0; i < num_label; i++)
                    {

                        if (strcmp(labels[i].label, arg2) == 0)
                        {
                            instruction += ((labels[i].addr - PC - 1) & 0xFFFF);
                            found = 1;

                            break;
                        }
                    }

                    if (!found)
                        exit(1);
                }
            }
        }
        else if (!strcmp(opcode, "jalr"))
        {
            instruction = 0b101 << 3;
            instruction += atoi(arg0);
            instruction <<= 3;
            instruction += atoi(arg1);
            instruction <<= 16;
        }
        else if (!strcmp(opcode, "halt"))
        {
            instruction = 0b110;
            instruction <<= 22;
        }
        else if (!strcmp(opcode, "noop"))
        {
            instruction = 0b111;
            instruction <<= 22;
        }
        else if (!strcmp(opcode, ".fill"))
        {

            if (isNumber(arg0))
            {
                if (atoi(arg0) <= 2147483647 && atoi(arg0) >= -2147483648)
                {
                    instruction = atoi(arg0);
                }
                else
                    exit(1);
            }
            else
            {
                int found = 0;
                for (int i = 0; i < num_label; i++)
                {
                    if (strcmp(labels[i].label, arg0) == 0)
                    {
                        instruction += labels[i].addr;
                        found = 1;
                        break;
                    }
                }
                if (!found && (arg2[0] < 'A' && arg2[0] > 'Z'))
                    exit(1);
            }
        }
        else
        {
            exit(1);
        }

        fprintf(outFilePtr, "%d\n", instruction);

        PC++;

        instruction = 0;
    }

    for (int i = 0; i < global_symbol_size; i++)
    {
        if (symbols[i].addr == -1)
        {
            symbols[i].section = 'U';
            symbols[i].addr = 0;
        }
        else
        {
            if (symbols[i].addr < text_size)
                symbols[i].section = 'T';
            else
            {
                symbols[i].section = 'D';
                symbols[i].addr -= text_size;
            }
        }

        fprintf(outFilePtr, "%s %c %d\n", symbols[i].symbol, symbols[i].section, symbols[i].addr);
    }

    for (int i = 0; i < relocation_size; i++)
    {

        if (relocations[i].addr >= text_size)
        {
            relocations[i].addr -= text_size;
        }
        fprintf(outFilePtr, "%d %s %s\n", relocations[i].addr, relocations[i].operation, relocations[i].symbol);
    }

    return (0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if successfully read
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
                 char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL)
    {
        /* reached end of file */
        return (0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL)
    {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }

    /* is there a label? */
    char *ptr = line;
    if (sscanf(ptr, "%[^\t\n\r ]", label))
    {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
           opcode, arg0, arg1, arg2);
    return (1);
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return ((sscanf(string, "%d", &i)) == 1);
}

int label_defined(char label[7], struct labelInfo labels[MAXLINELENGTH], int label_size)
{

    for (int i = 0; i < label_size; i++)
    {
        if (strcmp(label, labels[i].label) == 0)
            return 1;
    }

    return 0;
}

int symbol_defined(char symbol[7], struct symbolInfo symbols[MAXLINELENGTH], int symbol_size)
{
    for (int i = 0; i < symbol_size; i++)
    {
        if (strcmp(symbol, symbols[i].symbol) == 0)
            return 1;
    }

    return 0;
}
