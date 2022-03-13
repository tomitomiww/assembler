#include "firstProcess.h"

/*
 * FILE: firstProcess.c
 * GOAL: implements the first process of the assembler:
 	 - checks the syntax of the processed file.
 	 - appends the symbols to the symbol-table.
 	 - encodes all of the file's lines (if possible).
 * ASSUMPTIONS: the file is an *.as file and is readable. 
*/

/* lookup table of various commands */
lookup_element LOOKUP_TABLE[] = {
			{"add", R, NULL_DATA, ADD_FUNCT, ADD_OPC},
			{"sub", R, NULL_DATA, SUB_FUNCT, SUB_OPC},
			{"or", R, NULL_DATA, OR_FUNCT, OR_OPC},
			{"nor", R, NULL_DATA, NOR_FUNCT, NOR_OPC},
			{"move", R, NULL_DATA, MOVE_FUNCT, MOVE_OPC},
			{"mvhi", R, NULL_DATA, MVHI_FUNCT, MVHI_OPC},
			{"mvlo", R, NULL_DATA, MVLO_FUNCT, MVLO_OPC},
			{"addi", I, NULL_DATA, NULL_FUNCT, ADDI_OPC},
			{"subi", I, NULL_DATA, NULL_FUNCT, SUBI_OPC},
			{"andi", I, NULL_DATA, NULL_FUNCT, ANDI_OPC},
			{"ori", I, NULL_DATA, NULL_FUNCT, ORI_OPC},
			{"nori", I, NULL_DATA, NULL_FUNCT, NORI_OPC},
			{"bne", I, NULL_DATA, NULL_FUNCT, BNE_OPC},
			{"beq", I, NULL_DATA, NULL_FUNCT, BEQ_OPC},
			{"blt", I, NULL_DATA, NULL_FUNCT, BLT_OPC},
			{"bgt", I, NULL_DATA, NULL_FUNCT, BGT_OPC},
			{"lb", I, NULL_DATA, NULL_FUNCT, LB_OPC},
			{"sb", I, NULL_DATA, NULL_FUNCT, SB_OPC},
			{"lw", I, NULL_DATA, NULL_FUNCT, LW_OPC},
			{"sw", I, NULL_DATA, NULL_FUNCT, SW_OPC},
			{"lh", I, NULL_DATA, NULL_FUNCT, LH_OPC},
			{"sh", I, NULL_DATA, NULL_FUNCT, SH_OPC},
			{"jmp", J, NULL_DATA, NULL_FUNCT, JMP_OPC},
			{"la", J, NULL_DATA, NULL_FUNCT, LA_OPC},
			{"call", J, NULL_DATA, NULL_FUNCT, CALL_OPC},
			{"stop", J, NULL_DATA, NULL_FUNCT, STOP_OPC},
			{".db", NULL_OPT, DB, NULL_FUNCT, NULL_OPC},
			{".dh", NULL_OPT, DH, NULL_FUNCT, NULL_OPC},
			{".dw", NULL_OPT, DW, NULL_FUNCT, NULL_OPC},
			{".asciz", NULL_OPT, ASCIZ, NULL_FUNCT, NULL_OPC},
			{"\0", NULL_OPT, NULL_DATA, NULL_FUNCT, NULL_OPC}
			
};


