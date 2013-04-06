CC=gcc
CFLAGS=-Wall
E=echo
T=time
F="%esec\n"
P=tmpprog.out
S=tmpsys.out
L=1200
all: sys prog
sys: zad1.c
	$(CC) $(CFLAGS) $^ -o $@.out
prog: zad1.c
	$(CC) $(CFLAGS) $^ -o $@.out -D FHANDLE
clean:
	rm -rf *.out

times: sys prog
	$E SYSTEMOWO
	./sys.out generuj 0 $L $S
	$T -f $F ./sys.out sortuj $S
	./sys.out generuj 512 $L $S
	$T -f $F ./sys.out sortuj $S
	./sys.out generuj 4098 $L $S
	$T -f $F ./sys.out sortuj $S
	./sys.out generuj 8192 $L $S
	$T -f $F ./sys.out sortuj $S
	$E PROGRAMOWO
	./prog.out generuj 0 $L $P	
	$T -f $F ./prog.out sortuj $P
	./prog.out generuj 512 $L $P	
	$T -f $F ./prog.out sortuj $P
	./prog.out generuj 4098 $L $P	
	$T -f $F ./prog.out sortuj $P
	./prog.out generuj 8192 $L $P	
	$T -f $F ./prog.out sortuj $P


