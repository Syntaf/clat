
OBJS = main.o
CC = gcc -Wall -g 
LIBS = sigsegv

all: main

main: main.o
	$(CC) main.o -l$(LIBS) -o main

main.o: main.c
	$(CC) -c main.c 

clean:
	rm *.o main


