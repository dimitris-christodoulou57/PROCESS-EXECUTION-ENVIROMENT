all: main program

main: main.c
	gcc -Wall -g main.c -o main

program: program.c 
	gcc -Wall -g program.c -o program

