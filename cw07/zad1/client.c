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
#include <signal.h>
#include "defs.h"
int pid;
/** funkcje porzadkujace */
void clean1(){
    kill(pid,SIGKILL);
}
void clean2(int i){
    kill(pid,SIGKILL);
}

int main(int argc, char * argv[]){
    if(argc<2){
        printf("podaj nazwe klienta\n");
        return 1;
    }
    /** nazwa kolejki servera jest arbitralna, lokalizacja binarki klienta taka, jak servera */
    int queue_id = msgget(ftok("tmp/servers.rip",1), 0);
    /** wysylamy nasza nazwe */
    text name;
    sprintf(name.text,"%s",argv[1]);
    name.mtype=CLIENTS;
    int rc = msgsnd(queue_id, &name, sizeof(name), 0);
    if (rc < 0) {
        printf("name not send, msgsnd errno: %d\n", rc);
        return 1;
    }
    /** czytamy id naszej kolejki utworzonej przez server */
    itgr buff;
    printf("logging %s at %d\n",name.text,queue_id);
    rc = msgrcv(queue_id, &buff, sizeof(buff), SERVERS, 0);
    if (rc < 0) {
        printf("id not recieved, msgrcv errno: %d\n", rc);
        return 1;
    }
    printf("logged as %d\n", buff.id);

    message msg;
    message reply;
    int id = buff.id;
    /* //wlasciwie tak mozna uzyskac ta kolejke
    char buff2[128];
    sprintf(buff2,"tmp/%s.rip",name.text);
    int id = msgget(ftok(buff2,1), 0);
    */
    /** mamy drugi proces na obsluge przychodzacych wiadomosci */
    pid = fork();
    atexit(clean1);
    signal(SIGINT,clean2);
    if(!pid){
        while(1){
            /** pakujemy tresc do struktury z data, adreatem, typem(dla obslugi kolejki) */
            fgets(msg.to, sizeof(msg.to), stdin);
            fgets(msg.what, sizeof(msg.what), stdin);
            msg.when=times();
            msg.mtype=CLIENTS;
            int rc = msgsnd(id, &msg, sizeof(msg), 0);
            if (rc < 0) {
                printf("message not send, msgsnd errno: %d\n", rc);
            }
        }
    }
    /** sami tylko odberamy wiadomosci */
    while(1){
        int rc = msgrcv(id, &reply, sizeof(reply), SERVERS, 0);
        if(rc>0){
            printf("\n\a\tFROM: %s\tWHEN: %s\t%s",reply.to,ctime(&reply.when),reply.what);
        }
    }
    return 0;
}
