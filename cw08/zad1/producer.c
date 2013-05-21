#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include "macros.h"

int semaphores_set;
int shared;
void* memory;

void clean(int a){
	HANDLER(shmdt(memory) == -1)
	HANDLER(shmctl(shared, IPC_RMID, NULL) == -1)
	HANDLER(semctl(semaphores_set, IPC_RMID, 0) == -1)
}

void produce(int freshly_created){
	srand(time(NULL));
	struct sembuf semaphore_attributes;
	semaphore_attributes.sem_flg = 0;
    /** lokalizacja zmiennych w pamieci wspoldzielonej **/
	int* actual_position = memory;
    struct matrix* tab = memory+2*sizeof(int);

    /** jezeli utworzylismy kolejki zaczynamy od 0 **/
	if(freshly_created)
		*actual_position = 0;
	int counter;

	while(1){
        /** zmniejszamy ilosc wolnego miejsca **/
		semaphore_attributes.sem_num = EMPTY_SEM;
		semaphore_attributes.sem_op = ZMNIEJSZ;
		CLEANING_HANDLER(semop(semaphores_set, &semaphore_attributes, 1) == -1)

        /** rezerwujemy mutex **/
		semaphore_attributes.sem_num = MUTEX;
		CLEANING_HANDLER(semop(semaphores_set, &semaphore_attributes, 1) == -1)

        /** aktualizujemy aktualna pozycje **/
		counter = *actual_position;
		(*actual_position) = (counter+1)%MAXTAB;

        /** GENERUJEMY **/
		tab[counter].dimmension=TABDIM;
		int i, j;
		for(i = 0; i < TABDIM; ++i)
			for(j = 0; j < TABDIM; ++j){
				tab[counter].data[i][j] = rand()/(rand()+0.1);
			}
		printf("Task added to %i cell\n", counter);

		/** zwalniamy mutex **/
		semaphore_attributes.sem_op = ZWIEKSZ;
		CLEANING_HANDLER(semop(semaphores_set, &semaphore_attributes, 1) == -1)

        /** zwiekszamy ilosc istniejacych elem **/
		semaphore_attributes.sem_num = FULL_SEM;
		CLEANING_HANDLER(semop(semaphores_set, &semaphore_attributes, 1) == -1)
	}
}


int main(int argc, char* argv[]){
	int des;
	key_t klucz;
	int freshly_created = 1;
    /** przygotowujemy i zapisujemy do pliku klucz **/
	HANDLER(signal(SIGINT, clean) == SIG_ERR)
	HANDLER((des = open(KPATH, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
	HANDLER(close(des) == -1)
	HANDLER((klucz = ftok(KPATH, KVAL)) == -1)
    /** otwieramy nasze semafory (caly zestaw) **/
	if((semaphores_set = semget(klucz, 3, IPC_CREAT | IPC_EXCL | S_IRWXU)) == -1){
	    /** nie tworzymy, tylko otwieramy **/
		HANDLER(errno != EEXIST)
        freshly_created = 0;
        HANDLER((semaphores_set = semget(klucz, 3, 0)) == -1)
	} else {
	    /** inicjalizujemy nasz zestaw semaforow **/
		HANDLER(semctl(semaphores_set, EMPTY_SEM, SETVAL, MAXTAB) == -1)
		HANDLER(semctl(semaphores_set, FULL_SEM, SETVAL, 0) == -1)
		HANDLER(semctl(semaphores_set, MUTEX, SETVAL, 1) == -1)
	}
    /**inicjalizujemy pamiec wspoldzielona **/
	if((shared = shmget(klucz, MAXTAB*sizeof(struct matrix)+2*sizeof(int), IPC_CREAT | IPC_EXCL | S_IRWXU)) == -1){
		HANDLER(errno != EEXIST)
        freshly_created = 0;
        HANDLER((shared = shmget(klucz, MAXTAB*sizeof(struct matrix)+2*sizeof(int), 0)) == -1)

	}
	HANDLER((memory = shmat(shared, NULL, 0)) == NULL)

	produce(freshly_created);

	clean(0);
	return 0;
}