boolean first_processing(FILE *file, char *fileName, int *IC, int *DC, symbol_table *node_symbol, machine_code *node_code)
{
	unsigned int numLine; /* Holds the number of the current line */
	boolean success; /* A flag - notes whether the process succeeded */
	boolean finalSuccess; /* Notes whether the first process was done correctly*/
	boolean isThereLabel; /* A flag - notes whether a label is defined in the line */
	boolean labelExtEnt; /* A flag - notes if a label is defined in an ".extern" or "entry" line */
	char *line; /* Holds the current line of the file */
	lookup_element element; /* Holds the information about the operation/data-word (type, funct, etc.) */
	int labelLength; /* The length of the label which is defined in the line */
	
	/* Initialization */
	success = True;
	finalSuccess = True;
	*IC = INIT_IC_VALUE;
	*DC = 0;
	line = calloc(MAX_LINE_LENGTH + 2, sizeof(char));
	
	/* going over all the lines in the file */
	for(numLine = 1; fgets(line, MAX_LINE_LENGTH+2, file); numLine++)
	{
		/* if a line couldn't be handles correctly */
		if(success == False)
		{
			finalSuccess = False;
		}
		isThereLabel = False;
		
		/* If the line is longer than allowed */
		if(!(feof(file)) && !(strchr(line, '\n')))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - line's length is bigger than allowed ", fileName, numLine);
			printf("(max length: %d characters)\n", MAX_LINE_LENGTH);
			do
			{
				fgets(line, MAX_LINE_LENGTH+2, file);
			} while(!(feof(file)) && !(strchr(line, '\n')));
			do
			{
				line++;
			} while(line[0]!='\n' && line[0]!=EOF);
			success = False;
			continue;	        
		}
		line = skipSpaces(line);
		
		/* An empty line or a comment */
		if(line[0]=='\n' || line[0]==';')
		{
			continue;
		}
		
		/* A possible operation or data line */
		if(islower(line[0]) || line[0]=='.')
		{
			line = skipSpaces(line);
			element = getElement(line, fileName, numLine);
			if(element.word!=NULL)
			{	
				/* If the saved word is being used as a label */
				if(line[strlen(element.word)]==':')
				{
					printf("[!] ERROR - FILE: \"%s\" - LINE: %d - labels names can't be saved words (%s).\n", fileName, numLine, element.word);
				}
				/* If it's not a data line */
				else if(element.data==NULL_DATA)
				{
					success = handleOpLine(line, fileName, numLine, IC, DC, &node_symbol, &node_code, element, isThereLabel);
				}
				/* If it's not an operation line */
				else if(element.opT==NULL_OPT)
				{
					success = handleDataLine(line, fileName, numLine, IC, DC, &node_symbol, &node_code, element, isThereLabel);
				}
				continue;
			}
		}
		
		/* A possible extern or entry line */
		if(line[0]=='.')
		{
			/* If it's an extern line */
			if(strEqual(line, ".extern"))
			{
				success = handleExtEntLine(line, fileName, numLine, IC, DC, &node_symbol, &node_code);
				continue;
			}	
			/* If it's an entry line */
			if(strEqual(line, ".entry"))
			{
				success = handleExtEntLine(line, fileName, numLine, IC, DC, &node_symbol, &node_code);
				continue;
			}	
		}
		/* At this point there must be a label */
		
		isThereLabel = True;
		labelExtEnt = isExtEnt(line, fileName, numLine);
		/* If it's a label in an entry or extern line */
		if(labelExtEnt)
		{
			while(!(isspace(line[0]))) { line++; }
			line = skipSpaces(line);
			success = handleExtEntLine(line, fileName, numLine, IC, DC, &node_symbol, &node_code);
			continue;
		}
		labelLength = isLabelValid(line, fileName, numLine, False);
		
		/* If the label isn't valid */
		if(!labelLength) { continue; }
		/* if the label isn't followed by ':' */
		if(line[labelLength]!=':')
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - labels decelerations must be followed by colon.\n", fileName, numLine);
			continue;
		}
		
		line += labelLength;
		line++; /* skipping the ':' as well */
		line = skipSpaces(line);
		element = getElement(line, fileName, numLine);
		if(element.word!=NULL)
		{	
			/* If it's not a data line */
			if(element.data==NULL_DATA)
			{
				success = handleOpLine(line-labelLength-2, fileName, numLine, IC, DC, &node_symbol, &node_code, element, isThereLabel);
			}
			/* If it's not an operation line */
			else if(element.opT==NULL_OPT)
			{
				success = handleDataLine(line-labelLength-2, fileName, numLine, IC, DC, &node_symbol, &node_code, element, isThereLabel);
			}
			continue;
		}
		/* if element.word==NULL, than the command which was written isn't valid */
		labelLength = 0; /* We'll use "labelLength" as an iterator, since it's no longer important */
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - \"", fileName, numLine);
		while(!(isspace(line[labelLength])) && labelLength<strlen(line))
		{
			printf("%c", line[labelLength]);
			labelLength++;
		}
		printf("\" isn't a valid command.\n");
		continue;
	}
	
	return finalSuccess;
}

