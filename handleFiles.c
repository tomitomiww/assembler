#include "handleFiles.h"

/*
 * FILE: handleFiles.c
 * GOAL: implements various functions which help handling the assembled files.
 * ASSUMPTIONS: see the functions' headers.
*/

char *skipSpaces(char *line)
{
	int i; /* An iterator */
	
	for(i = 0; isspace(line[i]) && line[i]!='\n' && line[i]!=EOF && i<strlen(line); i++){}
	return line+i;
}

boolean strEqual(char *s1, char *s2)
{
	int s1Length; /* the length of s1's first word (s1 could be more than one word) */
	int s2Length; /* the length of s2's first word (s2 could be more than one word) */
	
	/* inialization */
	s1Length = 0;
	s2Length = 0;
	
	/* getting s1Length and s2Length */
	while((isalpha(s1[s1Length]) || isdigit(s1[s1Length]) || s1[s1Length]=='.') && s1Length<strlen(s1)) { s1Length++; }
	while((isalpha(s2[s2Length]) || isdigit(s2[s2Length]) || s2[s2Length]=='.') && s2Length<strlen(s2)) { s2Length++; }
	
	/* if s1!=s2 */
	if(s1Length!=s2Length || (strncmp(s1, s2, s1Length)))
	{
		return False;
	}
	return True;
}

boolean addSymbol(char *symName, char *fileName, unsigned int numLine, symbol_table **node, int value, sym_attr attr, int length)
{
	int i; /* An iterator */
	symbol_table *next; /* The next node in the linked-list */
	
	/* Moving to the last node in the list */
	while(((*node)->next)!=NULL)
	{
		/* If the symbol was defined before */
		if(strEqual(symName, (*node)->symbol))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - the symbol \"", fileName, numLine);
			for(i = 0; !(isspace(symName[i])) && i<strlen(symName); i++) { printf("%c", symName[i]); }
			printf("\" already exists.\n");
			return False;
		}
		node = &((*node)->next);
	}
	
	(*node)->symbol = (char *)calloc(length+1, sizeof(char));
	if(!((*node)->symbol))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	/* copying the symbol to the node */
	for(i = 0; i<length; i++)
	{
		((*node)->symbol)[i] = symName[i];
	}
	((*node)->symbol)[i] = '\0';
	/* setting other fields */
	(*node)->value = value;
	(*node)->attribute = attr;
	
	/* setting the next node */
	next = (symbol_table *)calloc(1, sizeof(symbol_table));
	if(!(next))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	(*node)->next = next;
	return True;
}

boolean addOpRMachineCode(machine_code **node, reg rs, reg rt, reg rd, lookup_element element, char *fileName, unsigned int numLine)
{
	machine_code *next; /* The next node in the linked-list */
	
	/* moving to the last node in the list */
	while(((*node)->next)!=NULL)
	{
		node = &((*node)->next);
	}
	
	(*node)->numLine = numLine;
	/* this is an operation code */
	(*node)->isOp = True;
	(*node)->mu.op = (op_code *)calloc(1, sizeof(op_code));
	if(!((*node)->mu.op))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	/* This is an R-type machine code */
	(*node)->mu.op->type = R;
	(*node)->mu.op->opu.R = (r_op *)calloc(1, sizeof(r_op));
	if(!((*node)->mu.op->opu.R))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	(*node)->mu.op->opu.R->NOT_USED = 0;
	(*node)->mu.op->opu.R->funct = decToBin(element.func);
	(*node)->mu.op->opu.R->rd = decToBin(rd);
	(*node)->mu.op->opu.R->rt = decToBin(rt);
	(*node)->mu.op->opu.R->rs = decToBin(rs);
	(*node)->mu.op->opu.R->opcode = decToBin(element.opC);
	
	/* setting the next node */
	next = (machine_code *)calloc(1, sizeof(machine_code));
	if(!next)
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	(*node)->next = next;
	return True;
}

