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
union sigval value;
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
        if(execl(name,name,NULL) <0){
            printf("Blad exec\n");
            return -21;
        }
    }else{
        sleep(1);
        i=N;
        while(i--){
            printf("posłano SIGRTMAX #%d\n",N-i);
            sigqueue(PID,SIGRTMAX,value);
            //sleep(1);
        }
        printf("posłano SIGRTMIN\n");
        sigqueue(PID,SIGRTMIN,value);
        while(1){}
    }
    return 11;
}
