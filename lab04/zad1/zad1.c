#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <linux/sched.h>

#define STACK_SIZE 1000
static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;
int i,PID,licznik = 0;
int fn(void *arg) {

    licznik++;
    _exit(0);
    return 0;

}

void start_clock(){
    st_time = times(&st_cpu);
}
void end_clock(){
    en_time = times(&en_cpu);
    printf("Real Time: %.2f, User Time %.2f, System Time %.2f\n",
        ((double)(en_time - st_time))/sysconf(_SC_CLK_TCK),
        ((double)(en_cpu.tms_utime - st_cpu.tms_utime))/sysconf(_SC_CLK_TCK),
        ((double)(en_cpu.tms_stime - st_cpu.tms_stime))/sysconf(_SC_CLK_TCK));
}

int main(int argc, char **argv)
{
    int forks;
    if(argc<2){
        printf(" number of forks missing\n");
        return 1;
    }
    forks = atoi(argv[1]);
    if(forks<1){
        printf(" wrong number of forks\n");
        return 2;
    }
    char *childstack = malloc(STACK_SIZE);
    start_clock();
    for(i=0;i<forks;i++){
#ifdef FORK
        PID = fork();
        if (PID == 0) {
            licznik++;
            _exit(0);
        } else
#else
#ifdef VFORK
        PID = vfork();
        if (PID == 0) {
            licznik++;
            _exit(0);
        } else
#else
#ifdef CLONE
        PID = clone(fn, childstack + STACK_SIZE, SIGCHLD, NULL);
#else
#ifdef VCLONE
        PID = clone(fn, childstack + STACK_SIZE, SIGCHLD | CLONE_VM | CLONE_VFORK, NULL);
#else
        printf("Compilated without forking method specification \n");
        return 7;
#endif
#endif
#endif
#endif
        {
            assert(PID>-1);
            int b = -1;
            wait(&b);
        }
    }
    end_clock("");
    return 0;
}
