#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
int N,ctr=0;
int lock=0;
void handler2(int x){
    int stat;
    wait(&stat);
    printf("\nKONIEC, odebrano %d z %d\n",ctr,N);
    exit(0);
}

void handler1(int x){
    printf("odebrano zwrotkę\n");
    lock =0;
    ctr++;
}

int main(int argc, char * argv[]) {
    signal(SIGUSR1,handler1);
    signal(SIGUSR2,handler2);
    if (argc != 2 || (N = atoi(argv[1]))<1 ) {
        printf("Nalezy podac ilosc sygnalow!\n");
        //return 0;
        N=2;
    }
    int i;
    int PID=fork();
    if(PID==0){
        const char* name ="zad3bReciever";
        if(execve(name,NULL,NULL) <0)printf("Blad exec\n");
    }else{
        sleep(1);
        i=N;
        while(i--){
            if(lock){
                i++;
            } else {
                printf("posłano SIGUSR1 #%d\n",N-i);
                kill(PID,SIGUSR1);
                lock=1;
            }
            sleep(1);
        }
        printf("posłano SIGUSR2\n");
        kill(PID,SIGUSR2);

        while(1){}
    }
    return 11;
}
