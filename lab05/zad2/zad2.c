#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include "config.h"
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

time_t stamp;
int pid;
int value;
void signal_callback_handler(int signum) {
    int diff= time(NULL)-stamp;
    printf(" # <%d> caught signal %d - lifetime = %ds\n",getpid(),signum,diff);
    _exit(diff);
}

void signal_callback_handler_queued(int signum, siginfo_t* info, void* context) {
    int diff= time(NULL)-stamp;
    printf(" # <%d> caught signal %d - lifetime = %ds\n",getpid(),signum,diff);
    if(info->si_code == SI_QUEUE){
		printf("\tValue=%d\tsenderUID=%d\tsenderPID=%d\n",info->si_value.sival_int,info->si_uid,info->si_pid);
        exit(info->si_value.sival_int);
    }
    _exit(diff);
}
void signal_callback_handler_parent(int signum) {
    int b = -1;
    pid_t pid = wait(&b);
#ifdef ACTION
    printf("-> <%d> had value  %d\n",pid,b/256);
#else
    printf("-> <%d> dead after %ds\n",pid,b/256);
#endif
}
int forks;
int* pids;
struct sigaction *handler,*parenthandler;
void create(int i) {
    int PID = fork();
    if (PID == 0) {
        stamp=time(NULL);
        pid=getpid();
#ifdef ACTION
        sigaction(SIGTSTP,handler,NULL);
#else
        signal(SIGTSTP, signal_callback_handler);
#endif
        while(1) {
            printf("alive: %d\n",pid);
            sleep(2);
        }
    } else {
        pids[i]=PID;
    }
}
int main(int argc, char** argv) {
#ifdef ACTION
    if(argc<3 || (forks =atoi(argv[1]))<1 || (value =atoi(argv[2]))<0 || (value)>127){
        printf("usage: [# of childern (1+)] [value]\n");
        return 1;
    }
    handler = malloc(sizeof(struct sigaction));
    parenthandler = malloc(sizeof(struct sigaction));
    handler->sa_sigaction=signal_callback_handler_queued;
    parenthandler->sa_handler=signal_callback_handler_parent;
    sigemptyset(&(handler->sa_mask));
    sigemptyset(&(parenthandler->sa_mask));
    sigaction(SIGCHLD,parenthandler,NULL);
    handler->sa_flags=SI_QUEUE;
    union sigval sigvalue;
    sigvalue.sival_int=value;
#else
    if(argc<2 || (forks =atoi(argv[1]))<1){
        printf("usage: [# of childern (1+)]\n");
        return 11;
    }
    signal(SIGCHLD, signal_callback_handler_parent);
#endif
    pids = malloc(sizeof(int)*forks);
    int i;
    for(i=0; i<forks; i++) {
        create(i);
    }
    while(1) {
        sleep(1);
        int i =rand()%forks;
#ifdef ACTION
        sigqueue(pids[i],SIGTSTP,sigvalue);
        sigvalue.sival_int++;
#else
        kill(pids[i],SIGTSTP);
#endif
        create(i);
        sleep(1);
    }
    free(pids);
#ifdef ACTION
    free(handler);
    free(parenthandler);
#endif
    return 0;
}
