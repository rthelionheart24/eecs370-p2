/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXSIZE 300
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry
{
	char label[7];
	char location;
	int offset;
};

struct RelocationTableEntry
{
	int offset;
	char inst[7];
	char label[7];
	int file;
};

struct FileData
{
	int textSize;
	int dataSize;
	int symbolTableSize;
	int relocationTableSize;
	int textStartingLine; // in final executable
	int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles
{
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
	int textSize;
	int dataSize;
	int symTableSize;
	int relocTableSize;
};
int checkRelocationTableForText(FileData files, int location);
int checkRelocationTableForData(FileData files, int location);
int countTextSize(struct FileData files[MAXFILES], int argc);
int countDataSize(struct FileData files[MAXFILES], int argc);
int countSymbolTableSize(struct FileData files[MAXFILES], int argc);
int countRelocationTableSize(struct FileData files[MAXFILES], int argc);

int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	int i, j;

	if (argc <= 2)
	{
		printf("error: usage: %s <obj file> ... <output-exe-file>\n",
			   argv[0]);
		exit(1);
	}

	outFileString = argv[argc - 1];

	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL)
	{
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	FileData files[MAXFILES];

	// read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; i++)
	{
		inFileString = argv[i + 1];

		inFilePtr = fopen(inFileString, "r");
		printf("opening %s\n", inFileString);

		if (inFilePtr == NULL)
		{
			printf("error in opening %s\n", inFileString);
			exit(1);
		}

		char line[MAXLINELENGTH];
		int sizeText, sizeData, sizeSymbol, sizeReloc;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
			   &sizeText, &sizeData, &sizeSymbol, &sizeReloc);

		files[i].textSize = sizeText;
		files[i].dataSize = sizeData;
		files[i].symbolTableSize = sizeSymbol;
		files[i].relocationTableSize = sizeReloc;

		// read in text section
		int instr;
		for (j = 0; j < sizeText; j++)
		{
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = atoi(line);
			files[i].text[j] = instr;
		}

		// read in data section
		int data;
		for (j = 0; j < sizeData; j++)
		{
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = atoi(line);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		int addr;
		for (j = 0; j < sizeSymbol; j++)
		{
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
				   label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < sizeReloc; j++)
		{
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
				   &addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file = i;
		}
		fclose(inFilePtr);
	} // end reading files

	// *** INSERT YOUR CODE BELOW ***
	//    Begin the linking process
	//    Happy coding!!!

	CombinedFiles final;
	final.textSize = countTextSize(files, argc);
	final.dataSize = countDataSize(files, argc);
	final.symTableSize = countSymbolTableSize(files, argc);
	final.relocTableSize = countRelocationTableSize(files, argc);

	files[0].textStartingLine = 0;
	files[0].dataStartingLine = final.textSize;

	for (int i = 1; i < argc - 2; i++)
	{

		files[i].textStartingLine = files[i - 1].textStartingLine + files[i - 1].textSize;
		files[i].dataStartingLine = files[i - 1].dataStartingLine + files[i - 1].dataSize;
	}

	for (int i = 0; i < argc - 2; i++)
	{
		for (int j = 0; j < files[i].textSize; j++)
		{
			// If the current text involves the use of relocation table
			if (checkRelocationTableForText(files[i], j) != -1)
			{
				// ! Do something here to modified the text that uses symbolic address
				// ! May need to differentiate between global and local

				int offset_value = files[i].text[j] & 0x00FF;

				// When the symbol is in the data section
				if (offset_value > files[i].textSize - 1)
				{
					files[i].text[j] = files[i].text[j] - offset_value + files[i].dataStartingLine + (j + 1 - files[i].dataSize);
				}
				// When the symbol is in the text section
				else
				{
				}

				fprintf(outFilePtr, "%d, This line needs to be modified\n", files[i].text[j]);
			}
			else
				fprintf(outFilePtr, "%d\n", files[i].text[j]);
		}
	}

	for (int i = 0; i < argc - 2; i++)
	{
		for (int j = 0; j < files[i].dataSize; j++)
		{
			// If the current text involves the use of relocation table
			if (checkRelocationTableForData(files[i], j) != -1)
			{
				// ! Do something here to modified the data that uses symbolic address
				// ! May need to differentiate between global and local

				/** 
				 * * When the label is local, we can just shift it 
				 * * by the starting location of the current file's
				 * * text
				 * */
				files[i].data[j] += files[i].textStartingLine;

				/**
				 * * If the label is global:
				 * */

				fprintf(outFilePtr, "%d, This line needs to be modified\n", files[i].data[j]);
			}
			else
				fprintf(outFilePtr, "%d\n", files[i].data[j]);
		}
	}

} // main

int checkRelocationTableForText(FileData files, int location)
{
	for (int i = 0; i < files.relocationTableSize; i++)
	{
		if (files.relocTable[i].offset == location)
			return i;
	}

	return -1;
}

int checkRelocationTableForData(FileData files, int location)
{
	for (int i = 0; i < files.relocationTableSize; i++)
	{
		if (files.relocTable[i].offset == location && strcmp(files.relocTable[i].inst, ".fill") == 0)
			return i;
	}

	return -1;
}

int countTextSize(struct FileData files[MAXFILES], int argc)
{
	int count = 0;
	for (int i = 0; i < argc - 2; i++)
	{
		count += files[i].textSize;
	}
	return count;
}

int countDataSize(struct FileData files[MAXFILES], int argc)
{
	int count = 0;
	for (int i = 0; i < argc - 2; i++)
	{
		count += files[i].dataSize;
	}
	return count;
}

int countSymbolTableSize(struct FileData files[MAXFILES], int argc)
{
	int count = 0;
	for (int i = 0; i < argc - 2; i++)
	{
		count += files[i].symbolTableSize;
	}
	return count;
}

int countRelocationTableSize(struct FileData files[MAXFILES], int argc)
{
	int count = 0;
	for (int i = 0; i < argc - 2; i++)
	{
		count += files[i].relocationTableSize;
	}
	return count;
}
