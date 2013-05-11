CC=gcc
CFLAGS=-Wall

all: client.out server.out

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.out: %.o
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf *~
