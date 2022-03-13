#include "writeFiles.h"

/*
 * FILE: writeFiles.c
 * GOAL: writes the output files of the assembler:
 	 - *.ob file.
 	 - *.ext file (if needed).
 	 - *.ent file (if needed).
 * ASSUMPTIONS: see the functions' headers.
*/

void writeObFile(char *fileName, machine_code *node, int ICF, int DCF)
{
	machine_code *tempNode; /* Temporary node of the machine_code */
	char *obFileName; /* The final file's name */
	FILE *obFile; /* A pointer to the object-file which will be written */
	int ICC; /* Instruction-counter-counter */
	
	
	/* initialization */
	ICC = INIT_IC_VALUE;
	tempNode = node;
	
	/* creating the object-file name */
	obFileName = calloc(strlen(fileName)+strlen(OBJECT_SUFFIX)+1, sizeof(char));
	if(!obFileName)
	{
		printf("[!] ERROR - FILE: \"%s\" - ran out of memory.\n", fileName);
		return;
	}
	strcpy(obFileName, fileName);
	strcat(obFileName, OBJECT_SUFFIX);
	
	/* creating the object file */
	obFile = fopen(obFileName, "w");
	if(!obFile)
	{
		printf("[!] ERROR - FILE: \"%s\" - could not write or replace the file \"%s\".\n", fileName, obFileName);
		return;
	}
	
	/* writing the file */
	
	/* writing the ICF and the DCF */
	fprintf(obFile, "     %d %d", ICF-INIT_IC_VALUE, DCF);
	
	/* printing all of the operation-lines encodings */
	while((tempNode->next)!=NULL)
	{
		if(tempNode->isOp)
		{
			if(tempNode->mu.op->type == R)
			{
				writeRLine(obFile, tempNode->mu.op->opu.R, &ICC);
			}
			else if(tempNode->mu.op->type == I)
			{
				writeILine(obFile, tempNode->mu.op->opu.I, &ICC);
			}
			else if(tempNode->mu.op->type == J)
			{
				writeJLine(obFile, tempNode->mu.op->opu.J, &ICC);
			}
		}
		tempNode = tempNode->next;
	}
	
	/* printing all of the data-lines encodings */
	tempNode = node;
	while((tempNode->next)!=NULL)
	{
		if(!(tempNode->isOp))
		{
			if(tempNode->mu.data->type==DB || tempNode->mu.data->type==ASCIZ)
			{
				if(ICC%4==0)
				{
					fprintf(obFile, "\n%04d ", ICC);
				}
				writeByte(obFile, tempNode->mu.data->du.db->data, 1);
				ICC += DB_BYTES;
			}
			else if(tempNode->mu.data->type==DH)
			{
				/* writing the first half the .dh */
				if(ICC%4==0)
				{
					fprintf(obFile, "\n%04d ", ICC);
				}
				writeByte(obFile, tempNode->mu.data->du.dh->data, 1);
				ICC += DB_BYTES;
				
				/* writing the second half the .dh */
				if(ICC%4==0)
				{
					fprintf(obFile, "\n%04d ", ICC);
				}
				writeByte(obFile, tempNode->mu.data->du.dh->data, 2);
				ICC += DB_BYTES;
			}
			else if(tempNode->mu.data->type==DW)
			{
				/* writing the first quarter of the .dw */
				if(ICC%4==0)
				{
					fprintf(obFile, "\n%04d ", ICC);
				}
				writeByte(obFile, tempNode->mu.data->du.dw->data, 1);
				ICC += DB_BYTES;
				
				/* writing the first second of the .dw */
				if(ICC%4==0)
				{
					fprintf(obFile, "\n%04d ", ICC);
				}
				writeByte(obFile, tempNode->mu.data->du.dw->data, 2);
				ICC += DB_BYTES;
				
				/* writing the first third of the .dw */
				if(ICC%4==0)
				{
					fprintf(obFile, "\n%04d ", ICC);
				}
				writeByte(obFile, tempNode->mu.data->du.dw->data, 3);
				ICC += DB_BYTES;
				
				/* writing the first fourth of the .dw */
				if(ICC%4==0)
				{
					fprintf(obFile, "\n%04d ", ICC);
				}
				writeByte(obFile, tempNode->mu.data->du.dw->data, 4);
				ICC += DB_BYTES;
				
			}
		
		}
		tempNode = tempNode->next;
	}
	
	return;
}

void writeEntFile(char *fileName, symbol_table *node)
{
	char *entFileName; /* The final file's name */
	FILE *entFile; /* A pointer to the entry-file which will be written */
	boolean hadEntry; /* Notes whether there was an entry-symbol */
	
	hadEntry = False;
	
	/* writing the file */
	
	while((node->next)!=NULL)
	{
		if((node->attribute)==CODE_ENTRY || (node->attribute)==DATA_ENTRY)
		{
			if(!hadEntry)
			{
				/* a file is needed to be created */
				entFileName = calloc(strlen(fileName)+strlen(ENTRY_SUFFIX)+1, sizeof(char));
				if(!entFileName)
				{
					printf("[!] ERROR - FILE: \"%s\" - ran out of memory.\n", fileName);
					return;
				}
				strcpy(entFileName, fileName);
				strcat(entFileName, ENTRY_SUFFIX);
	
				/* creating the entry file */
				entFile = fopen(entFileName, "w");
				if(!entFile)
				{
					printf("[!] ERROR - FILE: \"%s\" - could not write or replace the file \"%s\".\n", fileName, entFileName);
					return;
				}
				hadEntry = True;
			}
			/* printing the information */
			fprintf(entFile, "%s %04d\n", node->symbol, node->value);
		}
		node = node->next;
	}
	
	return;
}

