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
int shared;
void* memory;
char* server_path;
char* client_path;
int fifo;
int looks_for;
int operation_counter=1;
node* tab;
int* tab_i;
int* consistent;

///--------------------------------------

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
    sem_unlink(buff2);
}
void trypost_semaphore(const char* hash);
int try_semaphore(const char* hash);
void clean(int a){
    int i = *tab_i;
    while(i--){
        trypost_semaphore(tab[i].relative_name);
	}
    sem_trywait(fifo_semaphore);
    sem_post(fifo_semaphore);
    sem_trywait(counter_semaphore);
    sem_post(counter_semaphore);

    //zakomentowano niepożądane likwidacje
    //int i = *tab_i;
	while(i--){
	    if(!try_semaphore(tab[i].relative_name)){
            semaphore_from_hash_disposal(tab[i].relative_name);
	    }
	}
	//munmap(memory, MAXTAB*sizeof(node)+4*sizeof(int));
	//shm_unlink("/shared");
	//sem_close(counter_semaphore);
	//sem_close(fifo_semaphore);
	//sem_unlink("/readers_counter");
	//sem_unlink("/consistency");
	//sem_unlink("/fifos");
	//close(fifo);
	//remove(FPATH);
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
        //printf("%s <> %s\n",fpath,tab[i].relative_name);
        if(!strcmp(fpath,tab[i].relative_name))return &tab[i];
    }
    return NULL;
}

///--------------------------------------



void prepare_path(const char* fpath){
        char name[256];
        char command[256];
        strcpy(name,fpath);
        char* pos = strrchr(name,'/');
        if(pos){
            pos[0]=0;
            sprintf(command,"mkdir -p \"%s/.commited/%s\"",server_path,name);
            system(command);
        }
}
void prepar_clients_path(const char* buff2){
    char name[256];
    char command[256];
    strcpy(name,buff2);
    char* pos = strrchr(name,'/');
    if(pos){
        pos[0]=0;
        sprintf(command,"mkdir -p \"%s/%s\"",client_path,name);
        system(command);
    }
}
void update_stamp(const char* shorten,time_t mtime){
    char buff2[256];
    sprintf(buff2,"rm -rf \"%s/...%s.*\"",client_path,shorten);
    system(buff2);
    sprintf(buff2,"...%s.%ld",shorten,mtime);
    prepar_clients_path(buff2);
    sprintf(buff2,"touch \"%s/...%s.%ld\"",client_path,shorten,mtime);
    system(buff2);
}
void add_file(const char *fname){
    /** bedziemy zmieniac - blokujemy plik az do potwierdzenia przez server **/
    if(try_semaphore(fname))return;
    printf("created\t-> %s\n",fname);
    char command[256];
    prepare_path(fname);
    sprintf(command,"cp -p \"%s/%s\" \"%s/.commited/%s\"",client_path,fname,server_path,fname);
    system(command);
}
void update_file(node* found){
    char command[256];
    struct stat sb;
    char buff[256];
    sprintf(buff,"%s/%s",client_path,found->relative_name);
    stat(buff,&sb);
    if(sb.st_mtime != found->modified){
        /** czy zmienialismy 'oficjalna' wersje **/
        char buff2[256];
        sprintf(buff2,"%s/...%s.%ld",client_path,found->relative_name,found->modified);
        if(stat(buff2, &sb) == -1)return;
        /** bedziemy zmieniac - blokujemy plik az do potwierdzenia przez server **/
        if(try_semaphore(found->relative_name))return;
        printf("updated\t=> %s %ld %ld\n",found->relative_name,found->modified,sb.st_mtime);
        prepare_path(found->relative_name);
        sprintf(command,"cp -p \"%s/%s\" \"%s/.commited/%s\"",client_path,found->relative_name,server_path,found->relative_name);
        system(command);
        /** zapisujemy timestamp **/
        update_stamp(found->relative_name,found->modified);
    }
}
int commit_file(const char *fpath, const struct stat *sb,int typeflag){
    if(typeflag!=FTW_F)return 0;
    const char* shorten = fpath+strlen(client_path)+sizeof(char);
    if(shorten[0]=='.' && shorten[1]=='.' && shorten[2]=='.')return 0;
    node* found = find_version(shorten);
    /** czy mamy juz wersje tego pliku **/
    if(found==NULL){
        add_file(shorten);
        update_stamp(shorten,sb->st_mtime);
    } else {
        update_file(found);
    }
    return 0;
}

