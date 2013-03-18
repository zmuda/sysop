CC=gcc
CFLAGS=-Wall
CMATH=-lm
F="%esec\n"
A=-march=native
O=_opt
M=_march
E=echo
T=time

all: zad1a zad1b zad1c zad1d test


zad1a: zad1a.c
	$(CC) $(CFLAGS) $@.c -o $@.out
	$(CC) $(CFLAGS) -O1  $@.c -o $@$O.out
	$(CC) $(CFLAGS) $@.c -o $@$M.out $A
	$(CC) $(CFLAGS) -O1  $@.c -o $@$O$M.out $A

zad1b: zad1b.c
	$(CC) $(CFLAGS) -O1 $@.c -o $@.out
	$(CC) $(CFLAGS) -O2  $@.c -o $@$O.out
	$(CC) $(CFLAGS) -O1 $@.c -o $@$M.out $A
	$(CC) $(CFLAGS) -O2  $@.c -o $@$O$M.out $A	

zad1c: zad1c.c
	$(CC) $(CFLAGS) $(CMATH) -O1 $@.c -o $@.out
	$(CC) $(CFLAGS) $(CMATH) -O1 -ffast-math $@.c -o $@$O.out
	$(CC) $(CFLAGS) $(CMATH) -O1 $@.c -o $@$M.out $A
	$(CC) $(CFLAGS) $(CMATH) -O1 -ffast-math $@.c -o $@$O$M.out $A

zad1d: zad1d.c
	$(CC) $(CFLAGS) -O1 $@.c -o $@.out
	$(CC) $(CFLAGS) -O1 -funroll-loops $@.c -o $@$O.out
	$(CC) $(CFLAGS) -O1 $@.c -o $@$M.out $A
	$(CC) $(CFLAGS) -O1 -funroll-loops $@.c -o $@$O$M.out $A

test:
	$E 'zad1a bez optymalizacji'
	$T -f $F ./zad1a.out
	$E 'zad1a z optymalizacja O1'
	$T -f $F ./zad1a$O.out
	$E 'zad1b z optymalizacja O1'
	$T -f $F ./zad1b.out
	$E 'zad1b z optymalizacja O2'
	$T -f $F ./zad1b$O.out
	$E 'zad1c tylko z flaga O1'
	$T -f $F ./zad1c.out
	$E 'zad1c z flaga -ffast-math'
	$T -f $F ./zad1c$O.out
	$E 'zad1d tylko z flaga O1'
	$T -f $F ./zad1d.out
	$E 'zad1d z flaga -funroll-loops'
	$T -f $F ./zad1d$O.out
	$E 'POD ARCHITEKTURE '$A	
	$E 'zad1a bez optymalizacji'
	$T -f $F ./zad1a$M.out
	$E 'zad1a z optymalizacja O1'
	$T -f $F ./zad1a$O$M.out
	$E 'zad1b z optymalizacja O1'
	$T -f $F ./zad1b$M.out
	$E 'zad1b z optymalizacja O2'
	$T -f $F ./zad1b$O$M.out
	$E 'zad1c tylko z flaga O1'
	$T -f $F ./zad1c$M.out
	$E 'zad1c z flaga -ffast-math'
	$T -f $F ./zad1c$O$M.out
	$E 'zad1d tylko z flaga O1'
	$T -f $F ./zad1d$M.out
	$E 'zad1d z flaga -funroll-loops'
	$T -f $F ./zad1d$O$M.out	
	

clean:
	rm -rf *.out
