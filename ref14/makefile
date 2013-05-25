CC=gcc
CFLAGS=-Wall

all: client.out server.out

%.out: %.c
	$(CC) $(CFLAGS) $< -o $@ -lrt

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf *~
	rm -rf tmp/*

