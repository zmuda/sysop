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
#include <mqueue.h>
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
    mqd_t queue_id = createQueue(QUEUENAME,sizeof(message));
    printf("opended server id:%d\n",queue_id);
    char buff[128];
    sprintf(buff,"/%s",argv[1]);
    mqd_t id = createQueue(buff,sizeof(nack));
    perror(NULL);
    /** otwieramy kolejki utworzone przez serwer - wiemy jakie sa ich nazwy */

    while(1){
        /** wysylamy nasza wiadomosc */
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
        int rc = mq_send(queue_id, (char*)(&comm), sizeof(comm) , 0);
        if (rc < 0) {
            printf("message not send, msgsnd errno: %d\n", rc);
            return 1;
        } else {
            nack x;
            rc = mq_receive(id, (char*)(&x), sizeof(x), NULL);
            perror(NULL);
            if (!x.ok){
                printf("ALREADY FULLL\n");
            } else {
                printf("SEND:\t%s\n%s\n",comm.who,comm.what);
            }
        }
    }
}
