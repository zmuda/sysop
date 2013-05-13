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


/**
* tworzy plik kolejki o nazwie wynikajacej z unikalnej nazwy uzyszkodnika
* (dzieki temu nie nalezy przekazywac id kolejki (otworzyc kolejke z pliku mozna)
*    - ale i tak to robie)
* otwiera kolejke w tym pliku
*/
mqd_t createQueue(char* name,size_t size){
    struct mq_attr attr;
    attr.mq_maxmsg = QUEUESIZE;
    attr.mq_msgsize = size;
    attr.mq_flags = 0;
    mqd_t queue_id = mq_open (name, O_RDWR | O_CREAT |O_NONBLOCK, 0664, &attr);
    if(queue_id<0)perror(NULL);
	return queue_id;
}

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
    mqd_t queue_id = createQueue(QUEUENAME,sizeof(char)*128);
    if(queue_id<0){
        printf("SERVER not opened\n");
        perror(NULL);
        exit(1);
    }
    printf("opended server id:%d\n",queue_id);
    /** wysylamy nasza nazwe */
    char name[128];
    sprintf(name,"%s",argv[1]);
    int rc = mq_send(queue_id, name, sizeof(char)*128, 0);
    sleep(1);
    if (rc < 0) {
            printf("message not send");
            perror(NULL);
            return 1;
    }
    /** otwieramy kolejki utworzone przez serwer - wiemy jakie sa ich nazwy */
    message msg;
    message reply;
    char buff2[128];
    sprintf(buff2,"/%s",argv[1]);
    mqd_t id = createQueue(buff2,sizeof(msg)); // do tej piszemy
    if(id<0){
        printf("queue not opened\n");
        perror(NULL);
        exit(1);
    }

    if(id<0){
        printf("write queue not opened\n");
        perror(NULL);
        exit(1);
    }
    sprintf(buff2,"/%sr",argv[1]);
    mqd_t idrdonly = createQueue(buff2,sizeof(msg)); //z tej tylko czytamy
    if(idrdonly<0){
        printf("read queue not opened\n");
        perror(NULL);
        exit(1);
    }
    printf("logged as %s\tid:%d\n", argv[1],id);

    /** mamy drugi proces na obsluge przychodzacych wiadomosci */
    pid = fork();
    if(!pid){
        signal(SIGINT,breakloop);
        while(ender){
            /** pakujemy tresc do struktury z data, adreatem, typem(dla obslugi kolejki) */
            fgets(msg.to, sizeof(msg.to), stdin);
            fgets(msg.what, sizeof(msg.what), stdin);
            msg.when=times(NULL);
            int rc = mq_send(idrdonly, (char*)(&msg), sizeof(msg) , 0);
            if (rc < 0) {
                printf("message not send, msgsnd errno: %d\n", rc);
            }
        }
        exit(0);
    }
    /** sami tylko odberamy wiadomosci */
    signal(SIGINT,breakloop);
    while(ender){
        sleep(1);
        rc = mq_receive(id, (char*)(&reply), sizeof(reply), NULL);
        if(rc>0){
            printf("\n\a\tFROM: %s\tWHEN: %s\t%s",reply.to,ctime(&reply.when),reply.what);
        }
    }
    cleanup();
    return 0;
}
