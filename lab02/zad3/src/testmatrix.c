#include <stdio.h>
#include <stdlib.h>
#include<sys/times.h>
#include<time.h>
#include<unistd.h>
#include "../lib/libmatrix.h"
#include "../lib/libmem.h"
MemoryManager* man;

#ifdef DYNAMIC
#include<dlfcn.h>
#define zeros zeroRuntime
#define inputs inputRuntime
#define add adRuntime
#define dispose disposRuntime
#define sub suRuntime
#define mul muRuntime
#define printMatrix printMatriRuntime
#define createWithoutFilling createWithoutFillinRuntime
/**/
    typedef Macierz* (*typeA)(unsigned, unsigned, MemoryManager*);
    typeA zeroRuntime =0;
    typeA inputRuntime =0;
    typeA createWithoutFillinRuntime =0;
    typedef void (*typeB)(Macierz*);
    typeB printMatriRuntime =0;
    typedef void (*typeC)(Macierz*,MemoryManager*);
    typeC disposRuntime =0;
    typedef Macierz* (*typeD)(Macierz*,Macierz*, MemoryManager*);
    typeD adRuntime =0;
    typeD muRuntime =0;
    typeD suRuntime =0;

#endif

void printDiagnostics() {
    Diagnostics* diags = diagnose(man);
    printf("\n\tDlugosc listy wolnych obszarow: %u",diags->freeListLen);
    printf("\n\tDlugosc listy zajetych obszarow: %u",diags->occupiedListLen);
    printf("\n\tNajwiekszy wolny: %uB",diags->biggestFree);
    printf("\n\tNajmniejszy wolny: %uB",diags->biggestFree);
}

struct tms* previousTime = 0;
clock_t previousReal = 0;
struct tms firstTime;
clock_t firstReal = 0;
#define CLK sysconf(_SC_CLK_TCK)
void checkpoint(){
    struct tms now;
    times(&now);
    clock_t nowReal = clock();
    if(!previousTime){
        firstReal=nowReal;
        firstTime=now;
    } else {
    printDiagnostics();
    printf("\n\tOd pierwszego:\t\tR %.2f\tS %.2f\tU %.2f",
           ((double)(nowReal-firstReal))/CLOCKS_PER_SEC,
           ((double)(now.tms_stime-firstTime.tms_stime))/CLK,
           ((double)(now.tms_utime-firstTime.tms_utime))/CLK);

    printf("\n\tOd poprzedniego:\tR %.2f\tS %.2f\tU %.2f",
           ((double)(nowReal-previousReal))/CLOCKS_PER_SEC,
           ((double)(now.tms_stime-previousTime->tms_stime))/CLK,
           ((double)(now.tms_utime-previousTime->tms_utime))/CLK);

    }
    printf("\n\tCzas:\t\t\tR %.2f\tS %.2f\tU %.2f\n\n\n",
           ((double)nowReal)/CLOCKS_PER_SEC,
           ((double)now.tms_stime)/CLK,
           ((double)now.tms_utime)/CLK);
    previousTime=&now;
    previousReal=nowReal;
}



#define SIZE 120
#define TIMES 340
int main(int argc, char **argv)
{

#ifdef DYNAMIC
    void* libka0 = dlopen("lib/libmatrix.so", RTLD_LAZY);
    if(!libka0)printf("\n%s\n", dlerror());
    typeA zeroRuntime=(typeA)dlsym(libka0,"zeros");
    if(!zeroRuntime)printf("\n%s\n", dlerror());
    typeA inputRuntime=(typeA)dlsym(libka0,"inputs");
    typeA createWithoutFillinRuntime=(typeA)dlsym(libka0,"createWithoutFilling");
    typeB printMatriRuntime=(typeB)dlsym(libka0,"printMatrix");
    typeC disposRuntime=(typeC)dlsym(libka0,"dispose");
    typeD adRuntime=(typeD)dlsym(libka0,"add");
    typeD muRuntime=(typeD)dlsym(libka0,"mul");
    typeD suRuntime=(typeD)dlsym(libka0,"sub");
#endif

    man = malloc(sizeof(MemoryManager*));
    man->diags=malloc(sizeof(Diagnostics));
    printf("Rozpoczecie wykonania");
    checkpoint();

    init(SIZE*SIZE*3*sizeof(double),man);
    printf("Zaalokowano bufor na %i bloków",SIZE*SIZE*TIMES*3*sizeof(double));
    checkpoint();

    Macierz* a= createWithoutFilling(SIZE,SIZE,man);
    Macierz* b= createWithoutFilling(SIZE,SIZE,man);
    Macierz* tmp;
    printf("Zaalokowano dwie macierze kwadratowe o rozmiarze %d wypelnione zerami",SIZE);
    checkpoint();

    int i;
    for(i=0;i<SIZE*SIZE;i++){
        a->tab[i/SIZE][i%SIZE]=i;
        b->tab[i/SIZE][i%SIZE]=-i;
    }
    printf("Wypelniono macierze kolejnumi liczbami naturalnymi; w jednej macierzy ujemne");
    checkpoint();


    for(i=0;i<TIMES;i++){
        tmp=a;
        a=mul(a,b,man);
        dispose(tmp,man);
    }
    printf("Pomnozono a*b %d razy",TIMES);
    checkpoint();

    for(i=0;i<TIMES;i++){
        tmp=a;
        a=sub(a,b,man);
        dispose(tmp,man);
    }

    printf("Odjeto a-b %d razy",TIMES);
    checkpoint();

    fianlizeMemory(man);
    printf("Zwolniono bufor; Zakonczenie wykonania");
    previousTime=0;//nieelegancko steruje sterowaniem
    checkpoint();
/**/
    return 0;
}