lookup_element getElement(char *line, char *fileName, unsigned int numLine)
{
	int i; /* An iterator */
	
	for(i = 0; LOOKUP_TABLE[i].word!=NULL; i++)
	{
		if(strEqual(line, LOOKUP_TABLE[i].word))
		{
			return LOOKUP_TABLE[i];
		}
	}
	return LOOKUP_TABLE[i];
}

boolean isExtEnt(char *line, char *fileName, unsigned int numLine)
{
	/* skipping the label */
	while(!(isspace(line[0]))) { line++; }
	line = skipSpaces(line);
	
	/* There must be a dot in ".extern" or ".entry" */
	if(line[0]!='.')
	{
		return False;
	}
	
	line++;
	
	if(strEqual(line, "extern"))
	{
		printf("[?] WARNING - FILE \"%s\" - LINE: %d - ", fileName, numLine);
		printf("a label in the beginning of an \".extern\" line is meaningless.\n");
		return True;
	}
	if(strEqual(line, "entry"))
	{
		printf("[?] WARNING - FILE \"%s\" - LINE: %d - ", fileName, numLine);
		printf("a label in the beginning of an \".entry\" line is meaningless.\n");
		return True;
	}
	return False;
	
}

boolean handleOpLine(char *line, char *fileName, unsigned int numLine, int *IC, int *DC, symbol_table **node_symbol,
		     machine_code **node_code, lookup_element element, boolean isThereLabel)
{
	boolean success; /* Notes whether the process succeeded */
	int labelLength; /* Holds the label's length, if defined in the line */
	
	/* initialization */
	success = True;
	labelLength = 0;
	
	if(isThereLabel)
	{
		/* adding the label to the symbol-table */
		for(; !(isspace(line[labelLength]))&&line[labelLength]!=':'; labelLength++) {}
		success = addSymbol(line, fileName, numLine, node_symbol, *IC, CODE, labelLength);
	}
	if(success)
	{
		/* The program handled the label well */
		line += labelLength+1;
		line = skipSpaces(line);
		line += strlen(element.word);
		line = skipSpaces(line);
		
		if(element.opT == R)
		{
			success = handleRLine(line, fileName, numLine, node_code, element);
		}
		else if(element.opT == I)
		{
			success = handleILine(line, fileName, numLine, node_code, element, *IC);
		}
		else if(element.opT == J)
		{
			success = handleJLine(line, fileName, numLine, node_code, element, *IC);
		}
	}
	/* If the encoding succeeded, increasing the instruction-count */
	if(success) { *IC += 4; }
	
	return success;
}

