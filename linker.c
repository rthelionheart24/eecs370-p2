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

/*int checkDuplicateGlobalandStack(CombinedFiles final);
int countTextSize(struct FileData files[MAXFILES], int argc);
int countDataSize(struct FileData files[MAXFILES], int argc);
int countSymbolTableSize(struct FileData files[MAXFILES], int argc);
int countRelocationTableSize(struct FileData files[MAXFILES], int argc);
int isUpper(char *string);
*/
int findGlobal(FileData files[MAXFILES], char *label, int argc);
int main(int argc, char *argv[])
{
	int textStart = 0;
	int totalLines = 0;
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
		files[i].textStartingLine = textStart;
		totalLines += files[i].textSize + files[i].dataSize;

		// read in text section
		int instr;
		for (j = 0; j < sizeText; j++)
		{
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = atoi(line);
			files[i].text[j] = instr;
			++textStart;
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

	int calcData = 0;
	for (int i = 0; i < argc - 2; ++i)
	{
		calcData += files[i].textSize;
	}
	for (int i = 0; i < argc - 2; ++i)
	{
		files[i].dataStartingLine = calcData;
		calcData += files[i].dataSize;
	}

	for (int i = 0; i < argc - 2; i++)
	{
		for (int j = 0; j < files[i].symbolTableSize; j++)
		{
			if (strcmp(files[i].symbolTable[j].label, "Stack") == 0 && files[i].symbolTable[j].location != 'U')
				exit(1);
		}
	}

	for (int i = 0; i < argc - 2; ++i)
	{
		for (int j = 0; j < files[i].symbolTableSize; ++j)
		{
			if (isupper(files[i].symbolTable[j].label[0]) && files[i].symbolTable[j].location != 'U')
			{
				for (int k = 0; k < argc - 2; ++k)
				{
					for (int l = 0; l < files[k].symbolTableSize; ++l)
					{
						if (!strcmp(files[k].symbolTable[l].label, files[i].symbolTable[j].label) && files[k].symbolTable[l].location != 'U' && !(i == k && j == l))
						{
							printf("%s", "Duplicate global label");
							exit(1);
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < argc - 2; ++i)
	{
		for (int j = 0; j < files[i].symbolTableSize; ++j)
		{
			if (isupper(files[i].symbolTable[j].label[0]) && files[i].symbolTable[j].location == 'U' && strcmp(files[i].symbolTable[j].label, "Stack"))
			{
				int found = 0;
				for (int k = 0; k < argc - 2; ++k)
				{
					for (int l = 0; l < files[k].symbolTableSize; ++l)
					{
						if (!strcmp(files[i].symbolTable[j].label, files[k].symbolTable[l].label) &&
							files[k].symbolTable[l].location != 'U')
						{
							found = 1;
						}
					}
				}
				if (!found)
				{
					printf("%s", "Undefined global variable");
					exit(1);
				}
			}
		}
	}

	for (int i = 0; i < argc - 2; i++)
	{
		for (int j = 0; j < files[i].relocationTableSize; j++)
		{

			// If the symbol involved is global
			if (isupper(files[i].relocTable[j].label[0]))
			{
				int globeStore = 0;
				if (!strcmp(files[i].relocTable[j].inst, "lw") || !strcmp(files[i].relocTable[j].inst, "sw"))
				{
					if (!strcmp(files[i].relocTable[j].label, "Stack"))
					{
						files[i].text[files[i].relocTable[j].offset] += totalLines;
					}
					else
					{
						for (int a = 0; a < files[i].symbolTableSize; ++a)
						{
							if (!strcmp(files[i].symbolTable[a].label, files[i].relocTable[j].label))
							{
								if (files[i].symbolTable[a].location == 'U')
								{
									globeStore = findGlobal(files, files[i].relocTable[j].label, argc - 2);
								}
								else if (files[i].symbolTable[a].location == 'T')
								{
									globeStore = files[i].textStartingLine;
								}
								else if (files[i].symbolTable[a].location == 'D')
								{
									globeStore = files[i].dataStartingLine - files[i].textSize;
								}
							}
						}
						files[i].text[files[i].relocTable[j].offset] += globeStore;
					}

					//final.text[files[i].textStartingLine + j] += globeStore;
				}
				else if (!strcmp(files[i].relocTable[j].inst, ".fill"))
				{
					if (!strcmp(files[i].relocTable[j].label, "Stack"))
					{
						files[i].data[files[i].relocTable[j].offset] += totalLines;
					}
					else
					{
						for (int a = 0; a < files[i].symbolTableSize; ++a)
						{
							if (!strcmp(files[i].symbolTable[a].label, files[i].relocTable[j].label))
							{
								if (files[i].symbolTable[a].location == 'U')
								{
									globeStore = findGlobal(files, files[i].relocTable[j].label, argc - 2);
								}
								else if (files[i].symbolTable[a].location == 'T')
								{
									globeStore = files[i].textStartingLine + files[i].symbolTable[a].offset;
								}
								else if (files[i].symbolTable[a].location == 'D')
								{
									globeStore = files[i].dataStartingLine + files[i].symbolTable[a].offset;
								}
							}
						}
						files[i].data[files[i].relocTable[j].offset] = globeStore;
					}

					//final.data[files[i].dataStartingLine + j] = globeStore;
				}
			}
			// Local label
			else
			{
				if (!strcmp(files[i].relocTable[j].inst, "lw") || !strcmp(files[i].relocTable[j].inst, "sw"))
				{
					int offset = files[i].text[files[i].relocTable[j].offset] & 0x00FF;
					//	printf("%s\n", files[i].relocTable[j].inst);
					//	printf("%s\n", files[i].relocTable[j].label);
					//	printf("%s%d\n", "offset: ", offset);
					int temp = 0;
					if (offset < files[i].textSize)
					{
						temp = files[i].textStartingLine + offset;
					}
					else
					{
						temp = files[i].dataStartingLine + offset - files[i].textSize;
					}
					temp -= offset;
					//	printf("%s%d\n", "OFFSET ORiginal", files[i].text[files[i].relocTable[j].offset]);
					//	printf("%s%d\n", "temp: ", temp);
					files[i].text[files[i].relocTable[j].offset] += temp;
				}
				else if (!strcmp(files[i].relocTable[j].inst, ".fill"))
				{
					int temp = files[i].data[files[i].relocTable[j].offset];
					if (temp < files[i].textSize)
					{
						temp = files[i].textStartingLine + temp;
					}
					else
					{
						temp = files[i].dataStartingLine + temp - files[i].textSize;
					}
					files[i].data[files[i].relocTable[j].offset] = temp;
				}
			}
		}
	}
	for (int i = 0; i < argc - 2; ++i)
	{
		for (int j = 0; j < files[i].textSize; ++j)
		{
			fprintf(outFilePtr, "%d\n", files[i].text[j]);
		}
	}

	for (int i = 0; i < argc - 2; ++i)
	{
		for (int j = 0; j < files[i].dataSize; ++j)
		{
			fprintf(outFilePtr, "%d\n", files[i].data[j]);
		}
	}

	return 0;
}

int findGlobal(FileData files[MAXFILES], char *label, int argc)
{
	int pos = -1;
	for (int i = 0; i < argc; ++i)
	{
		for (int j = 0; j < files[i].symbolTableSize; ++j)
		{
			if (!strcmp(files[i].symbolTable[j].label, label))
			{
				if (files[i].symbolTable[j].location == 'D')
				{
					pos = files[i].dataStartingLine + files[i].symbolTable[j].offset;
				}
				else if (files[i].symbolTable[j].location == 'T')
				{
					pos = files[i].textStartingLine + files[i].symbolTable[j].offset;
				}
			}
		}
	}
	return pos;
}