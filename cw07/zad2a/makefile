CC=gcc
CFLAGS=-Wall

all: client.out server.out

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lrt

%.out: %.o
	$(CC) $(CFLAGS) $< -o $@ -lrt

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf *~
	rm -rf tmp/*
