LIBDIR = ./lib
SRCDIR = ./src
TESTDIR = ./test
CC = gcc
FLAGS = -Wall -Wno-unused-variable -I$(LIBDIR)
LIBS = sigsegv
DEPS = $(LIBDIR)/clat.h

all: build test

build: clat.o

test: main.o clat.o
	$(CC) $(FLAGS) $(TESTDIR)/main.o $(SRCDIR)/clat.o -l$(LIBS) -o test/main

main.o: $(TESTDIR)/main.c
	$(CC) $(FLAGS) -c $(TESTDIR)/main.c -o test/main.o

clat.o: $(SRCDIR)/clat.c $(DEPS)
	$(CC) $(FLAGS) -c $(SRCDIR)/clat.c -o $(SRCDIR)/clat.o

clean:
	find $(SRCDIR) -type f -name '*.o' -delete
	find $(TESTDIR) -type f -name '*.o' -delete

