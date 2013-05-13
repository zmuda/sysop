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
#include <mqueue.h>
#include "defs.h"

// nie mozna sie wylogowac!

mqd_t queue_id;
mqd_t* ids;
mqd_t* idsr;
char** files;
int ids_size=0;

/**
* pomocnicza - sprzata kolejke i plik (nie gromadza sie komunikaty przypadkiem)
*/
void closeQueue(char* name,mqd_t queue_id){
    mq_close(queue_id);
	unlink(name);
}


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

/**
* zestaw funkcji porzadkujacych
*/
void cleanup(){
    closeQueue(QUEUENAME,queue_id);
    int i;
    for(i=0; i<ids_size;i++){
        closeQueue(files[i],ids[i]);
        free(files[i]);
    }
    free(ids);
    free(idsr);
    free(files);
}
/**
* glowna petle (czyli program) konczy sie ctrl+c
*/
int flag =1;
void breakloop(int i){
    flag=0;
}

/**
* znajduje id na podstawie unikalnej nazwy, ktora legitymuja sie klienci
*/
mqd_t find_id(char* name){
    int i=0;
    for(;i<ids_size;i++){
        if(!strcmp(name,files[i]))return ids[i];
    }
    return -1;
}

int main(int argc, char** argv){
    message msg;
    /** nibymapa */
    ids=malloc(LIMIT*sizeof(mqd_t));
    idsr=malloc(LIMIT*sizeof(mqd_t));
    files=malloc(LIMIT*sizeof(char*));
    /** otwieramy arbitralnie zadana kolejke */
    queue_id = createQueue(QUEUENAME,sizeof(char)*128);
    if(queue_id<0){
        printf("SERVER not created\n");
        perror(NULL);
        exit(1);
    }
    printf("SERVER %s at %d\n",QUEUENAME,queue_id);
    signal(SIGINT,breakloop);
    int rc;
    while(flag){
        sleep(1);
        /** odbior komunikatu nt. pojawienia sie klienta
        *   nie czekamy, musimy jeszcze obslugiwac transfer wiadomosci
        */
        char buff[128];
        rc = mq_receive(queue_id, (char*)buff, sizeof(buff), NULL);
        if(rc>=0){
            char newbuff[128];
            /** nowy klieny - tworzymy mu kolejke */
            sprintf(newbuff,"/%s",buff);
            mqd_t created_id = createQueue(newbuff,sizeof(msg));
            printf("logging %s at %s as %d\n",buff,newbuff,created_id);
            sprintf(newbuff,"/%sr",buff);
            mqd_t created_idr = createQueue(newbuff,sizeof(msg));
            /** 'pakujemy' id kolejki w opakowanie dla wiadomosci w kolejce
            *   ustalimy typ - umozliwienie transmisji dwukerunkowej
            */
            /** uzupelniamy 'mape' */
            ids[ids_size]=created_id;
            idsr[ids_size]=created_idr;
            files[ids_size]=malloc(sizeof(char)*strlen(buff));
            sprintf(files[ids_size],"%s\n",buff);
            printf("\t%s\tis now registered\n",files[ids_size]);
            ids_size++;
        }
        int i;
        /** przeszukiwanie oczekujacych wiadomosci od istniejacych klientow */
        for(i=0;i<ids_size;i++){
            int rc = mq_receive(idsr[i], (char*)(&msg), sizeof(msg), NULL);
            if(rc>=0){
                /** szukamy adresata */
                int to = find_id( msg.to);
                printf("<%d>",to);
                if(to>-1){
                    /** msg.to staje sie nazwa tego, ktory wyslal */
                    sprintf(msg.to,"%s",files[i]);
                } else {
                    /** informujemy o zlym zaadresowaniu */
                    sprintf(msg.what,"ERROR: user offline!\n");
                    to = ids[i];
                }
                rc = mq_send(to, (char*)(&msg), sizeof(msg) , 0);
                if (rc < 0) {
                    perror(NULL);
                    printf("message not forwarded, msgsnd errno: %d\n", rc);
                    return 1;
                }
            }
        }
    }
    cleanup();
    return 0;
}
