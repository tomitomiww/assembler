assembler: assembler.o firstProcess.o secondProcess.o writeFiles.o handleFiles.o 
	gcc -ansi -Wall -g -pedantic assembler.o firstProcess.o secondProcess.o writeFiles.o handleFiles.o -o assembler
		
assembler.o: assembler.c 
	gcc -c -ansi -Wall -g -pedantic assembler.c -o assembler.o 
	
firstProcess.o: firstProcess.c firstProcess.h
	gcc -c -ansi -Wall -g -pedantic firstProcess.c -o firstProcess.o

secondProcess.o: secondProcess.c secondProcess.h
	gcc -c -ansi -Wall -g -pedantic secondProcess.c -o secondProcess.o
	
writeFiles.o: writeFiles.c writeFiles.h
	gcc -c -ansi -Wall -g -pedantic writeFiles.c -o writeFiles.o
	
handleFiles.o: handleFiles.c handleFiles.h
	gcc -c -ansi -Wall -g -pedantic handleFiles.c -o handleFiles.o
	
