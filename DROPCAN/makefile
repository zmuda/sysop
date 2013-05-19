CC=gcc
CFLAGS=-Wall

all: server.out client.out

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lrt

%.out: %.o
	$(CC) $(CFLAGS) $< -o $@ -lrt

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf *~
	rm -rf tmp/*

