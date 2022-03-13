#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

/* Represents a boolean variable */
typedef enum {
		False = 0,
		True = 1
} boolean;

/* Represents an operation type - R, I or J */
typedef enum {
		R = 1,
		I,
		J,
		NULL_OPT = 0
} opType;

/* Represents a data-operation type - .db, .dh, .dw or .asciz */
typedef enum {
		DB = 1,
		DH,
		DW,
		ASCIZ,
		NULL_DATA = 0
} dataType;

/* Represents funct values of operations */
typedef enum {
		/* type = R, opcode = 0 */
		ADD_FUNCT = 1,
		SUB_FUNCT = 2,
		AND_FUNCT = 3,
		OR_FUNCT = 4,
		NOR_FUNCT = 5,
		
		/* type = R, opcode = 1 */
		MOVE_FUNCT = 1,
		MVHI_FUNCT = 2,
		MVLO_FUNCT = 3,
		
		/* Error funct */
		NULL_FUNCT = 0
} funct;

/* Represents operation-codes of operations */
typedef enum {
		/* type = R */
		ADD_OPC = 0,
		SUB_OPC = 0,
		AND_OPC = 0,
		OR_OPC = 0,
		NOR_OPC = 0,
		MOVE_OPC = 1,
		MVHI_OPC = 1,
		MVLO_OPC = 1,
		
		/* type = I */
		ADDI_OPC = 10,
		SUBI_OPC = 11,
		ANDI_OPC = 12,
		ORI_OPC = 13,
		NORI_OPC = 14,
		BNE_OPC = 15,
		BEQ_OPC = 16,
	    	BLT_OPC = 17,
		BGT_OPC = 18,
		LB_OPC = 19,
		SB_OPC = 20,
		LW_OPC = 21,
		SW_OPC = 22,
		LH_OPC = 23,
		SH_OPC = 24,
		
		/* type = J */
		JMP_OPC = 30,
		LA_OPC = 31,
		CALL_OPC = 32,
		STOP_OPC = 63,
		
		/* Error opcode */
		NULL_OPC = -1
} opcode;

/* Represents registers */
typedef enum {
		R0 = 0,
		R1,
		R2,
		R3,
		R4,
		R5,
		R6,
		R7,
		R8,
		R9,
		R10,
		R11,
		R12,
		R13,
		R14,
		R15,
		R16,
		R17,
		R18,
		R19,
		R20,
		R21,
		R22,
		R23,
		R24,
		R25,
		R26,
		R27,
		R28,
		R29,
		R30, 
		R31,
		NULL_REG = -1
} reg;

/* Represents symbol-attributes of the symbol-table */
typedef enum {
		CODE,
		DATA,
		EXTERNAL,
		CODE_ENTRY,
		DATA_ENTRY
} sym_attr;

/* Represents R-type operations */
typedef struct r_op {
			unsigned int NOT_USED: 6;
			unsigned int funct: 5;
			unsigned int rd: 5;
			unsigned int rt: 5;
			unsigned int rs: 5;
			unsigned int opcode: 6;
} r_op;

/* Represents I-type operations */
typedef struct i_op {
			unsigned int immed: 16;
			unsigned int rt: 5;
			unsigned int rs: 5;
			unsigned int opcode: 6;
} i_op;

/* Represents J-type operations */
typedef struct j_op {
			unsigned int address: 25;
			unsigned int reg: 1;
			unsigned int opcode: 6;
} j_op;

/* Represents operation-words (R, I or J) */
typedef struct op_code {
			opType type; /* determine what is the type of the operation */
			union opu {
					r_op *R;
					i_op *I;
					j_op *J;
			} opu;
} op_code;

/* Represents .db data words */
typedef struct db_code {
			unsigned int data: 8;
} db_code;

/* Represents .dh data words */
typedef struct dh_code {
			unsigned int data: 16;
} dh_code;

/* Represents .dw data words */
typedef struct dw_word {
			long data;
} dw_code;

/* Represents data-words (.db, .dh or .dw) */
typedef struct data_code {
			dataType type; /* determine what is the type of the data */
			union du {
					db_code *db;
					dh_code *dh;
					dw_code *dw;
			} du;
} data_code;

/* Represents machine words (operation or data) */
typedef struct machine_code {
			unsigned int numLine; 
			boolean isOp; /* determine if it's an operation or data */
			union mu {
					op_code *op;
					data_code *data;
			} mu;
			struct machine_code *next; /* A pointer to the next machine-word */
} machine_code;

/* Represents a node in symbol-table-linked-list */
typedef struct symbol_table {
			char *symbol;
			int value;
			sym_attr attribute;
			struct symbol_table *next; /* A pointer to the next symbol */
} symbol_table;

/* Represents an element in the LOOKUP_TABLE */
typedef struct lookup_element {
			char *word;
			opType opT;
			dataType data;
			funct func;
			opcode opC;
} lookup_element;


/* Represents external symbols which are being used in the program */
typedef struct extern_sym {
			char *name;
			long address;
			struct extern_sym *next;
} extern_sym;

#endif