boolean addOpIMachineCode(machine_code **node, reg rs, reg rt, long immed, lookup_element element, char *fileName, unsigned int numLine)
{
	machine_code *next; /* The next node in the linked-list */
	
	/* moving to the last node in the list */
	while(((*node)->next)!=NULL)
	{
		node = &((*node)->next);
	}
	
	(*node)->numLine = numLine;
	/* this is an operation code */
	(*node)->isOp = True;
	(*node)->mu.op = (op_code *)calloc(1, sizeof(op_code));
	if(!((*node)->mu.op))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	/* This is an I-type machine code */
	(*node)->mu.op->type = I;
	(*node)->mu.op->opu.I = (i_op *)calloc(1, sizeof(i_op));
	if(!((*node)->mu.op->opu.I))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	/* setting the information */
	(*node)->mu.op->opu.I->opcode = decToBin(element.opC);
	(*node)->mu.op->opu.I->rs = decToBin(rs);
	(*node)->mu.op->opu.I->rt = decToBin(rt);
	(*node)->mu.op->opu.I->immed = decToBin(immed);
	
	/* setting the next node */
	next = (machine_code *)calloc(1, sizeof(machine_code));
	if(!next)
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	next->next = NULL;
	(*node)->next = next;
	return True;
}

boolean addOpJMachineCode(machine_code **node, int r, long address, lookup_element element, char *fileName, unsigned int numLine)
{
	machine_code *next; /* The next node in the linked-list */
	
	/* moving to the last node in the list */
	while(((*node)->next)!=NULL)
	{
		node = &((*node)->next);
	}
	
	(*node)->numLine = numLine;
	/* this is an operation code */
	(*node)->isOp = True;
	(*node)->mu.op = (op_code *)calloc(1, sizeof(op_code));
	if(!((*node)->mu.op))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	/* This is an J-type machine code */
	(*node)->mu.op->type = J;
	(*node)->mu.op->opu.J = (j_op *)calloc(1, sizeof(j_op));
	if(!((*node)->mu.op->opu.J))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	/* setting the information */
	(*node)->mu.op->opu.J->address = decToBin(address);
	(*node)->mu.op->opu.J->reg = decToBin(r);
	(*node)->mu.op->opu.J->opcode = decToBin(element.opC);
	
	/* setting the next node */
	next = (machine_code *)calloc(1, sizeof(machine_code));
	if(!next)
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	next->next = NULL;
	(*node)->next = next;
	return True;
}

boolean addDataMachineCode(machine_code **node, dataType type ,long data, char *fileName, unsigned int numLine, int *DC)
{

	machine_code *next; /* The next node in the linked-list */
	
	/* moving to the last node in the list */
	while(((*node)->next)!=NULL)
	{
		node = &((*node)->next);
	}
	
	(*node)->numLine = numLine;
	/* this is a data code, not an operation code */
	(*node)->isOp = False;
	(*node)->mu.data = (data_code *)calloc(1, sizeof(data_code));
	if(!((*node)->mu.data))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	(*node)->mu.data->type = type;
	/* handling each data-type in a similar way, according to "data" */
	if(type==DB || type==ASCIZ)
	{
		(*node)->mu.data->du.db = (db_code *)calloc(1, sizeof(db_code));
		if(!((*node)->mu.data->du.db))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
			return False;
		}
		(*node)->mu.data->du.db->data = 0;
		(*node)->mu.data->du.db->data = decToBin(data);
		if(type==DB) { *DC += DB_BYTES; }
		else { *DC += ASCIZ_BYTES; }
	}
	else if(type==DH)
	{
		(*node)->mu.data->du.dh = (dh_code *)calloc(1, sizeof(dh_code));
		if(!((*node)->mu.data->du.dh))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
			return False;
		}
		(*node)->mu.data->du.dh->data = 0;
		(*node)->mu.data->du.dh->data = decToBin(data);
		*DC += DH_BYTES;
	}
	else if(type==DW)
	{
		(*node)->mu.data->du.dw = (dw_code *)calloc(1, sizeof(dw_code));
		if(!((*node)->mu.data->du.dw))
		{
			printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
			return False;
		}
		(*node)->mu.data->du.dw->data = 0;
		(*node)->mu.data->du.dw->data = decToBin(data);
		*DC += DW_BYTES;
	}
	
	/* setting the next node */
	next = (machine_code *)calloc(1, sizeof(machine_code));
	if(!next)
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	next->next = NULL;
	(*node)->next = next;
	return True;
}

