#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <linux/sched.h>
#include <sys/wait.h>
#include <sched.h>

#define STACK_SIZE 1000
static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;
clock_t time_pot;
int licznik;
int forks;
int i,PID;
int fn(void *arg) {
    clock_t l =times(NULL);
    licznik=licznik+1;
    _exit(times(NULL)-l);
}

void start_clock(){
    st_time = times(&st_cpu);
}
void end_clock(){
    en_time = times(&en_cpu);
    double rc=((double)(time_pot))/sysconf(_SC_CLK_TCK);
    double uc=((double)(en_cpu.tms_cutime - st_cpu.tms_cutime))/sysconf(_SC_CLK_TCK);
    double sc=((double)(en_cpu.tms_cstime - st_cpu.tms_cstime))/sysconf(_SC_CLK_TCK);
    double rp=((double)(en_time - st_time))/sysconf(_SC_CLK_TCK);
    double up=((double)(en_cpu.tms_utime - st_cpu.tms_utime))/sysconf(_SC_CLK_TCK);
    double sp=((double)(en_cpu.tms_stime - st_cpu.tms_stime))/sysconf(_SC_CLK_TCK);
    FILE* fd = fopen("tmp.tmp","a+");
    fprintf(fd,"ChildCount\t%d\tRealTimeSum\t%.2f\tUserTimeSum\t%.2f\tSystemTimeSum\t%.2f\tSys+UsTimeSum\t%.2f\tRealTimeChild\t%.2f\tUserTimeChild\t%.2f\tSystemTimeChild\t%.2f\tSys+UsTimeCh\t%.2f\tRealTimeParent\t%.2f\tUserTimeParent\t%.2f\tSystemTimePar.\t%.2f\tSys+UsTimePar\t%.2f\n",
        forks,rc+rp,uc+up,sc+sp,uc+up+sc+sp,rc,uc,sc,uc+sc,rp,up,sp,up+sp);
    fclose(fd);
}
int main(int argc, char **argv)
{
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
    assert(childstack);
    start_clock();
    for(i=0;i<forks;i++){
#ifdef FORK
        PID = fork();
        if (PID == 0) {
            fn(NULL);
        } else
#else
#ifdef VFORK
        PID = vfork();
        if (PID == 0) {
            fn(NULL);
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
            time_pot+= WEXITSTATUS(b);
        }
    }
    end_clock("");
    printf("Counter: %d\n",licznik);
    return 0;
}
