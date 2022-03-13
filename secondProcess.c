#include "secondProcess.h"

/*
 * FILE: secondProcess.c
 * GOAL: implements the second process of the assembler:
 	 - completes the encoding of lines which couldn't be fully-encoded.
 	 - marks symbols as "code-entry" or "data-entry".
 	 - appends all the used extern symbols in a special list.
 * ASSUMPTIONS: the file's syntax is correct. 
*/

boolean second_processing(FILE *file, char *fileName, symbol_table *node_symbol, machine_code *node_code, extern_sym *node_ext)
{
	unsigned int numLine; /* Holds the number of the current line */
	boolean success; /* A flag - notes whether the process succeeded */
	boolean finalSuccess; /* Notes whether the first process was done correctly*/
	char *line; /* Holds the current line of the file */
	
	success = True;
	finalSuccess = True;
	line = calloc(MAX_LINE_LENGTH + 2, sizeof(char));
	/* going over the lines, assuming they are grammatically correct */
	for(numLine = 1; fgets(line, MAX_LINE_LENGTH+2, file); numLine++)
	{
		if(success == False)
		{
			finalSuccess = False;
		}
		
		line = skipSpaces(line);
		/* An empty line or a comment */
		if(line[0]=='\n' || line[0]==';')
		{
			continue;
		}
		/* checking if its an ".entry", I-type or J-type line */
		if(strEqual(line, ".entry"))
		{
			line += strlen(".entry");
			line = skipSpaces(line);
			success = handleEntrySP(line, fileName, numLine, &node_symbol);
			continue;
		}
		else if(strEqual(line, "bne") || strEqual(line, "beq") || strEqual(line, "blt") || strEqual(line, "bgt"))
		{
			line += strlen("bne"); /* all of the conditional branching commands has the same length */
			line = skipSpaces(line);
			success = handleISP(line, fileName, numLine, &node_symbol, &node_code);
			continue;
		}
		else if(strEqual(line, "jmp"))
		{
			line += strlen("jmp");
			line = skipSpaces(line);
			/* if the command uses a register, its line is fully encoded */
			if(line[0]=='$') { continue; }
			else
			{
				success = handleJSP(line, fileName, numLine, &node_symbol, &node_code, &node_ext);
			}
		}
		else if(strEqual(line, "la") || strEqual(line, "call"))
		{
			while(!isspace(line[0])) { line++; }
			line = skipSpaces(line);
			success = handleJSP(line, fileName, numLine, &node_symbol, &node_code, &node_ext);
			continue;
		}
		/* skipping over a possible label */
		while(!(isspace(line[0]))) { line++; }
		line = skipSpaces(line);
		
		if(strEqual(line, ".entry"))
		{
			line += strlen(".entry");
			line = skipSpaces(line);
			success = handleEntrySP(line, fileName, numLine, &node_symbol);
			continue;
		}
		else if(strEqual(line, "bne") || strEqual(line, "beq") || strEqual(line, "blt") || strEqual(line, "bgt"))
		{
			line += strlen("bne"); /* all of the conditional branching commands has the same length */
			line = skipSpaces(line);
			success = handleISP(line, fileName, numLine, &node_symbol, &node_code);
			continue;
		}
		else if(strEqual(line, "jmp"))
		{
			line += strlen("jmp");
			line = skipSpaces(line);
			if(line[0]=='$') { continue; }
			else
			{
				success = handleJSP(line, fileName, numLine, &node_symbol, &node_code, &node_ext);
			}
		}
		else if(strEqual(line, "la") || strEqual(line, "call"))
		{
			while(!isspace(line)) { line++; }
			line = skipSpaces(line);
			success = handleJSP(line, fileName, numLine, &node_symbol, &node_code, &node_ext);
			continue;
		}
	}
	return finalSuccess;
}

