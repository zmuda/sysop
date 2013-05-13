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

deinit:
	sudo umount /dev/mqueue
	sudo rm -rf /dev/mqueue/*
	sudo rm -rf /dev/mqueue
init:	
	sudo mkdir /dev/mqueue
	sudo mount -t mqueue none /dev/mqueue
