CC=gcc
CFLAGS=-Wall

all: policz dodaj_plik


policz: policz.c
	$(CC) $(CFLAGS) $@.c -o $@.out
	
dodaj_plik: dodaj_plik.c
	$(CC) $(CFLAGS) -O1 $@.c -o $@.out

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf *~
	rm -rf wyniki.log
