#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
int N,ctr=0;
void handler2(int x){
    int stat;
    wait(&stat);
    printf("\nKONIEC, odebrano %d z %d\n",ctr,N);
    exit(0);
}

void handler1(int x){
    printf("odebrano zwrotkę\n");
    ctr++;
}

int main(int argc, char * argv[]) {
    signal(SIGRTMAX,handler1);
    signal(SIGRTMIN,handler2);
    if (argc != 2 || (N = atoi(argv[1]))<1 ) {
        printf("Nalezy podac ilosc sygnalow!\n");
        //return 0;
        N=2;
    }
    int i;
    int PID=fork();
    if(PID==0){
        const char* name ="zad3cReciever";
        if(execve(name,NULL,NULL) <0){
            printf("Blad exec\n");
            return -21;
        }
    }else{
        sleep(1);
        i=N;
        while(i--){
            printf("posłano SIGRTMAX #%d\n",N-i);
            kill(PID,SIGRTMAX);
            sleep(1);
        }
        printf("posłano SIGRTMIN\n");
        kill(PID,SIGRTMIN);

        while(1){}
    }
    return 11;
}