void scan_to_commit(){
    ftw(client_path,commit_file,2);
    /** czy usunelismy jakis plik w kliencie **/
    int i=*tab_i;
    struct stat sb;
    while(i--){
        /** czy usunelismy aktualny - patrz timestamp **/
        char buff[256];
        char buff2[256];
        sprintf(buff,"%s/%s",client_path,tab[i].relative_name);
        sprintf(buff2,"%s/...%s.%ld",client_path,tab[i].relative_name,tab[i].modified);
        if (tab[i].active && stat(buff, &sb) == -1 && stat(buff2, &sb) != -1) {
            /** bedziemy zmieniac - blokujemy plik az do potwierdzenia przez server **/
            if(try_semaphore(tab[i].relative_name))return;
            tab[i].active=0;
            printf("CLIENT deleted:\t\"%s\"\t%d\n",tab[i].relative_name,i);
        }
    }
}

int fetch_file(const char *fpath, const struct stat *sb,int typeflag){
    if(typeflag!=FTW_F)return 0;
    const char* shorten = fpath+strlen(server_path)+sizeof(char);
    node* found = find_version(shorten);
    char buff[256];
    sprintf(buff,"%s/%s",client_path,shorten);
    char command[256];
    /** nieaktywne rekordy usuwamy **/
    struct stat native;
    if(!found)return 0;
    if(!found->active){
        /** bedziemy zmieniac - blokujemy plik az do potwierdzenia przez server **/
        if(stat(buff,&native)<0)return 0;
        sprintf(command,"rm \"%s\"",buff);
        system(command);
        printf("REMOTE deleted:\t%s\n",buff);
        return 0;
    }
    stat(buff,&native);
    if(native.st_mtime!=sb->st_mtime){
        /** bedziemy zmieniac - blokujemy plik az do potwierdzenia przez server **/
        if(try_semaphore(shorten))return 0;
        sprintf(command,"cp -p \"%s\" \"%s\"",fpath,buff);
        prepar_clients_path(shorten);
        printf("REMOTE changed:\t%s\n",shorten);
        system(command);
        update_stamp(shorten,sb->st_mtime);
    }
    return 0;
}

void fetch_commited(){
    ftw(server_path,fetch_file,10);
}

void READ(int freshly_created){
	srand(time(NULL));
	int* readers_counter = memory;
	int* operating_next_pid = memory+sizeof(int);

	if(freshly_created){
		*readers_counter = 0;
		*operating_next_pid = 0;
	}

	int pid = getpid();
	int tmp;

	while(1){
	    usleep(10000);
	    /** dodajemy sie do kolejki */
		if(write(fifo, &pid, sizeof(int)) == -1){
			CLEANING_HANDLER(errno != EAGAIN)
		}
        /** uzyskujemy gwarancję niezmienności kolejki */
		CLEANING_HANDLER(sem_wait(fifo_semaphore) == -1)
        /** nikt nie czeka? */
		if(*operating_next_pid == 0){
			*operating_next_pid = pid;
            /** nikt inny nie czeka (pusta kolejka)
            *   ustalamy, ze teraz nasza kolej i zwalniamy kolejke */
			CLEANING_HANDLER(sem_post(fifo_semaphore) == -1)
		}else{
		    /** sprawdzamy czy teraz nasza kolej */
			if(*operating_next_pid == pid){
				/** wyznaczamy nastepna osobe wg kolejki */
				if(read(fifo, &tmp, sizeof(int)) == -1){
					CLEANING_HANDLER(errno!=EAGAIN)
                    *operating_next_pid = 0;
				}else{
					*operating_next_pid = tmp;
				}
				/** zakonczylismy operacjena kolejce */
				CLEANING_HANDLER(sem_post(fifo_semaphore) == -1)

                /** lockujemy na czas inkrementacji licznika */
				CLEANING_HANDLER(sem_wait(counter_semaphore) == -1)
				++(*readers_counter);
				CLEANING_HANDLER(sem_post(counter_semaphore) == -1)

                /** READ **/
                scan_to_commit();
                fetch_commited();

				CLEANING_HANDLER(sem_wait(counter_semaphore) == -1)
				--(*readers_counter);
				CLEANING_HANDLER(sem_post(counter_semaphore) == -1)
			}else{
			    /** jezeli nie nasza kole, zwalniamy i przechodzimy dalej */
				CLEANING_HANDLER(sem_post(fifo_semaphore) == -1)
			}
		}
	}
}