void writeExtFile(char *fileName, extern_sym *node)
{
	char *extFileName; /* The final file's name */
	FILE *extFile; /* A pointer to the extern-file which will be written */
		
	if(node->address==0)
	{
		/* it means that no symbol was added to the list */
		return;
	}
	/* a file is needed to be created */
	extFileName = calloc(strlen(fileName)+strlen(ENTRY_SUFFIX)+1, sizeof(char));
	if(!extFileName)
	{
		printf("[!] ERROR - FILE: \"%s\" - ran out of memory.\n", fileName);
		return;
	}
	strcpy(extFileName, fileName);
	strcat(extFileName, EXTERN_SUFFIX);
	/* creating the entry file */
	extFile = fopen(extFileName, "w+");
	if(!extFile)
	{
		printf("[!] ERROR - FILE: \"%s\" - could not write or replace the file \"%s\".\n", fileName, extFileName);
		return;
	}
	/* writing the file */
	while((node->next)!=NULL)
	{
		fprintf(extFile, "%s %04ld\n", node->name, node->address);
		node = node->next;
	}
	return;
}

void writeRLine(FILE *file, r_op *line, int *ICC)
{
	long fullLine; /* Holds "line" as 32 bits */
	
	fullLine = 0;
	
	/* if the program had printed a full word */
	if(*ICC%4==0)
	{
		fprintf(file, "\n%04d ", *ICC);
	}
	
	/* putting all of the 32-bits together */
	fullLine |= line->opcode;
	fullLine <<= R_RS_BITS;
	fullLine |= line->rs;
	fullLine <<= R_RT_BITS;
	fullLine |= line->rt;
	fullLine <<= R_RD_BITS;
	fullLine |= line->rd;
	fullLine <<= R_FUNCT_BITS;
	fullLine |= line->funct;
	fullLine <<= R_NOT_USED_BITS;
	
	writeWord(file, fullLine);
	*ICC += WORD_BYTES_FACTOR;
}

void writeILine(FILE *file, i_op *line, int *ICC)
{
	long fullLine; /* Holds "line" as 32 bits */
	
	fullLine = 0;
	
	/* if the program had printed a full word */
	if(*ICC%4==0)
	{
		fprintf(file, "\n%04d ", *ICC);
	}
	/* putting all of the 32-bits together */
	fullLine |= line->opcode;
	fullLine <<= I_RS_BITS;
	fullLine |= line->rs;
	fullLine <<= I_RT_BITS;
	fullLine |= line->rt;
	fullLine <<= I_IMMED_BITS;
	fullLine |= line->immed;
	
	writeWord(file, fullLine);
	*ICC += WORD_BYTES_FACTOR;
}

void writeJLine(FILE *file, j_op *line, int *ICC)
{
	long fullLine; /* Holds "line" as 32 bits */
	
	fullLine = 0;
	
	/* if the program had printed a full word */
	if(*ICC%4==0)
	{
		fprintf(file, "\n%04d ", *ICC);
	}
	/* putting all of the 32-bits together */
	fullLine |= line->opcode;
	fullLine <<= J_REG_BITS;
	fullLine |= line->reg;
	fullLine <<= J_ADDRESS_BITS;
	fullLine |= line->address;
	
	writeWord(file, fullLine);
	*ICC += WORD_BYTES_FACTOR;
}

void writeWord(FILE *file, long line)
{
	writeByte(file, line, 1);
	writeByte(file, line, 2);
	writeByte(file, line, 3);
	writeByte(file, line, 4);
	
}

void writeByte(FILE *file, long data, int quarter)
{
	if(quarter==1)
	{
		/* writing the first byte */
		data = KEEP_FIRST_BYTE(data);
		fprintf(file, "%02X ", (unsigned)data);
	}
	else if(quarter==2)
	{
		/* writing the second byte */
		data = KEEP_SECOND_BYTE(data);
		data >>= BYTE_BITS_FACTOR;
		fprintf(file, "%02X ", (unsigned)data);
	}
	else if(quarter==3)
	{
		/* writing the third byte */
		data = KEEP_THIRD_BYTE(data);
		data >>= 2*BYTE_BITS_FACTOR;
		fprintf(file, "%02X ", (unsigned)data);
	}
	else if(quarter==4)
	{
		/* writing the fourth byte */
		data = KEEP_LAST_BYTE(data);
		data >>= 3*BYTE_BITS_FACTOR;
		data = KEEP_FIRST_BYTE(data); /* prevents incorrect writing because shift-right keeps the sign of data */
		fprintf(file, "%02X ", (unsigned)data);
	}
}




