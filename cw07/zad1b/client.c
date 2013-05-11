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
    /*
    struct mq_attr attr;
    attr.mq_maxmsg = QUEUESIZE;
    attr.mq_msgsize = sizeof(char)*128;
    attr.mq_flags = 0;
    mqd_t queue_id = mq_open (QUEUENAME, O_WRONLY, 0664, &attr);
    */
    mqd_t queue_id = createQueue(QUEUENAME,sizeof(char)*128);
    printf("opended server id:%d\n",queue_id);
    /** wysylamy nasza nazwe */
    char name[128];
    sprintf(name,"%s",argv[1]);
    int rc = mq_send(queue_id, name, sizeof(char)*128, 0);
    sleep(1);
    if (rc < 0) {
        printf("name not send to %d, msgsnd errno: %d\n", queue_id,rc);
        return 1;
    }
    /** otwieramy kolejki utworzone przez serwer - wiemy jakie sa ich nazwy */
    message msg;
    message reply;
    char buff2[128];
    sprintf(buff2,"/%s",argv[1]);
    mqd_t id = createQueue(buff2,sizeof(msg)); // do tej piszemy
    sprintf(buff2,"/%sr",argv[1]);
    mqd_t idrdonly = createQueue(buff2,sizeof(msg)); //z tej tylko czytamy

    printf("logged as %s\tid:%d\n", argv[1],id);

    /** mamy drugi proces na obsluge przychodzacych wiadomosci */
    pid = fork();
    atexit(clean1);
    signal(SIGINT,clean2);
    if(!pid){
        while(1){
            /** pakujemy tresc do struktury z data, adreatem, typem(dla obslugi kolejki) */
            fgets(msg.to, sizeof(msg.to), stdin);
            fgets(msg.what, sizeof(msg.what), stdin);
            msg.when=times(NULL);
            int rc = mq_send(idrdonly, (char*)(&msg), sizeof(msg) , 0);
            if (rc < 0) {
                printf("message not send, msgsnd errno: %d\n", rc);
            }
        }
    }
    /** sami tylko odberamy wiadomosci */
    while(1){
        sleep(1);
        rc = mq_receive(id, (char*)(&reply), sizeof(reply), NULL);
        if(rc>0){
            printf("\n\a\tFROM: %s\tWHEN: %s\t%s",reply.to,ctime(&reply.when),reply.what);
        }
    }
    return 0;
}
