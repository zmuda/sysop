CC=gcc
CFLAGS=-Wall
CDL=-ldl
#LMEM=-L lib/mem/src/ -lmem
LMEM=lib/libmem.a
SMEM=lib/libmem.so
LMAT=lib/libmatrix.a
SMAT=lib/libmatrix.so
LD_LIBRARY_PATH:=/home/klosz/agh/sysop/lab02/zad3/lib/

	#LD_LIBRARY_PATH=/home/klosz/agh/sysop/lab02/zad3/lib/mem/src/.libs/:/home/klosz/agh/sysop/lab02/zad3/lib/matrix/src/.libs/
	#export LD_LIBRARY_PATH

all: static shared user

static: static1 static2

shared: shared1 shared2

user: user1 user2

static1: src/testmem.c
	$(CC) $^ -o $@.out $(CDL) $(LMEM) 


shared1: src/testmem.c
	$(CC) $^ -o $@.out $(CDL) $(SMEM) 

user1: src/testmem.c
	$(CC) $^ -o $@.out $(CDL) -D DYNAMIC 


static2: src/testmatrix.c
	$(CC) $^ -o $@.out $(CDL) $(LMAT) $(LMEM)


shared2: src/testmatrix.c
	$(CC) $^ -o $@.out $(CDL) $(SMAT) $(SMEM)

user2: src/testmatrix.c
	$(CC) $^ -o $@.out $(CDL) -D DYNAMIC $(SMEM)

clean:
	rm -rf *.out