boolean handleRLine(char *line, char *fileName, unsigned int numLine, machine_code **node_code, lookup_element element)
{
	boolean success; /* Notes whether the process succeeded */
	reg rs, rt, rd, tempReg; /* The registers which will be called */
	
	/* initialization */
	success = True;
	rs = NULL_REG;
	rt = NULL_REG;
	rd = NULL_REG;
	tempReg = NULL_REG;
	
	while((rd == NULL_REG) && (line[0]!='\n') && (line[0]!=EOF))
	{
		line = skipSpaces(line);
		tempReg = getReg(line, fileName, numLine);
		if(tempReg==NULL_REG)
		{
			success = False;
			break;
		}
		else if(rs==NULL_REG) { rs = tempReg; }
		else if(rt==NULL_REG) { rt = tempReg; }
		else if(rd==NULL_REG) { rd = tempReg; }
		if(tempReg<10) { line +=2; } /* the register takes two characters */
		else { line += 3; } /* the register takes three characters */
		if(rd==NULL_REG)
		{
			if((element.opC == 1) && (rt!=NULL_REG))
			{
				/* rt isn't used in the command */
				rd = rt;
				rt = R0;
				line = skipSpaces(line);
				if(line[0]!='\n' && line[0]!=EOF)
				{
					printf("[!] ERROR - FILE: \"%s\" - LINE: %d - extraneous text after end of command.\n", fileName, numLine);
					success = False;
				}
				break;
			}
			else
			{
				line = skipSpaces(line);
				if(line[0]!=',')
				{
					printf("[!] ERROR - FILE: \"%s\" - LINE: %d - missing comma.\n", fileName, numLine);
					success = False;
					break;
				}
				line++;
			}
		}
		else
		{
			line = skipSpaces(line);
			if(line[0]!='\n' && line[0]!=EOF)
			{
				printf("[!] ERROR - FILE: \"%s\" - LINE: %d - extraneous text after end of command.\n", fileName, numLine);
				success = False;
				break;
			}
		}
		line = skipSpaces(line);
	}
	
	if(success)
	{
		success = addOpRMachineCode(node_code, rs, rt, rd, element, fileName, numLine);
	}
	
	return success;
}

boolean handleILine(char *line, char *fileName, unsigned int numLine, machine_code **node_code, lookup_element element, int IC)
{
	boolean success; /* Notes whether the process succeeded */
	boolean hadImmed; /* Notes whether the "immed" was defined yet */
	reg rs, rt; /* The registers which will be called */
	long immed; /* the "immed" number */
	long tempNum; /* temporary number */
	
	/* initialization */
	success = True;
	hadImmed = False;
	rs = NULL_REG;
	rt = NULL_REG;
	
	/* if the command isn't "bne", "beq", "blt" or "bgt" */
	if(!(element.opC>=BNE_OPC && element.opC<=BGT_OPC))
	{
		while(rt==NULL_REG)
		{
			line = skipSpaces(line);
			if(rs==NULL_REG) /* if rs is undefined */
			{
				rs = getReg(line, fileName, numLine);
				if(rs==NULL_REG)
				{
					success = False;
					break;
				}
				if(rs<10) { line +=2; } /* the register takes two characters */
				else { line += 3; } /* the register takes three characters */
			}
			else if(hadImmed==False) /* if immed is undefined */
			{
				immed = atol(line);
				if(immed==0 && line[0]!='0')
				{
					printf("[!] ERROR - FILE: \"%s\" - LINE: %d - could not convert ", fileName, numLine);
					while(!(isspace(*line))&& *line!='\n' && *line!=EOF)
					{
						printf("%c", *line);
						line++;
					}
					printf(" into a number.\n");
					success = False;
					break;
				}
				if(immed<DH_MIN || immed >DH_MAX)
				{
					printf("[!] ERROR - FILE: \"%s\" - LINE: %d - \"immed\" value must be in the range [%d, %d].\n",
					       fileName, numLine, DH_MIN, DH_MAX);
					       success = False;
					       break;
				}
				tempNum = immed;
				if(tempNum < 0) { line++; } /* skipping the minus sign */
				do
				{
					line++;
					tempNum /= 10;
				} while(tempNum!=0);
				hadImmed = True;
			}
			else if(rt==NULL_REG) /* if rt is undefined */
			{
				rt = getReg(line, fileName, numLine);
				if(rt==NULL_REG)
				{
					success = False;
					break;
				}
				if(rt<10) { line +=2; } /* the register takes two characters */
				else { line += 3; } /* the register takes three characters */
			}
			line = skipSpaces(line);
			if(line[0]!=',' && rt==NULL_REG)
			{
				printf("[!] ERROR - FILE: \"%s\" - LINE: %d - missing comma.\n", fileName, numLine);
				success = False;
				break;
			}
			line++;
		}
	}
	else
	{
		while(rt==NULL_REG)
		{
			line = skipSpaces(line);
			if(rs==NULL_REG)
			{
				rs = getReg(line, fileName, numLine);
				if(rs==NULL_REG)
				{
					success = False;
					break;
				}
				if(rs<10) { line += 2; } /* the register takes two characters */
				else { line += 3; } /* the register takes three characters */
			}
			else
			{
				rt = getReg(line, fileName, numLine);
				if(rt==NULL_REG)
				{
					success = False;
					break;
				}
				if(rt<10) { line += 2; } /* the register takes two characters */
				else { line += 3; } /* the register takes three characters */
			}
			skipSpaces(line);
			if(line[0]!=',')
			{
				printf("[!] ERROR - FILE: \"%s\" - LINE: %d - missing comma.\n", fileName, numLine);
				success = False;
				break;
			}
			line++;
			line = skipSpaces(line);
		}
		if(success)
		{
			while(!(isspace(line[0]))) { line++; } /* the label will be handled in the second process */
			line = skipSpaces(line);
			if(line[0]!='\n' && line[0]!=EOF)
			{
				printf("[!] ERROR - FILE: \"%s\" - LINE: %d - extraneous text after end of command.\n", fileName, numLine);
				success = False;
			}
			else
			{
				immed = (long)IC;
			}
		}
	}
	if(success)
	{
		success = addOpIMachineCode(node_code, rs, rt, immed, element, fileName, numLine);
	}
	
	return success;
}

