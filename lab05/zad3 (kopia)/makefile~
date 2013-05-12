CC = gcc
SHELL = /bin/bash
CFLAGS = -Wall
.PHONY = clean all

all: policz

zad3aS: zad3aSender.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3bS: zad3bSender.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3cS: zad3cSender.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3aR: zad3aReciever.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3bR: zad3bReciever.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3cR: zad3cReciever.c
	$(CC) $(CFLAGS)  $^ -o $@.out
policz: zad3aS zad3aR zad3bS zad3bR zad3cS zad3cR

clean:
	rm -f *.out
	rm -f *.o
