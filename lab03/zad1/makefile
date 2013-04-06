CC=gcc
CFLAGS=-Wall
all: sys prog
sys: zad1.c
	$(CC) $(CFLAGS) $^ -o $@.out
prog: zad1.c
	$(CC) $(CFLAGS) $^ -o $@.out
clean:
	rm -rf *.out
