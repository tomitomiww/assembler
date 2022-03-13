#ifndef HANDLE_FILES_H
#define HANDLE_FILES_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "dataStructures.h"

#define INIT_IC_VALUE 100

#define MAX_LABEL_LENGTH 31
#define MAX_LINE_LENGTH 80

#define DB_MAX 127
#define DB_MIN -128
#define DH_MAX 32767
#define DH_MIN -32768
#define DW_MAX 2147483647
#define DW_MIN -2147483648
#define DB_BYTES 1
#define DH_BYTES 2
#define DW_BYTES 4
#define ASCIZ_BYTES 1



/*
 * Skip spaces in a line, until the next non-space character.
 * INPUT - "line": the referenced line.
 * OUTPUT - a pointer to "line" after the skipping. 
*/
char *skipSpaces(char *line);


/*
 * Comparing two strings.
 * INPUT - "s1": the first string.
 	   "s2": the second string.
 * OUTPUT - "True": if the strings are equal.
 	    "False": otherwise.
*/
boolean strEqual(char *s1, char *s2);


/*
 * Creates a new node in the symbol-table linked-list, according to the information (symbol's name, value, attribute).
 * The function also creates the next node in the linked-list and links the current node to the next one.
 * INPUT - "symName": the symbol's name.
 	   "fileName": the current file.
 	   "numLine": the current line.
 	   "node": the current node.
 	   "value": the node's value.
 	   "attr": the node's attribute.
 	   "length": the length of the symbol.
 * OUTPUT - "True": if the process succeeded.
 	    "False": otherwise.
*/
boolean addSymbol(char *symName, char *fileName, unsigned int numLine, symbol_table **node, int value, sym_attr attr, int length);


/*
 * Creates a new node in the machine code linked-list, according to the information.
 * The function also creates the next node in the linked-list and links the current node to the next one.
 * The function is specified for R-type operation-lines ("add", "sub", "move", etc),
 * INPUT - "node": the current node.
 	   "rs": the rs register.
 	   "rt": the rt register.
 	   "rd": the rd register.
 	   "element": the element which holds the information.
 	   "fileName": the current file.
 	   "numLine": the current line.
 * OUTPUT - "True" if the process succeeded.
 	    "False": otherwise.
*/
boolean addOpRMachineCode(machine_code **node, reg rs, reg rt, reg rd, lookup_element element, char *fileName, unsigned int numLine);


/*
 * Creates a new node in the machine code linked-list, according to the information.
 * The function also creates the next node in the linked-list and links the current node to the next one.
 * The function is specified for R-type operation-lines ("add", "sub", "move", etc),
 * INPUT - "node": the current node.
 	   "rs": the rs register.
 	   "rt": the rt register.
 	   "immed": the immed value.
 	   "element": the element which holds the information.
 	   "fileName": the current file.
 	   "numLine": the current line.
 * OUTPUT - "True" if the process succeeded.
 	    "False": otherwise.
*/
boolean addOpIMachineCode(machine_code **node, reg rs, reg rt, long immed, lookup_element element, char *fileName, unsigned int numLine);


/*
 * Creates a new node in the machine code linked-list, according to the information.
 * The function also creates the next node in the linked-list and links the current node to the next one.
 * The function is specified for J-type operation-lines ("jmp", "la", "call", "stop").
 * INPUT - "node": the current node.
 	   "r": the "reg" segment in the J-line encoding.
 	   "address": the "address" segment in the J-line encoding.
 	   "element": the element which holds the information.
 	   "fileName": the current file.
 	   "numLine": the current line.
  * OUTPUT - "True" if the process succeeded.
 	     "False": otherwise.
*/
boolean addOpJMachineCode(machine_code **node, int r, long address, lookup_element element, char *fileName, unsigned int numLine);


/*
 * Creates a new node in the machine code linked-list, according to the information.
 * The function also creates the next node in the linked-list and links the current node to the next one.
 * The function is specified for data-code (".db", ".dh", ".dw", ".asciz"),
 * INPUT - "node": the current node.
 	   "type": the type of the data.
 	   "data": the referenced data.
 	   "fileName": the current file.
 	   "numLine": the current line.
 	   "DC": the data counter.
 * OUTPUT - "True": if the process succeeded.
 	    "False": otherwise.
*/
boolean addDataMachineCode(machine_code **node, dataType type ,long data, char *fileName, unsigned int numLine, int *DC); 


/*
 * Creates a new node in the extern-symbols linked-list, according to the information.
 * The function also creates the next node in the linked-list and links the current node to the next one.
 * INPUT - "node": the current node.
 	   "address": the address of the symbol.
 	   "name": the name of the symbol.
 	   "fileName": the current file.
 	   "numLine": the current line.
 * OUTPUT - "True": if the process succeeded.
 	    "False": otherwise.
*/
boolean addExtSymNode(extern_sym **node, long address, char *name, char *fileName, unsigned int numLine);


/*
 * Changes a number from decimal representation to a binary representation.
 * INPUT: "num": the decimal representaion.
 * OUTPUT: the binary representation of "num" (as an unsigned int). 
*/
unsigned int decToBin(long num);


/*
 * Frees the machine-code's linked-list's nodes.
 * INPUT: "node": the current node of the linked list.
 * OUTPUT: none.
*/
void freeCodeList(machine_code *node);


/*
 * Frees the symbols' linked-list's nodes.
 * INPUT: "node": the current node of the linked list.
 * OUTPUT: none.
*/
void freeSymbolList(symbol_table *node);


/*
 * Frees the extern symbols' linked-list's nodes.
 * INPUT: "node": the current node of the linked list.
 * OUTPUT: none.
*/
void freeExternList(extern_sym *node);

#endif
