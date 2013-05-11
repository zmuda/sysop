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
int limit;
int out;
char** files;
mqd_t* ids;
int ids_size=0;

/**
* pomocnicza - sprzata kolejke i plik (nie gromadza sie komunikaty przypadkiem)
*/
void closeQueue(char* name,mqd_t queue_id){
    mq_close(queue_id);
	unlink(name);
}

/**
* zestaw funkcji porzadkujacych
*/
void clean1(){
    closeQueue(QUEUENAME,queue_id);
}
void clean2(int i){
    clean1();
    exit(0);
}

mqd_t findit(char* name){
    int i=0;
    for(;i<ids_size;i++){
        if(!strcmp(name,files[i]))return ids[i];
    }
    return -1;
}

char buff[512];
int persist_it(message msg){
    time_t t;
    time(&t);
    sprintf(buff,"%s\t%s\n%s\n\n",ctime(&t),msg.who,msg.what);
    int thissize=strlen(buff);
    if(limit-thissize<0)return 0;
    write(out,buff,strlen(buff));
    limit-=thissize;
    return 1;
}

int main(int argc, char** argv){
    limit=100;
    /** otwieramy arbitralnie zadana kolejke */
    mqd_t queue_id = createQueue(QUEUENAME,sizeof(message));
    out=open("tmp/LOG.log",O_WRONLY|O_CREAT);

    /** nibymapa */
    files=malloc(LIMIT*sizeof(char*));
    ids=malloc(LIMIT*sizeof(mqd_t));
    ids_size=0;

    /** czyszczeniw na wyjsciu */
    atexit(clean1);
    int rc;
    while(1){
        /** odbior komunikatu nt. pojawienia sie klienta
        *   nie czekamy, musimy jeszcze obslugiwac transfer wiadomosci
        */
        message msg;
        rc = mq_receive(queue_id, (char*)(&msg), sizeof(msg), NULL);
        if(rc>=0){
            int ok = persist_it(msg);
            printf("\nRECIEVED:\t%s\n%s\n",msg.who,msg.what);
            if(!ok)printf("BUT NOT SAVED\n");
            printf("pid: %d\n",msg.pid);
            if(ok)kill(msg.pid,SIGUSR1);else kill(msg.pid,SIGUSR2);
            //pause();
            printf("LIMIT=%d\n",limit);
        }
    }
    return 0;
}
