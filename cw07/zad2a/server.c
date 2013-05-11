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
#include <sys/times.h>
#include "defs.h"

int queue_id;
int limit;
FILE* out;
char buff[512];
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
    fclose(out);
}
void clean2(int i){
    clean1();
    exit(0);
}
int persist_it(message msg){
    time_t t;
    time(&t);
    sprintf(buff,"%s\t%s\n%s\n\n",ctime(&t),msg.who,msg.what);
    int thissize=strlen(buff);
    if(limit-thissize<0)return 0;
    fprintf(out,"%s",buff);
    fflush(out);
    limit-=thissize;
    return 1;
}

int main(int argc, char** argv){
    limit=100;
    out=fopen("tmp/LOG.log","w");
    /** otwieramy arbitralnie zadana kolejke */
    queue_id = createQueue("tmp/servers.rip");
    /** czyszczeniw na wyjsciu */
    atexit(clean1);
    //signal(SIGINT,clean2);
    int rc;
    while(1){
        message buff;
        sleep(1);
        /** odbior komunikatu nt. pojawienia sie klienta
        *   nie czekamy, musimy jeszcze obslugiwac transfer wiadomosci
        */
        rc = msgrcv(queue_id, &buff, sizeof(buff), CLIENTS, IPC_NOWAIT);
        if(rc>=0){
            nack x;
            x.mtype=SERVERS;
            x.ok=persist_it(buff);
            printf("\nRECIEVED:\t%s\n%s\n",buff.who,buff.what);
            if(!x.ok)printf("BUT NOT SAVED\n");
            rc = msgsnd(queue_id, &x, sizeof(x), 0);
            if (rc < 0) {
                printf("id not send, msgsnd errrno: %d\n", rc);
                return 1;
            }
            printf("LIMIT=%d\n",limit);
        }

    }

    return 0;
}