boolean handleEntrySP(char *line, char *fileName, unsigned int numLine, symbol_table **node)
{
	boolean success; /* Notes whether the process succeeded */
	boolean foundSymbol; /* Notes whether the symbol was found */
	
	success = True;
	foundSymbol = False;
	/* searching for the symbol in the symbol-table */
	while(((*node)->next)!=NULL)
	{
		if(strEqual(line, (*node)->symbol))
		{
			if((*node)->attribute == EXTERNAL)
			{
				printf("[!] ERORR - FILE: \"%s\" - LINE: %d - \".extern\" labels cant be defined as \".entry\" as well.\n", fileName, numLine);
				success = False;
				break;
			}
			else
			{
				/* chaging the symbol's attribute */
				if((*node)->attribute == CODE) { (*node)->attribute = CODE_ENTRY; }
				else if((*node)->attribute == DATA) { (*node)->attribute = DATA_ENTRY; }
				foundSymbol = True;
				break; 
			}
		}
		node = &((*node)->next);
	}
	/* if the symbol wasn't defined in the file */
	if(success && !foundSymbol)
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - the label \"", fileName, numLine);
		while(!isspace(line[0])) { printf("%c", line[0]); line++; }
		printf("\" wasn't defined in the file.\n");
		success = False;
	}
	
	return success;
}

boolean handleISP(char *line, char *fileName, unsigned int numLine, symbol_table **node_symbol, machine_code **node_code)
{
	boolean success; /* Notes whether the process succeeded */
	boolean foundSymbol; /* Notes whether the symbol was found */
	int numOfCommas; /* counts the number of commas in the line */
	
	success = True;
	foundSymbol = False;
	numOfCommas = 0;
	
	/* skipping to the label */
	while(numOfCommas!=2)
	{
		if(line[0]==',')
		{
			numOfCommas++;
		}
		line++;
	}
	line = skipSpaces(line);
	
	/* searching for the symbol */
	while(((*node_symbol)->next)!=NULL)
	{
		if(strEqual(line, (*node_symbol)->symbol))
		{
			foundSymbol = True;
			if((*node_symbol)->attribute == EXTERNAL)
			{
				printf("[!] ERROR - FILE: \"%s\" - LINE: %d - labels in conditional branching can't be external.\n", fileName, numLine);
				success = False;
				break;
			}
			break;
		}
		node_symbol = &((*node_symbol)->next);
	}
	/* if the symbol wasn't defined in the file */
	if(success && !foundSymbol)
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - the label \"", fileName, numLine);
		while(!isspace(line[0])) { printf("%c", line[0]); line++; }
		printf("\" wasn't defined in the file.\n");
		success = False;
	}
	
	if(success)
	{
		/* searching for the same line in the machine-code linked-list */
		while(((*node_code)->numLine) != numLine)
		{
			node_code = &((*node_code)->next);
		}
		(*node_code)->mu.op->opu.I->immed = decToBin((*node_symbol)->value - (*node_code)->mu.op->opu.I->immed);
	}
	
	return success;
}

boolean handleJSP(char *line, char *fileName, unsigned int numLine, symbol_table **node_symbol, machine_code **node_code, extern_sym **node_ext)
{
	boolean success; /* Notes whether the process succeeded */
	boolean isExt; /* Notes whether the label is an ".extern" label */
	boolean foundSymbol; /* Notes whether the symbol was found */
	 
	success = True;
	isExt = False;
	foundSymbol = False;
	
	/* searching for the symbol */
	do
	{
		if(strEqual(line, (*node_symbol)->symbol))
		{
			if((*node_symbol)->attribute == EXTERNAL)
			{
				printf(" ");
				isExt = True;
			}
			foundSymbol = True;
			break;
		}
		node_symbol = &((*node_symbol)->next);
	} while(((*node_symbol)->next)!=NULL);
	/* if the symbol wasn't defined in the file */
	if(!foundSymbol)
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - the label \"", fileName, numLine);
		while(!isspace(line[0])) { printf("%c", line[0]); line++; }
		printf("\" wasn't defined in the file.\n");
		success = False;
	}
	/* searching for the same line in the machine-code linked-list */
	else
	{
		while(((*node_code)->numLine) != numLine)
		{
			node_code = &((*node_code)->next);
		}
		/* if it's an ".extern" label, save the machine-code line address (which is kept in the encoding) */
		if(isExt)
		{
			success = addExtSymNode(node_ext, (long)((*node_code)->mu.op->opu.J->address), (*node_symbol)->symbol, fileName, numLine);
		}
		if(success)
		{
			(*node_code)->mu.op->opu.J->address = decToBin((*node_symbol)->value);
		}
	}

	return success;
}


