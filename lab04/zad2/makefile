CC = gcc
SHELL = /bin/bash
CFLAGS = -Wall -lrt -O2
.PHONY = clean all archiwizuj dopisz

all: archiwizuj dopisz

archiwizuj: zad2a.c
	mkdir archiwum
	$(CC) $(CFLAGS)  $^ -o $@.out

dopisz: zad2b.c
	$(CC) $(CFLAGS) $^ -o $@.out

clean:
	rm -f *.out
	rm -f *.o
