CC=gcc
CFLAGS=-Wall

all: generator tester


generator: generator.c
	$(CC) $(CFLAGS) $@.c -o $@.out
	
tester: tester.c
	$(CC) $(CFLAGS) -O1 $@.c -o $@.out

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf *~
