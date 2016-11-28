LIBDIR = ./../lib
SRCDIR = ./../src
TESTDIR = ./
CC = gcc
FLAGS = -Wall -Wno-unused-variable -I$(LIBDIR)
LIBS = sigsegv
DEPS = $(SRCDIR)/clat.o clock.h

SRCS = $(wildcard *.c)
PROGS = $(patsubst %.c,%,$(SRCS))

all: $(PROGS)

%: %.c
	$(CC) $(FLAGS) $(DEPS) -o $@ -l$(LIBS) $<

clean:
	rm -f $(PROGS)