int present(const char *fpath, const struct stat *sb,int typeflag){
    if(typeflag!=FTW_F)return 0;
    const char* shorten = fpath+strlen(server_path)+sizeof(char);
    char* ptr = strrchr(shorten,'/');
    if(!ptr)ptr = strrchr(shorten,'.');
    if(!ptr)ptr = (char*)shorten;
    else ptr+=sizeof(char);
    time_t stamp = atoi(ptr);
    printf("type %s to choose:\t\t%s\n",ptr,ctime(&stamp));
    return 0;
}

int main(int argc, char* argv[]){
    if(argc<2){
        printf("usage: [path to storage folder]");
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
	/** semafory - na kolejke i na licznik; gotowosci servera */
	counter_semaphore = createSemafor("/readers_counter", &freshly_created);
	fifo_semaphore = createSemafor("/fifos", &freshly_created);
    consistent_semaphore = createSemafor("/consistency", &freshly_created);
    /** tworzymy albo otwieramy kolejke - to determinuje wystapienia inicjalizacji */
	if((shared = shm_open("/shared", O_RDWR | O_CREAT | O_EXCL, S_IRWXU)) == -1){
		HANDLER(errno != EEXIST)
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

    /** czekamy na inicjalizacje servera **/
    CLEANING_HANDLER(sem_wait(consistent_semaphore) == -1)
    *consistent=0;
    CLEANING_HANDLER(sem_post(consistent_semaphore) == -1)
    while(1){
        CLEANING_HANDLER(sem_wait(consistent_semaphore) == -1)
        if(*consistent)break;
        CLEANING_HANDLER(sem_post(consistent_semaphore) == -1)
    }

	CLEANING_HANDLER(sem_post(consistent_semaphore) == -1)
    if(freshly_created)*tab_i=0;

    client_path=malloc(256*sizeof(char));
    //sprintf(client_path,argv[0]);
    realpath(argv[1],client_path);

	if(!fork()){
        READ(freshly_created);
	} else {
	    while(1){
            printf("If you wish to recall any version\n");
            printf("provide name of file in following way: relative/path/to/file\n");
            printf("relative to monitored folder; no leading or following witespaces\n");
            char buff[256];
            scanf("%s",buff);

            /** blokujemy wszystko na czas wyboru **/
            char tmp[256];
            strcpy(tmp,buff);
            take_semaphore(tmp);
            printf("<%s>\n",tmp);
            CLEANING_HANDLER(sem_wait(consistent_semaphore) == -1)

			char path[256];
            sprintf(path,"%s/.%s",server_path,buff);
            printf("-possible-------------\n");
            ftw(path,present,2);
            char chosen[256];
            printf("----------------------\n");
            printf("type sth:\t");
            scanf("%s",chosen);
            sprintf(path,"%s/.%s/%s",server_path,buff,chosen);

            struct stat sb;
            if(!(stat(path,&sb)<0)){
                char command[256];
                /** wersje przywracamy przezu utworzenie nowego pliku **/
                sprintf(command,"cp \"%s\" \"%s/%s\"",path,client_path,buff);
                //printf("-|-\t%s\t-|-\n",command);
                system(command);
            } else printf("not reckognized\n");
            release_semaphore(tmp);
            CLEANING_HANDLER(sem_post(consistent_semaphore) == -1)
        }
	}

    free(client_path);
	clean(0);
	return 0;
}
