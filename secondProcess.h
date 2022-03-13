#ifndef SECOND_PROCESS_H
#define SECOND_PROCESS_H

#include <stdio.h>
#include <string.h>
#include "handleFiles.h"



/*
 * Handles the second processing of the file.
 * INPUT - "file": the pointer to the file.
 	   "fileName": the name of the file.
 	   "node_symbol": the current node of the symbol_table.
 	   "node_code": the current node of the machinde-code. 
 	   "node_ext": the current node of the external-symbol-table.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean second_processing(FILE *file, char *fileName, symbol_table *node_symbol, machine_code *node_code, extern_sym *node_ext);


/*
 * Handles an ".entry" line in the second process.
 * INPUT - "line": the referenced line.
 	   "fileName": the file's name.
 	   "numLine": the current line's number.
 	   "node_symbol": the current node of the symbol-table.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean handleEntrySP(char *line, char *fileName, unsigned int numLine, symbol_table **node);


/*
 * Handles an I-type line in the second process.
 * INPUT - "line": the referenced line.
 	   "fileName": the file's name.
 	   "numLine": the current line's number.
 	   "node_symbol": the current node of the symbol-table.
 	   "node_code": the current node of the machine-code.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean handleISP(char *line, char *fileName, unsigned int numLine, symbol_table **node_symbol, machine_code **node_code);


/*
 * Handles an J-type line in the second process.
 * INPUT - "line": the referenced line.
 	   "fileName": the file's name.
 	   "numLine": the current line's number.
 	   "node_symbol": the current node of the symbol-table.
 	   "node_code": the current node of the machine-code.
 	   "node_ext": the current node of the external-symbol-table.
 * OUTPUT - True: if the process succeeded.
 	    False: otherwise.
*/
boolean handleJSP(char *line, char *fileName, unsigned int numLine, symbol_table **node_symbol, machine_code **node_code, extern_sym **node_ext);





#endif
