;errors file

.entry Next
.extern: wNumber
STR: .asciz "aBcd
		.asciz abcd"
MAIN: add $3, 5, $9
LOOP: ori $9, 99999999999,$2
	la val
	j Next
Next1: move $43, $4
LIST: .db 5000,-9
	bgt $4 $2, END
	la    K
	sw $0, , 4, $10
	bne $31, $-9, LOOP
	callvall
	jmp 4
	la wNumber
.extern vall
	.dd 27056
K: .d 31, -12
END: stop please
.entry K