boolean handleJLine(char *line, char *fileName, unsigned int numLine, machine_code **node_code, lookup_element element, int IC)
{
	boolean success; /* Notes whether the process succeeded */
	long address; /* The address which the operation references to */
	int r; /* the "reg" segment in the J-line encoding */
	
	/* initialization */
	success = True;
	address = 0;
	
	line = skipSpaces(line);
	/* if the line is empty (and it's not a "stop" line) */
	if((line[0]=='\n' || line[0]==EOF) && !strEqual(element.word, "stop"))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - missing parameter.\n", fileName, numLine);
		success = False;
	}
	else
	{
		if(strEqual(element.word, "jmp"))
		{
			/* if the "jmp" refers to a register */
			if(line[0]=='$')
			{
				address = (long)getReg(line, fileName, numLine);
				if(address!=NULL_REG)
				{
					line += 2;
					if(address>=10) { line++; } /* if the register's number is a 2 digit number */
					r = 1;
				}
				else { success = False; }
			}
			else
			{
				/* setting the address as IC, for the second processing */
				address = IC; 
				r = 0;
				/* skipping the label */
				while(!(isspace(line[0]))) { line++; } 
			}
		}
		else if(strEqual(element.word, "la") || strEqual(element.word, "call"))
		{
			/* setting the address as IC, for the second processing */
			address = IC;
			r = 0;
			/* skipping the label */
			while(!(isspace(line[0]))) { line++; }
		}
		else if(strEqual(element.word, "stop"))
		{
			address = 0;
			r = 0;
		}
	}
	if(success)
	{
		line = skipSpaces(line);
		if(line[0]!='\n' && line[0]!=EOF && line[0]!='\0')
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - extraneous text after end of command.\n", fileName, numLine);
			success = False;
		}
	}
	if(success)
	{
		/* encoding the line */
		success = addOpJMachineCode(node_code, r, address, element, fileName, numLine);
	}

	return success;
}

