LIBDIR = ./lib
SRCDIR = ./src
TESTDIR = ./test
CC = gcc
FLAGS = -Wall -Wno-unused-variable -I$(LIBDIR)
LIBS = sigsegv
DEPS = $(LIBDIR)/clat.h

all: build test

build: clat.o

clat.o: $(SRCDIR)/clat.c $(DEPS)
	$(CC) $(FLAGS) -c $(SRCDIR)/clat.c -o $(SRCDIR)/clat.o -l$(LIBS) 

clean:
	find $(SRCDIR) -type f -name '*.o' -delete
	find $(TESTDIR) -type f -name '*.o' -delete

