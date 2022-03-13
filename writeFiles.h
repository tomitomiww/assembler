#ifndef WRITE_FILES_H
#define WRITE_FILES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataStructures.h"
#include "handleFiles.h"

#define OBJECT_SUFFIX ".ob"
#define ENTRY_SUFFIX ".ent"
#define EXTERN_SUFFIX ".ext"

#define R_OPC_BITS 6
#define R_RS_BITS 5
#define R_RT_BITS 5
#define R_RD_BITS 5
#define R_FUNCT_BITS 5
#define R_NOT_USED_BITS 6

#define I_OPC_BITS 6
#define I_RS_BITS 5
#define I_RT_BITS 5
#define I_IMMED_BITS 16

#define J_OPC_BITS 6
#define J_REG_BITS 1
#define J_ADDRESS_BITS 25

#define BYTE_BITS_FACTOR 8
#define WORD_BYTES_FACTOR 4

#define KEEP_FIRST_BYTE(line) line & 0x000000ff
#define KEEP_SECOND_BYTE(line) line & 0x0000ff00
#define KEEP_THIRD_BYTE(line) line & 0x00ff0000
#define KEEP_LAST_BYTE(line) line & 0xff000000

/*
 * Writes the .object file of the assembled file.
 * INPUT - "fileName": the referenced file's name.
 	   "node": the current node of the machine-code linked-list.
 	   "ICF": the final instruction counter.
 	   "DCF": the final data counter.
 * OUTPUT - none.
*/
void writeObFile(char *fileName, machine_code *node, int ICF, int DCF);


/* Writes the .ent file of the assembled file, if needed.
 * INPUT - "fileName": the referenced file's name.
 	   "node": the current node of the symbol-table linked-list.
 * OUTPUT - none.
*/
void writeEntFile(char *fileName, symbol_table *node);


/*
 * Writes the .ext file of the assembled file, if needed.
 * INPUT - "fileName": the referenced file's name.
 	   "node": the current node of the external-symbols linked-list.
 * OUTPUT - none.
*/
void writeExtFile(char *fileName, extern_sym *node);


/*
 * Writes an encoded R-operation line.
 * INPUT - "file": the pointer to the file.
 	   "line": the encoded line.
 	   "ICC": the instruction-counter counter.
 * OUTPUT - none.
*/
void writeRLine(FILE *file, r_op *line, int *ICC);


/*
 * Writes an encoded I-operation line.
 * INPUT - "file": the pointer to the file.
 	   "line": the encoded line.
 	   "ICC": the instruction-counter counter.
 * OUTPUT - none.
*/
void writeILine(FILE *file, i_op *line, int *ICC);


/*
 * Writes an encoded J-operation line.
 * INPUT - "file": the pointer to the file.
 	   "line": the encoded line.
 	   "ICC": the instruction-counter counter.
 * OUTPUT - none.
*/
void writeJLine(FILE *file, j_op *line, int *ICC);



/*
 * Writes a full word (32 bits) line.
 * INPUT - "file": the pointer to the file.
 *	   "line": the referenced line.
 * OUTPUT - none.
*/
void writeWord(FILE *file, long line);


/*
 * Writes a byte from a given word.
 * INPUT - "file": the pointer to the file.
 	   "data": the given word.
 	   "quarter": notes which byte is need to be written.
 * OUTPUT - none.
*/
void writeByte(FILE *file, long data, int quarter);

#endif





