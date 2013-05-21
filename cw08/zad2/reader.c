#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include "macros.h"

sem_t* counter_semaphore;
sem_t* fifo_semaphore;
int shared;
void* memory;
int fifo;
int looks_for;
int operation_counter=1;

void clean(int a){
	HANDLER(munmap(memory, MAXTAB*sizeof(int)+2*sizeof(int)) == -1)
	HANDLER(shm_unlink("/shared") == -1)
	HANDLER(sem_close(counter_semaphore) == -1)
	HANDLER(sem_close(fifo_semaphore) == -1)
	HANDLER(sem_unlink("/readers_counter") == -1)
	HANDLER(sem_unlink("/fifos") == -1)
	HANDLER(close(fifo) == -1)
	remove(FPATH);
}

void finding(int* tab){
	int  counter = 0;
	int i;
	for(i = 0; i < MAXTAB; ++i)
		if(looks_for==tab[i])
            counter++;
	printf("(%d) %d found %d times\n",operation_counter++,looks_for,counter);
}

void READ(int freshly_created){
	srand(time(NULL));
	int* readers_counter = memory;
	int* operating_next_pid = memory+sizeof(int);
	int* tab = memory+2*sizeof(int);

	if(freshly_created){
		*readers_counter = 0;
		*operating_next_pid = 0;
	}

	int pid = getpid();
	int tmp;

	while(1){
	    /**
	    * dodajemy sie do kolejki
	    */
		if(write(fifo, &pid, sizeof(int)) == -1){
			CLEANING_HANDLER(errno != EAGAIN)
		}

        /**
        * uzyskujemy gwarancję niezmienności kolejki
        */
		CLEANING_HANDLER(sem_wait(fifo_semaphore) == -1)

        /**
        * nikt nie czeka?
        */
		if(*operating_next_pid == 0){
			*operating_next_pid = pid;
            /**
            * nikt inny nie czeka (pusta kolejka)
            * ustalamy, ze teraz nasza kolej i zwalniamy kolejke
            */
			CLEANING_HANDLER(sem_post(fifo_semaphore) == -1)
		}else{
		    /**
            * sprawdzamy czy teraz nasza kolej
            */
			if(*operating_next_pid == pid){
				/**
				* wyznaczamy nastepna osobe wg kolejki
				*/
				if(read(fifo, &tmp, sizeof(int)) == -1){
					CLEANING_HANDLER(errno!=EAGAIN)
                    *operating_next_pid = 0;
				}else{
					*operating_next_pid = tmp;
				}
				/**
				* zakonczylismy operacjena kolejce
				*/
				CLEANING_HANDLER(sem_post(fifo_semaphore) == -1)

                /**
                * lockujemy na czas inkrementacji licznika
                */
				CLEANING_HANDLER(sem_wait(counter_semaphore) == -1)
				++(*readers_counter);
				CLEANING_HANDLER(sem_post(counter_semaphore) == -1)
                //PERFORM READ
                finding(tab);
                //DONE
                /**
                * lockujemy na czas dekrementacji licznika
                */
				CLEANING_HANDLER(sem_wait(counter_semaphore) == -1)
				--(*readers_counter);
				CLEANING_HANDLER(sem_post(counter_semaphore) == -1)
			}else{
			    /**
			    * jezeli nie nasza kole, zwalniamy i przechodzimy dalej
			    */
				CLEANING_HANDLER(sem_post(fifo_semaphore) == -1)
			}
		}
	}
}

sem_t* createSemafor(char* path, int* freshly_created){
	sem_t* sem;
	if((sem = sem_open(path, O_CREAT | O_EXCL, S_IRWXU, 1)) == SEM_FAILED){
		HANDLER(errno != EEXIST)
		freshly_created = 0;
		HANDLER((sem = sem_open(path,  0)) == SEM_FAILED)
	}
	return sem;
}

int main(int argc, char* argv[]){
    if(argc<2){
        printf("usage: [value to look for]\n");
        exit(1);
    }
    looks_for=atoi(argv[1]);

	int freshly_created = 1;
    HANDLER(signal(SIGINT, clean) == SIG_ERR)
	signal(SIGSEGV, clean);
    /**
    * otwieramy kolejke na czytelnikow i pisarzy (albo tworzymy)
    */
	if(mkfifo(FPATH, S_IRWXU) == -1){
		HANDLER(errno != EEXIST)
	}
	HANDLER((fifo = open(FPATH, O_RDWR | O_NONBLOCK)) == -1)
    /**
    * semafory - na kolejke i na licznik
    */
	counter_semaphore = createSemafor("/readers_counter", &freshly_created);
	fifo_semaphore = createSemafor("/fifos", &freshly_created);
    /**
    * tworzymy albo otwieramy kolejke - to determinuje wystapienia inicjalizacji
    */
	if((shared = shm_open("/shared", O_RDWR | O_CREAT | O_EXCL, S_IRWXU)) == -1){
		HANDLER(errno != EEXIST)
        freshly_created = 0;
        HANDLER((shared = shm_open("/shared", O_RDWR, 0)) == -1)
	}
	/**
	* otwieramy i ustalamy rozmiar pamieci wspoldzielonej
	*/
	HANDLER(ftruncate(shared, MAXTAB*sizeof(int)+sizeof(int)) == -1)
	HANDLER((memory = mmap(NULL, MAXTAB*sizeof(int)+2*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shared, 0)) == NULL)

	READ(freshly_created);

	clean(0);
	return 0;
}
