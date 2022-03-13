#include "firstProcess.h"
#include "secondProcess.h"
#include "writeFiles.h"

/*
 * Maman14 - course 20465 - b2021
 * AUTHOR: Tom Weiner (326382363).
 * DATE: 12/08/2021
 * FILES INCLUDED: 
 	 - assembler.c
 	 - firstProcess.c, firstProcess.h
 	 - secondProcess.c, secondProcess.h
 	 - writeFiles.c, writeFiles.h
 	 - handleFiles.c, handleFiles.h
 	 - testing files (ps.as, as.as, noExtEnt.as, firstErrorFile.as, secondErrorFile.as) and their outputs.
*/

/*
 * FILE: assembler.c
 * GOAL: implements the assembler:
 	 - opens the files.
 	 - calls the first-process and the second-process.
 	 - writes the output files of the assembler.
 	 - frees all the linked-lists.
 * ASSUMPTIONS: none. 
*/


/*
 * Handles the processing of the file.
 * Calls both the first-processing function and the second-processing function.
 * INPUT - "fileName": the name of the file.
 * OUTPUT - none. 
*/
void handle_file(char *fileName);


/*
 * Updates symbols' values if the have a specific attribute.
 * INPUT - "node": the head of the symbol_table.
           "value": the value which will be added to the symbols.
           "attr": the referenced attribute.
 * OUTPUT - none.
*/
void updateSymbolByAttr(symbol_table *node, int value, sym_attr attr);


int main(int argc, char *argv[])
{
	
	int i; /* An iterator */
	
	for(i = 1; i<argc; i++)
	{
		handle_file(argv[i]);
		printf("\n--------------------\n");
	}
	printf("\n");
	return 0;
}

void handle_file(char *fileName)
{
	FILE *file; /* A pointer to the current file */
	boolean success; /* A flag to note whether the process succeeded */
	int IC; /* Instruction counter */
	int DC; /* Data counter */
	int ICF; /* Final IC */
	int DCF; /* Final DC */
	symbol_table *head_symbol; /* The head of the symbol-table linked-list */
	machine_code *head_code; /* The head of the machine-code linked-list */
	extern_sym *head_extern; /* The head of the external-symbols linked-list */
	
	head_symbol = (symbol_table*)calloc(1, sizeof(symbol_table));
	head_code = (machine_code*)calloc(1, sizeof(machine_code));
	head_extern = (extern_sym*)calloc(1, sizeof(extern_sym));
	/* If there wasn't enough memory for the allocations */
	if(!(head_symbol) || !(head_code) || !(head_extern))
	{
		printf("[!] ERROR - FILE: \"%s\" - ran out of memory.\n", fileName);
		free(head_symbol);
		free(head_code);
		free(head_extern);
		return;
	}
	head_symbol->next = NULL;
	head_code->next = NULL;
	head_extern->next = NULL;
	head_extern->address = 0; /* helps writing the ".ext" file */
	
	/* If the file isn't an assembly-file */
	if(fileName[strlen(fileName)-1]!='s' || fileName[strlen(fileName)-2]!='a' || fileName[strlen(fileName)-3]!='.')
	{
		printf("[!] ERROR - FILE: \"%s\" - the file isn't an assembly-source file.\n", fileName);
		free(head_symbol);
		free(head_code);
		free(head_extern);
		return;
	}
	 
	file = fopen(fileName, "r");
	/* If the file couldn't be opened for reading */
	if(!(file))
	{
		printf("[!] ERROR - FILE: \"%s\" - could not open the file. \n", fileName);
		free(head_symbol);
		free(head_code);
		free(head_extern);
		return;
	}
	/* First-processing the file */
	success = first_processing(file, fileName, &IC, &DC, head_symbol, head_code);
	/* If the first-processing failed */
	if(!(success))
	{
		/* freeing the linked-lists */
		freeCodeList(head_code);
		freeSymbolList(head_symbol);
		freeExternList(head_extern);
		fclose(file);
		return;
	}
	ICF = IC;
	DCF = DC;
	updateSymbolByAttr(head_symbol, ICF, DATA);
	fseek(file, 0, SEEK_SET);
	success = second_processing(file, fileName, head_symbol, head_code, head_extern);
	/* if the second-processing failed */
	if(!(success))
	{
		/* freeing the linked-lists */
		freeCodeList(head_code);
		freeSymbolList(head_symbol);
		freeExternList(head_extern);
		fclose(file);
		return;
	}
	/*fclose(file);*/
	
	/* deleting the ".as" part from the tail of fileName */
	fileName[strlen(fileName)-1] = '\0';
	fileName[strlen(fileName)-2] = '\0';
	
	/* writing the .ob, .ent, .ext files (if needed) */
	writeObFile(fileName, head_code, ICF, DCF);
	writeEntFile(fileName, head_symbol);
	writeExtFile(fileName, head_extern);

	/* freeing the linked-lists */
	freeCodeList(head_code);
	freeSymbolList(head_symbol);
	freeExternList(head_extern);
	
	return;
}

void updateSymbolByAttr(symbol_table *node, int value, sym_attr attr)
{
	while(node!=NULL)
	{
		if((node)->attribute==attr)
		{
			/* updates the value */
			(node)->value += value;
		}
		node = node->next;
	}
}
