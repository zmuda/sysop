#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/times.h>
#include "defs.h"


int pid;
/** funkcje porzadkujace */
void cleanup(){
    kill(pid,SIGINT);
}
int ender=1;
void breakloop(int i){
    ender=0;
}


int main(int argc, char * argv[]){
    if(argc<2){
        printf("podaj nazwe klienta\n");
        return 1;
    }
    /** nazwa kolejki servera jest arbitralna, lokalizacja binarki klienta taka, jak servera */
    int queue_id = msgget(ftok("tmp/servers.rip",1), 0);
    if(queue_id<0){
        printf("SERVER not opened\n");
        perror(NULL);
        exit(1);
    }
    /** wysylamy nasza nazwe */
    text name;
    sprintf(name.text,"%s",argv[1]);
    name.mtype=CLIENTS;
    int rc = msgsnd(queue_id, &name, sizeof(name), 0);
    if (rc < 0) {
            printf("message not send");
            perror(NULL);
            return 1;
    }
    /** czytamy id naszej kolejki utworzonej przez server */
    itgr buff;
    printf("logging %s at %d\n",name.text,queue_id);
    rc = msgrcv(queue_id, &buff, sizeof(buff), SERVERS, 0);
    if (rc < 0) {
        printf("message not recieved");
        perror(NULL);
        return 1;
    }
    printf("logged as %d\n", buff.id);

    message msg;
    message reply;
    int id = buff.id;
    /** mamy drugi proces na obsluge przychodzacych wiadomosci */
    pid = fork();
    if(!pid){
        signal(SIGINT,breakloop);
        while(ender){
            /** pakujemy tresc do struktury z data, adreatem, typem(dla obslugi kolejki) */
            fgets(msg.to, sizeof(msg.to), stdin);
            fgets(msg.what, sizeof(msg.what), stdin);
            msg.when=times(NULL);
            msg.mtype=CLIENTS;
            int rc = msgsnd(id, &msg, sizeof(msg), 0);
            if (rc < 0) {
                printf("message not send");
                perror(NULL);
                return 1;
            }
        }
        exit(0);
    }
    /** sami tylko odberamy wiadomosci */
    signal(SIGINT,breakloop);
    while(ender){
        int rc = msgrcv(id, &reply, sizeof(reply), SERVERS, 0);
        if(rc>0){
            printf("\n\a\tFROM: %s\tWHEN: %s\t%s",reply.to,ctime(&reply.when),reply.what);
        }
    }
    cleanup();
    return 0;
}
