/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

int checkDuplicateGlobalandStack(CombinedFiles final);
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
	int traceT = 0, traceD = final.textSize, traceR = 0, traceS = 0;
	for (int i = 0; i < argc - 2; i++)
	{
		for (int t = 0; t < files[i].textSize; t++)
		{
			final.text[traceT++] = files[i].text[t];
		}
		for (int d = 0; d < files[i].dataSize; d++)
		{
			final.data[traceD++] = files[i].data[d];
		}
		for (int s = 0; s < files[i].symbolTableSize; s++)
		{
			if (files[i].symbolTable[s].location != 'U')
				final.symTable[traceS++] = files[i].symbolTable[s];
		}
		for (int r = 0; r < files[i].relocationTableSize; r++)
		{
			final.relocTable[traceR++] = files[i].relocTable[r];
		}
	}
	// ! Check for duplicate global labels and definition of Stack
	if (checkDuplicateGlobalandStack(final) == 1)
		exit(1);

	for (int i = 0; i < argc - 2; i++)
	{
		for (int j = 0; j < files[i].relocationTableSize; j++)
		{
			int text, offset, additonal_offset = 0;
			text = files[i].text[files[i].relocTable[j].offset];
			offset = text & 0x00FF;

			// If the symbol involved is global
			if (isupper(files[i].relocTable[j].label[0]))
			{

				int temp[2];
				temp[0] = -1;

				char global[7];
				strcpy(global, files[i].relocTable[j].label);

				// If this global label is "Stack"
				if (strcmp(global, "Stack") == 0)
				{

					// When the line using symbol is in text
					if (files[i].relocTable[j].offset + additonal_offset < files[i].textSize)
					{
						final.text[files[i].textStartingLine + j] += (final.textSize + final.dataSize);
					}
					// When the line using the symbol is in data
					else
					{
						final.data[files[i].textStartingLine + j] += (final.textSize + final.dataSize);
					}

					continue;
				}

				// If this global label is .fill
				if (strcmp(files[i].relocTable[j].inst, ".fill") == 0)
				{
					additonal_offset = files[i].textSize;
				}

				// Search for the global label
				for (int i = 0; i < argc - 2; i++)
				{
					for (int j = 0; j < files[i].symbolTableSize; j++)
					{

						if (strcmp(files[i].symbolTable[j].label, global) == 0 && files[i].symbolTable[j].location != 'U')
						{
							// If the global symbol is in text
							if (files[i].symbolTable[j].location == 'T')
							{
								temp[0] = 0;
								temp[1] = files[i].textStartingLine + files[i].symbolTable[j].offset;
							}
							// If the global symbol is in data
							else
							{
								temp[0] = 1;
								temp[1] = files[i].dataStartingLine + files[i].symbolTable[j].offset;
							}
						}
					}
				}

				// ! Error checking: when the global symbol is undefined
				if (temp[0] == -1)
					exit(1);

				// When the line using symbol is in text
				if (files[i].relocTable[j].offset + additonal_offset < files[i].textSize)
				{
					// If the label is in text
					if (temp[0] == 0)
					{
						final.text[files[i].textStartingLine + j] += temp[1];
					}
					// If the label is in data
					else
					{
						final.text[files[i].textStartingLine + j] += temp[1];
					}
				}
				// When the line using the symbol is in data
				else
				{
					// If the label is in text
					if (temp[0] == 0)
					{
						final.data[files[i].dataStartingLine + j] += temp[1];
					}
					// If the label is in data
					else
					{
						final.data[files[i].dataStartingLine + j] += temp[1];
					}
				}
			}
			// Local label
			else
			{
				if (strcmp(files[i].relocTable[j].inst, ".fill") == 0)
				{
					additonal_offset = files[i].textSize;
				}

				// When the line using symbol is in text
				if (files[i].relocTable[j].offset + additonal_offset < files[i].textSize)
				{
					// If the label is in text
					if (offset < files[i].textSize)
					{
						final.text[files[i].textStartingLine + j] -= offset;
						final.text[files[i].textStartingLine + j] += files[i].textStartingLine + offset;
					}
					// If the label is in data
					else
					{
						final.text[files[i].textStartingLine + j] -= offset;
						final.text[files[i].textStartingLine + j] += files[i].dataStartingLine + offset - files[i].textSize;
					}
				}
				// When the line using the symbol is in data
				else
				{
					// If the label is in text
					if (offset < files[i].textSize)
					{
						final.data[files[i].dataStartingLine + j] -= offset;
						final.data[files[i].dataStartingLine + j] += files[i].textStartingLine + offset;
					}
					// If the label is in data
					else
					{
						final.data[files[i].dataStartingLine + j] -= offset;
						final.data[files[i].dataStartingLine + j] += files[i].dataStartingLine + offset - files[i].textSize;
					}
				}
			}
		}
	}

	for (int t = 0; t < final.textSize; t++)
	{
		fprintf(outFilePtr, "%d\n", final.text[t]);
	}
	for (int d = final.textSize; d < final.textSize + final.dataSize; d++)
	{
		fprintf(outFilePtr, "%d\n", final.data[d]);
	}

	return 0;
} // main

int checkDuplicateGlobalandStack(CombinedFiles final)
{
	for (int g = 0; g < final.symTableSize; g++)
	{
		for (int j = 0; j < final.symTableSize && g != j; j++)
			if (strcmp(final.symTable[g].label, final.symTable[j].label) == 0 || strcmp(final.symTable[g].label, "Stack") == 0)
				return 1;
	}

	return 0;
}

int countTextSize(FileData files[MAXFILES], int argc)
{
	int count = 0;
	for (int i = 0; i < argc - 2; i++)
	{
		count += files[i].textSize;
	}
	return count;
}

int countDataSize(FileData files[MAXFILES], int argc)
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
		for (int j = 0; j < files[i].symbolTableSize; j++)
		{
			if (files[i].symbolTable[j].location != 'U')
				count++;
		}
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