boolean handleDataLine(char *line, char *fileName, unsigned int numLine, int *IC, int *DC, symbol_table **node_symbol,
		       machine_code **node_code, lookup_element element, boolean isThereLabel)
{
	boolean success; /* Notes whether the process succeeded */
	int labelLength; /* Holds the label's length, if defined in the line */
	int i; /* An iterator */
	
	/* initialization */
	success = True;
	labelLength = 0;
	
	if(isThereLabel)
	{
		/* adding the label to the symbol-table */
		for(; !(isspace(line[labelLength])) && line[labelLength]!=':'; labelLength++) {}
		success = addSymbol(line, fileName, numLine, node_symbol, *DC, DATA, labelLength);
	}
	if(success)
	{
		/* The program handled the label well */
		line += labelLength+1;
		line = skipSpaces(line);
		if(element.data==DB)
		{
			line += strlen(".db");
			success = encodeDataLine(line, fileName, numLine, DC, node_code, DB);
		}
		else if(element.data==DH)
		{
			line += strlen(".dh");
			success = encodeDataLine(line, fileName, numLine, DC, node_code, DH);
		}
		else if(element.data==DW)
		{
			line += strlen(".dw");
			success = encodeDataLine(line, fileName, numLine, DC, node_code, DW);
		}
		else
		{
			/* handling ".asciz" line is different than the rest, so the function handles it by itself */
			line += strlen(".asciz");
			line = skipSpaces(line);
			if(line[0]!='"')
			{
				printf("[!] ERROR - FILE: %s - LINE: %d - \".asciz\" must be defined by opening apostrophes.\n", fileName, numLine);
				return False;
			}
			line++;
			/* checking the validation of the line */
			for(i = 0; line[i]!='"' && line[i]!='\n' && line[i]!=EOF; i++)
			{
				if(!(isprint(line[i])))
				{
					printf("ERROR - FILE %s - LINE: %d - characters of \".asciz\" must be printing characters.\n", fileName, numLine);
					return False;
				}
			}
			if(line[i]!='"')
			{
				printf("[!] ERROR - FILE: %s - LINE: %d - \".asciz\" must be defined by closing apostrophes.\n", fileName, numLine);
				return False;
			}
			/* encoding the characters */
			for(i = 0; line[i]!='"'; i++)
			{
				success = addDataMachineCode(node_code, ASCIZ ,(long)line[i], fileName, numLine, DC); 
			}
			/* adding the null-character to terminate the string */
			success = addDataMachineCode(node_code, ASCIZ ,0, fileName, numLine, DC);
			line += i+1;
			line = skipSpaces(line);
			if(line[0]!='\n' && line[0]!=EOF)
			{
				printf("[!] ERROR - FILE: \"%s\" - LINE: %d - extraneous text after end of command.\n", fileName, numLine);
				success = False;
			} 
		}
	}
	return success;
}

