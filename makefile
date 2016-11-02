
OBJS = main.o clib.o
CC = gcc -Wall -g 
LIBS = sigsegv
DEPS = clib.h

all: main

main: main.o clib.o
	$(CC) $(OBJS) -l$(LIBS) -o main

main.o: main.c
	$(CC) -c main.c 

clib.o: clib.c $(DEPS)
	$(CC) -c clib.c

clean:
	rm *.o main


