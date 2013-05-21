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
FILE* file;

void clean(int a){
	HANDLER(shmdt(memory) == -1)
	HANDLER(fclose(file) == EOF)
}

/**
* liczenie wyznacznika z rozwiniecia laplace'a
*/
double det(int n,int w, int * PermutationVector, struct matrix *mat){
    int i,j,k,m,*ColumnPermutation;
    double s;
    #define A (mat->data)
    if(n == 1){
        return A[w][PermutationVector[0]];
    }
    else{
        ColumnPermutation = malloc(sizeof(int)*(n - 1));
        s = 0;
        m = 1;
        for(i = 0; i < n; i++){
            k = 0;
            for(j = 0; j < n - 1; j++){
                if(k == i) k++;
                ColumnPermutation[j] = PermutationVector[k++];
            }
            s += m * A[w][PermutationVector[i]] * det(n-1, w + 1, ColumnPermutation, mat);
            m = -m;
        }
        free(ColumnPermutation);
        return s;
    }
}


void consume(int freshly_created ){
	struct sembuf semaphore_attributes;
	semaphore_attributes.sem_flg = 0;
    /** lokalizacja zmiennych w pamieci wspoldzielonej **/
	int* actual_position = memory+sizeof(int);
	struct matrix* tab = memory+2*sizeof(int);

    /** jezeli utworzylismy kolejki zaczynamy od 0 **/
	if(freshly_created )
		*actual_position = 0;
	int counter;

	while(1){
        /** zmniejszamy ilosc dostepnych produktow **/
		semaphore_attributes.sem_num = FULL_SEM;
		semaphore_attributes.sem_op = ZMNIEJSZ;
		CLEANING_HANDLER(semop(semaphores_set, &semaphore_attributes, 1) == -1)

        /** rezerwujemy mutex **/
		semaphore_attributes.sem_num = MUTEX;
		CLEANING_HANDLER(semop(semaphores_set, &semaphore_attributes, 1) == -1)

        /** aktualizujemy aktualna pozycje **/
		counter = *actual_position;
		(*actual_position) = (counter+1)%MAXTAB;

        /** WYZNACZNIK **/
        int PermutationVector[TABDIM],i;
        for(i = 0; i < TABDIM; i++) PermutationVector[i] = i;
        printf("Wyznacznik macierzy spod indeksu %d wynosi %f\n",counter,det(TABDIM, 0, PermutationVector, &tab[counter]));

        /** zwalniamy mutex **/
		semaphore_attributes.sem_op = ZWIEKSZ;
		CLEANING_HANDLER(semop(semaphores_set, &semaphore_attributes, 1) == -1)

        /** zwiekszamy ilosc idostepnego miejsca **/
		semaphore_attributes.sem_num = EMPTY_SEM;
		CLEANING_HANDLER(semop(semaphores_set, &semaphore_attributes, 1) == -1)
	}
}


int main(int argc, char* argv[]){
	key_t key;
	int freshly_created ;
    /** przygotowujemy i zapisujemy do pliku klucz **/
	HANDLER(signal(SIGINT, clean) == SIG_ERR)
	HANDLER((key = ftok(KPATH, KVAL)) == -1)
	/** otwieramy nasze semafory (caly zestaw) **/
	if((semaphores_set = semget(key, 3, IPC_CREAT | IPC_EXCL | S_IRWXU)) == -1){
		/** nie tworzymy, tylko otwieramy **/
		HANDLER(errno != EEXIST)
        freshly_created  = 0;
        HANDLER((semaphores_set = semget(key, 3, 0)) == -1)
	} else {
	    /** inicjalizujemy nasz zestaw semaforow **/
		HANDLER(semctl(semaphores_set, EMPTY_SEM, SETVAL, MAXTAB) == -1)
		HANDLER(semctl(semaphores_set, FULL_SEM, SETVAL, 0) == -1)
		HANDLER(semctl(semaphores_set, MUTEX, SETVAL, 1) == -1)
	}
    /**inicjalizujemy pamiec wspoldzielona **/
	if((shared = shmget(key, MAXTAB*sizeof(struct matrix)+2*sizeof(int), IPC_CREAT | IPC_EXCL | S_IRWXU)) == -1){
		HANDLER(errno != EEXIST)
        freshly_created  = 0;
        HANDLER((shared = shmget(key, MAXTAB*sizeof(struct matrix)+2*sizeof(int), 0)) == -1)
	}
	HANDLER((memory = shmat(shared, NULL, 0)) == NULL)

/*
	char buf[PATH_MAX];
	sprintf(buf, "%s%i", "/tmp/prod", getpid());
	HANDLER((file = fopen(buf, "w+")) == NULL)
*/
	consume(freshly_created );

	clean(0);
	return 0;
}
