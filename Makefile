all: valws379 process
valws379: valws379-input.c
	gcc valws379-input.c -o valws379
process: valws379-process.c
	gcc valws379-process.c -o process