boolean encodeDataLine(char *line, char *fileName, unsigned int numLine, int *DC, machine_code **node_code, dataType type)
{
	boolean success; /* Notes whether the process succeeded */
	boolean needComma; /* Notes whether a comma is needed or not */
	int maxValue; /* The maximum value which "type" can get */
	int minValue; /* The minimum value which "type" can get */
	long currNum; /* The current number in "line" */
	char *copyLine; /* A copy of line */
	
	/* initialization */
	success = True;
	copyLine = line;
	needComma = False;
	if(type==DB) { maxValue = DB_MAX; minValue = DB_MIN; }
	else if(type==DH) { maxValue = DH_MAX; minValue = DH_MIN; }
	else if(type==DW) { maxValue = DW_MAX; minValue = DW_MIN; }
	
	copyLine = skipSpaces(copyLine);
	if(*copyLine!='-' && *copyLine!='+' && !(isdigit(*copyLine)))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - extraneous text before start of command.\n",fileName, numLine);
		success = False;
	}
	/* checking that the text is valid */
	while(*copyLine!='\n' && *copyLine!=EOF && success)
	{
		copyLine = skipSpaces(copyLine);
		if(needComma && *copyLine!=',')
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - a comma is needed between arguments.\n", fileName, numLine);
			success = False;
			continue;
		}
		else if(!(needComma) && *copyLine==',')
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - multiple consecutive commas.\n", fileName, numLine);
			success = False;
			continue;
		}
		else if(needComma && *copyLine==',')
		{
			copyLine++;
			needComma = False;
			copyLine = skipSpaces(copyLine);
			continue;
		}
		else if(!(isdigit(*copyLine)) && *copyLine!='-' && *copyLine!='+')
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - character \"%c\" isn't a valid character.\n", fileName, numLine, *copyLine);
			success = False;
			continue;
		}
		currNum = atol(copyLine);
		if(!currNum && *copyLine!='0')
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - could not convert ", fileName, numLine);
			while(!(isspace(*copyLine))&&*copyLine!='\n' && *copyLine!=EOF)
			{
				printf("%c", *copyLine);
				copyLine++;
			}
			printf(" into a number.\n");
			success = False;
			continue;
		}
		if(!(currNum<=maxValue && currNum>=minValue))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - the number %ld should be in the range [%d, %d].\n", fileName, numLine, 
															 currNum, maxValue, minValue);
			success = False;
			continue;
		}
		/* skipping over the number's characters */
		if(currNum<0) { copyLine++; }
		do { copyLine++; currNum/=10; } while(currNum!=0);
		needComma = True;
		copyLine = skipSpaces(copyLine);
	}
		
	if(!(needComma) && success)
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - extraneous comma after end of command.\n", fileName, numLine);
		success = False;
	}
	
	copyLine = line;
	/* if the text is valid, encoding the numbers */
	while(*copyLine!='\n' && *copyLine!=EOF && success)
	{
		copyLine = skipSpaces(copyLine);
		currNum = atol(copyLine);
		if(currNum || *copyLine=='0')
		{
			success = addDataMachineCode(node_code, type, currNum, fileName, numLine, DC);
		}
		if(currNum<0) { copyLine++; }
		do { copyLine++; currNum/=10; } while(currNum!=0);
		copyLine = skipSpaces(copyLine);
		if(*copyLine==',')
		{
			copyLine++; /* skipping the comma */
		}
	}
	
	return success;	
}

boolean handleExtEntLine(char *line, char *fileName, unsigned int numLine, int *IC, int *DC, symbol_table **node_symbol, machine_code **node_code)
{
	int symbolLength; /* The length of the new symbol */
	int i; /* An iterator */
	boolean success; /* Notes if the process succeeded */
	
	/* If it's an ".entry" line, it will be handled in the second process */
	if(strEqual(line, ".entry")) 
	{
		/* checking if the line is grammatically correct */
		line += strlen(".entry");
		line = skipSpaces(line);
		while(!isspace(line[0])) { line++; }
		line = skipSpaces(line);
		if(line[0]!='\n' && line[0]!=EOF)
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - extraneous text after end of command", fileName, numLine);
			return False;
		}
		return True; 
	}
	/* It must be an ".extern" line */
	line += strlen(".extern");

	line = skipSpaces(line);
	if(line[0]=='\n' || line[0]==EOF)
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - missing parameter.\n", fileName, numLine);
	}
	symbolLength = isLabelValid(line, fileName, numLine, True);
	if(!symbolLength) { return False; } 
	for(i = 0; LOOKUP_TABLE[i].word!=NULL; i++)
	{
		if(strEqual(line, LOOKUP_TABLE[i].word))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - labels can't be saved words (%s).\n", fileName, numLine, LOOKUP_TABLE[i].word);
			return False;
		}
	}
	i = 0;
	while(!(isspace(line[i]))) { i++; }
	for(i = i; line[i]!='\n' && line[i]!=EOF; i++)
	{
		if(!(isspace(line[i])))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - extraneous operand (%s).\n", fileName, numLine, line+i);
			return False; 
		}
	}
	/* If the line is valid, add the symbol */
	success = addSymbol(line, fileName, numLine, node_symbol, 0, EXTERNAL, symbolLength);
	return success;
	
}

