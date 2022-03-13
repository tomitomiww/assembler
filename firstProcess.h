#ifndef FIRST_PROCESS_H
#define FIRST_PROCESS_H

#include <stdio.h>
#include <string.h>
#include "handleFiles.h"



/*
 * Handles the first processing of the file.
 * INPUT - "file": the pointer to the file.
 	   "fileName": the name of the file.
 	   "IC": The instruction counter.
 	   "DC": The data counter.
 	   "node_symbol": the current node of the symbol_table.
 	   "node_code": the current node of the machine-code. 
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean first_processing(FILE *file, char *fileName, int *IC, int *DC, symbol_table *node_symbol, machine_code *node_code);


/*
 * Checks if a label is defined in an ".extern" or an ".entry" line.
 * INPUT - "line": the referenced line.
 	   "fileName": the file's name.
 	   "numLine": the current line's number.
 * OUTPUT - True: if "line" is ".extern" or ".entry" type.
  	    False: otherwise.
*/
boolean isExtEnt(char *line, char *fileName, unsigned int numLine);


/*
 * gets the element and its information by the file's line.
 * INPUT - "line": the referenced line.
 	   "fileName": the file's name.
 	   "numLine": the current line's number.
 * OUTPUT - the correct element, or a NULL-element if there's a syntex error.
*/
lookup_element getElement(char *line, char *fileName, unsigned int numLine);


/*
 * Handles operation (add, move, ls, call, etc) lines according to the assembler orders.
  * INPUT - "line": the referenced line.
 	    "filaName": the file's name.
 	    "numLine": the current line's number.
 	    "IC": the instruction counter.
 	    "DC": the data counter.
 	    "node_symbol": the current node of the symbol_table.
 	    "node_code": the current node of the machine-code.
 	    "element": the element which holds the information about the machine-code.
 	    "isThereLabel": notes if a label is defined in the line.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean handleOpLine(char *line, char *fileName,unsigned int numLine, int  *IC, int *DC, symbol_table **node_symbol,
		     machine_code **node_code, lookup_element element, boolean isThereLabel);
		     
		     
/*
 * Handles operation lines of type R.
 * INPUT - "line": the referenced line.
 	   "filaName": the file's name.
 	   "numLine": the current line's number.
 	   "node_code": the current node of the machine-code.
 	   "element": the element which holds the information about the machine-code.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean handleRLine(char *line, char *fileName, unsigned int numLine, machine_code **node_code, lookup_element element);


/*
 * Handles operation lines of type I.
 * INPUT - "line": the referenced line.
 	   "filaName": the file's name.
 	   "numLine": the current line's number.
 	   "node_code": the current node of the machine-code.
 	   "element": the element which holds the information about the machine-code.
 	   "IC": the current instruction counter, which will be used if the line can't be encoded.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean handleILine(char *line, char *fileName, unsigned int numLine, machine_code **node_code, lookup_element element, int IC);


/*
 * Handles operation lines of type J.
 * INPUT - "line": the referenced line.
 	   "filaName": the file's name.
 	   "numLine": the current line's number.
 	   "node_code": the current node of the machine-code.
 	   "element": the element which holds the information about the machine-code.
 	   "IC": the current instruction counter, which will be used if the line can't be encoded.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean handleJLine(char *line, char *fileName, unsigned int numLine, machine_code **node_code, lookup_element element, int IC);


/*
 * Handles data (.db, .dh, etc) lines according to the assembler orders.
  * INPUT - "line": the referenced line.
 	   "filaName": the file's name.
 	   "numLine": the current line's number.
 	   "IC": the instruction counter.
 	   "DC": the data counter.
 	   "node_symbol": the current node of the symbol_table.
 	   "node_code": the current node of the machine-code.
 	   "element": the element which holds the information about the machine-code.
 	   "isThereLabel": notes if a label is defined in the line.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean handleDataLine(char *line, char *fileName,unsigned int numLine, int  *IC, int *DC, symbol_table **node_symbol,
		       machine_code **node_code, lookup_element element, boolean isThereLabel);
		       
		       
/*
 * Encodes a data-line, according to its type(.db, .dh, .dw).
 * INPUT - "line": the referenced line.
 	   "filaName": the file's name.
 	   "numLine": the current line's number.
 	   "DC": the data counter.
 	   "node_code": the current node of the machine-code.
 	   "type": the type of the data.
  * OUTPUT - True: if the process succeeded.
 	     False: otherwise.
*/
boolean encodeDataLine(char *line, char *fileName, unsigned int numLine, int *DC, machine_code **node_code, dataType type);


/*
 * Handles an ".extern" or an ".entry" line.
 * INPUT - "line": the referenced line.
 	   "filaName": the file's name.
 	   "numLine": the current line's number.
 	   "IC": the instruction counter.
 	   "DC": the data counter.
 	   "node_symbol": the current node of the symbol_table.
 	   "node_code": the current node of the machine-code.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean handleExtEntLine(char *line, char *fileName, unsigned int numLine, int *IC, int *DC, symbol_table **node_symbol, machine_code **node_code); 


/*
 * Checks if a label in a line is valid (not longer than 31 characters, only numbers and letters).
 * INPUT - "line": the referenced line.
           "filaName": the file's name.
 	   "numLine": the current line's number.
 	   "isExtEnt": notes if the line is ".extern" or ".entry" line.
 * OUTPUT - the length of the label, including the colon character.
 	    If the label isn't valid, returns 0.
*/
int isLabelValid(char *line, char *fileName, unsigned int numLine, boolean isExtEnt);


/*
 * Returns the register which is referenced in the line, or NULL_REG if an error occurred.
 * INPUT - "line": the referenced line.
           "filaName": the file's name.
 	   "numLine": the current line's number.
 * OUTPUT - the referenced register.
 	    If an error occurred, returns NULL_REG (0).
*/
reg getReg(char *line, char *fileName, unsigned int numLine);

#endif
