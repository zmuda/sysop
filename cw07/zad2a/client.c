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
#include "defs.h"


void cleanup(){
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
    int queue_id = msgget(ftok("tmp/servers.rip",1), 0);
    if(queue_id<0){
        printf("SERVER not opened\n");
        perror(NULL);
        exit(1);
    }
    signal(SIGINT,breakloop);
    while(ender){
        printf("any key to send message\n");
        getchar();
        message comm;
        sprintf(comm.who,"%s",argv[1]);
        int i;
        srand(times(NULL));
        for(i=0;i<LEN;i++){
            if(!(rand()%128))break;
            char c ='a'+rand()%24;
            comm.what[i]=c;
        }
        comm.what[i-1]=0;
        comm.mtype=CLIENTS;
        int rc = msgsnd(queue_id, &comm, sizeof(comm), 0);
        if (rc < 0) {
            printf("message not send");
            perror(NULL);
            return 1;
        } else {
            nack x;
            rc = msgrcv(queue_id, &x, sizeof(x), SERVERS, 0);
            if (!x.ok){
                printf("ALREADY FULLL\n");
            } else {
                printf("SEND:\t%s\n%s\n",comm.who,comm.what);
            }
        }
    }
    cleanup();
    return 0;
}