boolean addExtSymNode(extern_sym **node, long address, char *newName, char *fileName, unsigned int numLine)
{
	int i; /* An iterator */
	extern_sym *next; /* The next node in the linked-list */
	
	/* Moving to the last node in the list */
	while(((*node)->next)!=NULL)
	{
		node = &((*node)->next);
	}
	
	(*node)->name = (char *)calloc(strlen(newName)+1, sizeof(char));
	if(!((*node)->name))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	/* copying the symbol to the node */
	for(i = 0; i<strlen(newName)+1; i++)
	{
		((*node)->name)[i] = newName[i];
	}
	((*node)->name)[i] = '\0';
	/* setting other fields */
	(*node)->address = address;
	
	/* setting the next node */
	next = (extern_sym *)calloc(1, sizeof(extern_sym));
	if(!(next))
	{
		printf("[!] ERROR - FILE: \"%s\" - LINE: %d - ran out of memory.\n", fileName, numLine);
		return False;
	}
	(*node)->next = next;
	return True;
}

unsigned int decToBin(long num)
{
	long bitCounter; /* Counts the current bit the function access */
	unsigned int binary; /* "num" in its binary representation */
	boolean isNegative; /* Notes whether "num" is a negative number */
	
	if(num<0)
	{
		isNegative = True;
		num = -num;
	}
	else { isNegative = False; }
	
	binary = 0;
	/* setting the bits */
	for(bitCounter = 0; num!=0; bitCounter++)
	{
		binary |= (num%2)<<bitCounter;
		num /= 2;
	}
	/* If "num" is negative, the function finds its binary representation as a 2's complement negative number */
	if(isNegative)
	{
		binary = ~binary;
		binary += 1;
	}
	return binary;
}

void freeCodeList(machine_code *node)
{
	machine_code *tempNode; /* Temporary node */
	
	while(node!=NULL)
	{
		if((node->next)==NULL)
		{
			free(node);
			return;
		}
		/* "saving" the current node */
		tempNode = node;
		/* moving to the next node */
		node = node->next;
		/* if this is an operation line, check it type and free it accordingly */
		if(tempNode->isOp)
		{
			if(tempNode->mu.op->type==R)
			{
				free(tempNode->mu.op->opu.R);
			}
			else if(tempNode->mu.op->type==I)
			{
				free(tempNode->mu.op->opu.I);
			}
			else if(tempNode->mu.op->type==J)
			{
				free(tempNode->mu.op->opu.J);
			}
			free(tempNode->mu.op);
		}
		/* if this is not an operation line, check it type and free it accordingly */	
		else		
		{
			if(tempNode->mu.data->type==DB)
			{
				free(tempNode->mu.data->du.db);
			}
			if(tempNode->mu.data->type==DH)
			{
				free(tempNode->mu.data->du.dh);
			}
			if(tempNode->mu.data->type==DW)
			{
				free(tempNode->mu.data->du.dw);
			}
			free(tempNode->mu.data);
		}
		free(tempNode);
	}
}

void freeSymbolList(symbol_table *node)
{
	symbol_table *tempNode; /* Temporary node */
	while(node!=NULL)
	{
		if((node->next)==NULL)
		{
			free(node);
			return;
		}
		/* "saving" the current node */
		tempNode = node;
		/* moving to the next node */
		node = node->next;
		free(tempNode->symbol);
		free(tempNode);
	}
}


void freeExternList(extern_sym *node)
{
	extern_sym *tempNode; /* Temporary node */
	while(node!=NULL)
	{
		if((node->next)==NULL)
		{
			free(node);
			return;
		}
		/* "saving" the current node */
		tempNode = node;
		/* moving to the next node */
		node = node->next;
		free(tempNode->name);
		free(tempNode);
	}
}
