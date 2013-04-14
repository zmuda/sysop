CC = gcc
SHELL = /bin/bash
CFLAGS = -Wall
.PHONY = clean all

all: policz

policz: zad3.c
	$(CC) $(CFLAGS)  $^ -o $@.out

clean:
	rm -f *.out
	rm -f *.o

