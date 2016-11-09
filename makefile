
OBJS = main.o clat.o
CC = gcc -Wall -Wno-unused-variable -g 
LIBS = sigsegv
DEPS = clat.h

all: main

main: main.o clat.o
	$(CC) $(OBJS) -l$(LIBS) -o main

main.o: main.c
	$(CC) -c main.c 

clat.o: clat.c $(DEPS)
	$(CC) -c clat.c

clean:
	rm *.o main


