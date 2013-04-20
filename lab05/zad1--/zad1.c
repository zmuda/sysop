#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>

#define ACTION
time_t stamp;
int pid;
void signal_callback_handler(int signum) {
    int diff= time(NULL)-stamp;
    printf(" # <%d> caught signal %d - lifetime = %ds\n",getpid(),signum,diff);
    _exit(diff);
}
void signal_callback_handler_parent(int signum) {
    int b = -1;
    pid_t pid = wait(&b);
    printf("-> <%d> dead after %ds\n",pid,b/256);
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
int main() {
#ifdef ACTION
    handler = malloc(sizeof(struct sigaction));
    parenthandler = malloc(sizeof(struct sigaction));
    handler->sa_handler=signal_callback_handler;
    parenthandler->sa_handler=signal_callback_handler_parent;
    sigemptyset(&(handler->sa_mask));
    sigemptyset(&(parenthandler->sa_mask));
    sigaction(SIGCHLD,parenthandler,NULL);
#else
    signal(SIGCHLD, signal_callback_handler_parent);
#endif
    forks =3;
    pids = malloc(sizeof(int)*forks);
    int i;
    for(i=0; i<forks; i++) {
        create(i);
    }
    while(1) {
        sleep(1);
        int i =rand()%forks;
        kill(pids[i],SIGTSTP);
        create(i);
        sleep(1);
    }
    free(pids);
    return 0;
}