int isLabelValid(char *line, char *fileName, unsigned int numLine, boolean isExtEnt)
{
	int labelLength; /* Holds the length of the label */
	
	if(!(isalpha(line[0])))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - labels must begin with a letter.\n", fileName, numLine);
		return 0;
	}
	
	/* getting the label's length */
	for(labelLength = 1; (isalpha(line[labelLength]) || isdigit(line[labelLength])) &&
			      labelLength<MAX_LABEL_LENGTH && labelLength<strlen(line); labelLength++) {}
	/* if the label has invalid characters */
	if(!(isalpha(line[labelLength]) || isdigit(line[labelLength])))
	{
		if(line[labelLength]!=':' && !(isExtEnt) && !(isspace(line[labelLength])))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - labels must contain only letters or digits.\n", fileName, numLine);
			labelLength = 0;
		}
	}
	/* if the label is longer than allowed */
	else if(labelLength==MAX_LABEL_LENGTH && (isalpha(line[labelLength]) || isdigit(line[labelLength])))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - labels maximum length is 31-characters.\n", fileName, numLine);
		labelLength = 0;
	}
	/* if the label is "extern" or "entry", it is not allowed */
	if(strEqual(line, "extern"))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - labels names can't be saved words (%s).\n", fileName, numLine, "extern");
		labelLength = 0;
	}
	if(strEqual(line, "entry"))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - labels names can't be saved words (%s).\n", fileName, numLine, "entry");
		labelLength = 0;
	}
	return labelLength;
}

reg getReg(char *line, char *fileName, unsigned int numLine)
{
	reg result; /* The register which will be returned */
	int regNum; /* The number of the register */
	
	result = NULL_REG;
	if(line[0]!='$')
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - registers' references must begin with a '$'.\n", fileName, numLine);
	}
	else
	{
		line++;
		regNum = atoi(line);
		if(!(regNum>=0 && regNum<=31))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - invalid register number (%d).\n", fileName, numLine, regNum);
		}
		if(regNum==0 && line[0]!='0')
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - invalid register number (%c).\n", fileName, numLine, line[0]);
		}
		/* getting the correct register */
		else if(regNum==0) { result = R0; }
		else if(regNum==1) { result = R1; }
		else if(regNum==2) { result = R2; }
		else if(regNum==3) { result = R3; }
		else if(regNum==4) { result = R4; }
		else if(regNum==5) { result = R5; }
		else if(regNum==6) { result = R6; }
		else if(regNum==7) { result = R7; }
		else if(regNum==8) { result = R8; }
		else if(regNum==9) { result = R9; }
		else if(regNum==10) { result = R10; }
		else if(regNum==11) { result = R11; }
		else if(regNum==12) { result = R12; }
		else if(regNum==13) { result = R13; }
		else if(regNum==14) { result = R14; }
		else if(regNum==15) { result = R15; }
		else if(regNum==16) { result = R16; }
		else if(regNum==17) { result = R17; }
		else if(regNum==18) { result = R18; }
		else if(regNum==19) { result = R19; }
		else if(regNum==20) { result = R20; }
		else if(regNum==21) { result = R21; }
		else if(regNum==22) { result = R22; }
		else if(regNum==23) { result = R23; }
		else if(regNum==24) { result = R24; }
		else if(regNum==25) { result = R25; }
		else if(regNum==26) { result = R26; }
		else if(regNum==27) { result = R27; }
		else if(regNum==28) { result = R28; }
		else if(regNum==29) { result = R29; }
		else if(regNum==30) { result = R30; }
		else if(regNum==31) { result = R31; }
	}
	return result;
}










