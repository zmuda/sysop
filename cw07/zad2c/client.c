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

int flag;
void wait_for_nack(int i){
    flag=1;
}

void wait_for_ack(int i){
    flag=2;
}

int ender=1;
void breakloop(int i){
    ender=0;
}

int main(int argc, char * argv[]){
    signal(SIGUSR2,wait_for_nack);
    signal(SIGUSR1,wait_for_ack);
    if(argc<2){
        printf("podaj nazwe klienta\n");
        return 1;
    }

    mqd_t queue_id = createQueue(QUEUENAME,sizeof(message));
    if(queue_id<0){
        printf("SERVER not opened\n");
        perror(NULL);
        exit(1);
    }
    printf("opended server id:%d\n",queue_id);

    signal(SIGINT,breakloop);
    while(ender){
        /** wysylamy nasza wiadomosc */
        printf("any key to send message\n");
        getchar();
        message comm;
        comm.pid=getpid();
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
            printf("message not send");
            perror(NULL);
            return 1;
        } else {
            flag=0;
            while(!flag){sleep(1);};
            if(flag==1){
                printf("ALREADY FULLL\n");
            }else{
                printf("SEND:\t%s\n%s\n",comm.who,comm.what);
            }
        }
    }
    return 0;
}
