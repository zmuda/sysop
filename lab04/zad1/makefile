CC = gcc
SHELL = /bin/bash
CFLAGS = -Wall
SOURCES = zad1.c
OBJ =  $(SOURCES:.c=.o)
.PHONY = clean all

all: plot

zad1fork: $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@.out -D FORK

zad1vfork: $(SOURCES)
	$(CC) $(CFLAGS)  $^ -o $@.out -D VFORK


zad1clone: $(SOURCES)
	$(CC) $(CFLAGS)  $^ -o $@.out -D CLONE

zad1vclone: $(SOURCES)
	$(CC) $(CFLAGS)  $^ -o $@.out -D VCLONE

plot: zad1fork zad1vfork zad1clone zad1vclone
	rm -f 'tmp.tmp'
	./zad1fork.out 30000
	./zad1fork.out 45000
	./zad1fork.out 60000
	./zad1fork.out 80000
	cp tmp.tmp fork.tmp
	rm -f 'tmp.tmp'
	./zad1vfork.out 30000
	./zad1vfork.out 45000
	./zad1vfork.out 60000
	./zad1vfork.out 80000
	cp tmp.tmp vfork.tmp
	rm -f 'tmp.tmp'
	./zad1clone.out 30000
	./zad1clone.out 45000
	./zad1clone.out 60000
	./zad1clone.out 80000
	cp tmp.tmp clone.tmp
	rm -f 'tmp.tmp'
	./zad1vclone.out 30000
	./zad1vclone.out 45000
	./zad1vclone.out 60000
	./zad1vclone.out 80000
	cp tmp.tmp vclone.tmp
	gnuplot plot.p
clean:
	rm -f *.out
	rm -f *.o
	rm -f *.tmp
	rm -f *.svg
