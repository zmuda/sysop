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
#include <sys/stat.h>
#include <ftw.h>

#include "macros.h"

sem_t* counter_semaphore;
sem_t* fifo_semaphore;
sem_t* consistent_semaphore;
char* server_path;
int shared;
void* memory;
int fifo;
node* tab;
int* tab_i;
int* consistent;


void flatten(char* str){
    char* pos = strrchr(str,'/');
    while(pos){
        pos[0]='_';
        pos = strrchr(str,'/');
    }
}
void semaphore_from_hash_disposal(const char* hash){
    char buff[256];
    strcpy(buff,hash);
    flatten(buff);
    char buff2[256];
    sprintf(buff2,"/%s___",buff);
    //printf("DISP %s\n",buff2);
    sem_unlink(buff2);
}

void prepare_path(const char* fpath){
        char name[256];
        char command[256];
        strcpy(name,fpath);
        //printf("[%s]",name);
        char* pos = strrchr(name,'/');
        if(pos){
            pos[0]=0;
            //printf("[%s]",name);
            sprintf(command,"mkdir -p \"%s/%s\"",server_path,name);
            system(command);
        }
}

void clean(int a){
    /*int fd = open("DUMP.CONF",O_CREAT|O_WRONLY,S_IRWXU);
    write(fd,memory,MAXTAB*sizeof(node)+4*sizeof(int)+sizeof(char)*256);
    close(fd);*/
    int i = *tab_i;
	while(i--){
        semaphore_from_hash_disposal(tab[i].relative_name);
	}
	munmap(memory, MAXTAB*sizeof(node)+4*sizeof(int));
	shm_unlink("/shared");
	sem_close(counter_semaphore);
	sem_close(fifo_semaphore);
	sem_unlink("/readers_counter");
	sem_unlink("/consistency");
	sem_unlink("/fifos");
	close(fifo);
	remove(FPATH);
	exit(0);
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

sem_t* semaphore_from_hash(const char* hash){
    char buff[256];
    strcpy(buff,hash);
    flatten(buff);
    char buff2[256];
    sprintf(buff2,"/%s___",buff);
    int k;//unused
    sem_t* semaphore = createSemafor(buff2, &k);
    return semaphore;
}
void take_semaphore(const char* hash){
    sem_t* semaphore = semaphore_from_hash(hash);
    CLEANING_HANDLER(sem_wait(semaphore) == -1)
}
void release_semaphore(const char* hash){
    sem_t* semaphore = semaphore_from_hash(hash);
    CLEANING_HANDLER(sem_post(semaphore) == -1)
}
int try_semaphore(const char* hash){
    sem_t* semaphore = semaphore_from_hash(hash);
    return sem_trywait(semaphore);
}
void trypost_semaphore(const char* hash){
    if(try_semaphore(hash))release_semaphore(hash);
}

node* find_version(const char *fpath){
    int i= *tab_i+1;
    while(i--){
        if(!strcmp(fpath,tab[i].relative_name))return &tab[i];
    }
    return NULL;
}
/** nowe zmiany zglaszane sa do folderu .commited
    tworzymy wersje w katlogach .[nazwa pliku]
    i dowiazania twarde do aktualnych wersji **/
void add_file(const char *fname){
    printf("created\t-> %s\n",fname);
    char command[256];
    struct stat sb;
    char buff[256];
    sprintf(buff,"%s/.commited/%s",server_path,fname);
    stat(buff,&sb);
    double hack = sb.st_mtime;
    char commited[256];
    strcpy(commited,buff);
    sprintf(command,"mkdir -p \"%s/.%s\"",server_path,fname);
    system(command);
    sprintf(command,"cp -p \"%s\" \"%s/.%s/%.0f\"",buff,server_path,fname,hack);
    system(command);
    tab[*tab_i].modified=sb.st_mtime;
    tab[*tab_i].active=1;
    strcpy(tab[*tab_i].relative_name,fname);
    *tab_i+=1;
    sprintf(command,"rm \"%s\"",commited);
    system(command);
    prepare_path(fname);
    sprintf(command,"ln -f \"%s/.%s/%.0f\" \"%s/%s\"",server_path,fname,hack,server_path,fname);
    system(command);
}
/** tu nie zakladam semaforow na konkretne pliki - i tak mam pelna kontrole
    serwer jest jedynym pisarzem **/
void update_file(node* found){
    printf("updated\t=> %s\n",found->relative_name);
    char command[256];
    struct stat sb;
    char buff[256];
    sprintf(buff,"%s/.commited/%s",server_path,found->relative_name);
    stat(buff,&sb);
    double hack = sb.st_mtime;
    char commited[256];
    strcpy(commited,buff);
    sprintf(command,"cp -p '%s' '%s/.%s/%.0f'",buff,server_path,found->relative_name,hack);
    system(command);
    found->modified=sb.st_mtime;
    found->active=1;
    sprintf(command,"rm '%s'",commited);
    system(command);
    prepare_path(found->relative_name);
    sprintf(command,"ln -f \"%s/.%s/%.0f\" \"%s/%s\"",server_path,found->relative_name,hack,server_path,found->relative_name);
    system(command);
}
int commit_file(const char *fpath, const struct stat *sb,int typeflag){
    if(typeflag!=FTW_F)return 0;
    char buff[256];
    sprintf(buff,"%s/.commited/",server_path);
    const char* shorten = fpath+strlen(buff);
    node* found = find_version(shorten);
    if(found==NULL){
        add_file(shorten);
    } else {
        update_file(found);
    }
    return 0;
}
void scan_commited(){
    char buff[256];
    sprintf(buff,"%s/.commited/",server_path);
    ftw(buff,commit_file,2);
}

void WRITE(int freshly_created){
	srand(time(NULL));

	int* readers_counter = memory;
	int* operating_next_pid = memory+sizeof(int);
	int pid = getpid();

	if(freshly_created){
		*readers_counter = 0;
		*operating_next_pid = 0;
	}
	while(1){
	    usleep(10000);
        if(write(fifo, &pid, sizeof(int)) == -1){
			CLEANING_HANDLER(errno != EAGAIN)
		}
        /** uzyskujemy gwarancję niezmienności kolejki */
		CLEANING_HANDLER(sem_wait(fifo_semaphore) == -1)
        /** nikt nie czeka? */
		if(*operating_next_pid == 0){
            /** nikt inny nie czeka (pusta kolejka)
            *   ustalamy, ze teraz nasza kolej i zwalniamy kolejke */
			CLEANING_HANDLER(sem_post(fifo_semaphore) == -1)
			*operating_next_pid = pid;
		}else{
            /** sprawdzamy czy teraz nasza kolej */
			if(*operating_next_pid == pid){
                /** wyznaczamy nastepna osobe wg kolejki */
				int tmp;
				if(read(fifo, &tmp, sizeof(int)) == -1){
					if(errno == EAGAIN){
						*operating_next_pid = 0;
					}
					CLEANING_HANDLER(errno!=EAGAIN)
				}else{
					*operating_next_pid = tmp;
				}
                /** zakonczylismy operacjena kolejce */
				CLEANING_HANDLER(sem_post(fifo_semaphore) == -1)
                /** czekamy na brak czytelnikow */
				while(1){
                    /** lockujemy na czas sprawdzenia licznika
                    *   nikt nie powiekszy licznika - my okupujemy kolejke */
					CLEANING_HANDLER(sem_wait(counter_semaphore) == -1)
                    if(*readers_counter == 0)
						break;
                    /** odblokowujemy aby mozna bylo zmniejszyc */
					CLEANING_HANDLER(sem_post(counter_semaphore) == -1)
				}
                /** WRITE **/
                scan_commited();
                int i=*tab_i;
                while(i--){
                    trypost_semaphore(tab[i].relative_name);
                }
				/** stan ustalony - mozna sie podlaczyc **/
                CLEANING_HANDLER(sem_wait(consistent_semaphore) == -1)
				*consistent=1;
				CLEANING_HANDLER(sem_post(consistent_semaphore) == -1)
				/** odlokowujemy licznik - czytelnicy moga sie znow dodawac */
				CLEANING_HANDLER(sem_post(counter_semaphore) == -1)
			}else{
			    /** nieistniejace juz procesy **/
			    int tmp;
			    if(kill(*operating_next_pid,0)){
                    if(read(fifo, &tmp, sizeof(int)) == -1){
                        if(errno == EAGAIN){
                            *operating_next_pid = 0;
                        }
                        CLEANING_HANDLER(errno!=EAGAIN)
                    }else{
                        *operating_next_pid = tmp;
                    }
			    }
			    /** jezeli nie nasza kole, zwalniamy i przechodzimy dalej */
				CLEANING_HANDLER(sem_post(fifo_semaphore) == -1)
			}
		}
	}
}




int main(int argc, char* argv[]){
    if(argc<2){
        printf("usage: [path to servers storage folder]");
        return 1;
    }

	int freshly_created = 1;
	HANDLER(signal(SIGINT, clean) == SIG_ERR)
	signal(SIGSEGV, clean);
    /** otwieramy kolejke na czytelnikow i pisarzy (albo tworzymy) */
	if(mkfifo(FPATH, S_IRWXU) == -1){
		HANDLER(errno != EEXIST)
	}
	HANDLER((fifo = open(FPATH, O_RDWR | O_NONBLOCK)) == -1)
    /** semafory - na kolejke i na licznik, gotowosc serwera*/
	counter_semaphore = createSemafor("/readers_counter", &freshly_created);
	fifo_semaphore = createSemafor("/fifos", &freshly_created);
    consistent_semaphore = createSemafor("/consistency", &freshly_created);
    /** tworzymy albo otwieramy kolejke - to determinuje wystapienia inicjalizacji */
	if((shared = shm_open("/shared", O_RDWR | O_CREAT | O_EXCL, S_IRWXU)) == -1){
		HANDLER(errno!=EEXIST)
        freshly_created = 0;
        HANDLER((shared = shm_open("/shared", O_RDWR, 0)) == -1)
	}
	/** otwieramy i ustalamy rozmiar pamieci wspoldzielonej */
	HANDLER(ftruncate(shared, MAXTAB*sizeof(node)+4*sizeof(int)+sizeof(char)*256) == -1)
	HANDLER((memory = mmap(NULL, MAXTAB*sizeof(node)+4*sizeof(int)+sizeof(char)*256, PROT_READ | PROT_WRITE, MAP_SHARED, shared, 0)) == NULL)

    tab = memory+4*sizeof(int)+sizeof(char)*256;
    server_path = memory+4*sizeof(int);
    tab_i = memory+3*sizeof(int);
    consistent = memory+2*sizeof(int);

    {
        *consistent = 1;
        *tab_i = 0;
        /** inicjalizacja sciezki **/
        realpath(argv[1],server_path);
        /** gdyby nie bylo miejsca **/
        char tmp[256];
        sprintf(tmp,"mkdir -p \"%s/.commited/\"",server_path);
        system(tmp);
    }

	WRITE(freshly_created);

	clean(0);
	return 0;
}
