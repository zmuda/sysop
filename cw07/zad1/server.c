#include <sys/stat.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include "defs.h"

// nie mozna sie wylogowac!

int queue_id;
int pid;
char** files;
int* ids;
int ids_size=0;

/**
* tworzy plik kolejki o nazwie wynikajacej z unikalnej nazwy uzyszkodnika
* (dzieki temu nie nalezy przekazywac id kolejki (otworzyc kolejke z pliku mozna)
*    - ale i tak to robie)
* otwiera kolejke w tym pliku
*/
int createQueue(char* name){
	close(open(name,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP));
	int queue_id= msgget(ftok(name,1), IPC_CREAT | S_IRUSR | S_IWUSR | S_IWGRP);
	return queue_id;
}

/**
* pomocnicza - sprzata kolejke i plik (nie gromadza sie komunikaty przypadkiem)
*/
void closeQueue(char* name,int queue_id){
    msgctl(queue_id, IPC_RMID, NULL);
	unlink(name);
}

/**
* zestaw funkcji porzadkujacych
*/
void clean1(){
    closeQueue("tmp/servers.rip",queue_id);
    int i;
    for(i=0; i<ids_size;i++){
        closeQueue(files[i],ids[i]);
        free(files[i]);
    }
}
void clean2(int i){
    clean1();
    exit(0);
}

/**
* znajduje id na podstawie unikalnej nazwy, ktora legitymuja sie klienci
*/
int find_id(char* name){
    int i=0;
    for(;i<ids_size;i++){
        if(!strcmp(name,files[i]))return ids[i];
    }
    return -1;
}

int main(int argc, char** argv){
    message msg;
    /** nibymapa */
    ids=malloc(LIMIT*sizeof(int));
    files=malloc(LIMIT*sizeof(char*));
    /** otwieramy arbitralnie zadana kolejke */
    queue_id = createQueue("tmp/servers.rip");
    /** czyszczeniw na wyjsciu */
    atexit(clean1);
    signal(SIGINT,clean2);
    int rc;
    while(1){
        text buff;
        sleep(1);
        /** odbior komunikatu nt. pojawienia sie klienta
        *   nie czekamy, musimy jeszcze obslugiwac transfer wiadomosci
        */
        rc = msgrcv(queue_id, &buff, sizeof(buff), CLIENTS, IPC_NOWAIT);
        if(rc>=0){
            char newbuff[128];
            /** nowy klieny - tworzymy mu kolejke */
            sprintf(newbuff,"tmp/%s.rip",buff.text);
            int created_id = createQueue(newbuff);
            printf("logging %s at %s as %d\n",buff.text,newbuff,id);
            /** 'pakujemy' id kolejki w opakowanie dla wiadomosci w kolejce
            *   ustalimy typ - umozliwienie transmisji dwukerunkowej
            */
            itgr id;
            id.id=created_id;
            id.mtype=SERVERS;
            /** wysylamy id kolejki */
            rc = msgsnd(queue_id, &id, sizeof(id2), 0);
            if (rc < 0) {
                printf("id not send, msgsnd errrno: %d\n", rc);
                return 1;
            }
            /** uzupelniamy 'mape' */
            ids[ids_size]=id;
            files[ids_size]=malloc(sizeof(char)*strlen(buff.text));
            sprintf(files[ids_size],"%s\n",buff.text);
            printf("\t%s\tis now registered\n",files[ids_size]);
            ids_size++;
        }
        int i;
        /** przeszukiwanie oczekujacych wiadomosci od istniejacych klientow */
        for(i=0;i<ids_size;i++){
            int rc = msgrcv(ids[i], &msg, sizeof(msg), CLIENTS, IPC_NOWAIT);
            if(rc>=0){
                msg.mtype=SERVERS;
                /** szukamy adresata */
                int to = find_id( msg.to);
                printf("<%d>",to);
                if(to>-1){
                    /** msg.to staje sie nazwa tego, ktory wyslal */
                    sprintf(msg.to,"%s",files[i]);
                    rc = msgsnd(to, &msg, sizeof(msg), 0);
                } else {
                    /** informujemy o zlym zaadresowaniu */
                    sprintf(msg.what,"ERROR: user offline!\n");
                    rc = msgsnd(ids[i], &msg, sizeof(msg), 0);
                }
                if (rc < 0) {
                    printf("message not forwarded, msgsnd errno: %d\n", rc);
                    return 1;
                }
            }
        }

    }

    return 0;
}
