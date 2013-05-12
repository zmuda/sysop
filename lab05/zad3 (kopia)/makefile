CC = gcc
SHELL = /bin/bash
CFLAGS = -Wall
.PHONY = clean all

all: policz

zad3aSender: zad3aSender.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3bSender: zad3bSender.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3cSender: zad3cSender.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3aReciever: zad3aReciever.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3bReciever: zad3bReciever.c
	$(CC) $(CFLAGS)  $^ -o $@.out
zad3cReciever: zad3cReciever.c
	$(CC) $(CFLAGS)  $^ -o $@.out
policz: zad3aS zad3aR zad3bS zad3bR zad3cS zad3cR

clean:
	rm -f *.out
	rm -f *.o
