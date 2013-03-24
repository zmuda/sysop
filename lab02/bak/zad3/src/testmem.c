#include <stdio.h>
#include <stdlib.h>
#include<sys/times.h>
#include<time.h>
#include<unistd.h>

#define DYNAMIC 0
#ifdef DYNAMIC
#include<dlfcn.h>
#define diagnose (*diagnosRuntime)
#endif

#include "../lib/libmatrix.h"
#include "../lib/libmem.h"


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




#define SIZE 100000
int main(int argc, char **argv)
{
    /*
#define DYNAMIC 0
#ifdef DYNAMIC
    void* libka = dlopen("../lib/lmem.so", RTLD_LAZY);
    typedef MemoryManager* (*type1)(unsigned long long);
    type1 iniRuntime = (type1)dlsym(libka,"init");
    typedef void (*type2)(void*,MemoryManager*);
    type2 deallocatioRuntime = (type2)dlsym(libka,"deallocation");
    typedef void* (*type3)(unsigned long long,MemoryManager*);
    type3 allocatioRuntime = (type3)dlsym(libka,"allocation");
    typedef Diagnostics* (*type4)(MemoryManager*);
    type4 diagnosRuntime = (type4)dlsym(libka,"diagnose");
    typedef void (*type0)(MemoryManager*);
    type0 finalizeMemorRuntime = (type0)dlsym(libka,"dinalizeMemory");
#define fianlizeMemory (*finalizeMemorRuntime)
#define diagnose (*diagnosRuntime)
#define deallocation (*deallocatioRuntime)
#define allocation (*allocatioRuntime)
//#define printDiagostics (*printDiagosticRuntime)
#define init (*iniRuntime)
#endif
*/
    printf("Rozpoczecie wykonania");
    checkpoint();

    man = init(SIZE);
    printf("Zaalokowano bufor na %i bloków",SIZE);
    checkpoint();

    int** tmp = malloc(sizeof(int*)*SIZE);
    int i=0;
    for(i=0;i<SIZE-sizeof(Diagnostics);i++){
        tmp[i]=allocation(1);
    }
    printf("Zapelniono w duzym stopniu bufor fragmentami o rozmiarze jednego bloku");
    checkpoint();

    for(i=0;i<SIZE-sizeof(Diagnostics);i++){
        deallocation(tmp[i]);
    }
    printf("Zwolniono przestrzen bufor zajeta przez uzytkownika");
    checkpoint();

    allocation(200);
    printf("Zdefragmentowano w wyniku alokacji duzego bloku");
    checkpoint();

    fianlizeMemory(man);
    printf("Zwolniono bufor; Zakonczenie wykonania");
    previousTime=0;//nieelegancko steruje sterowaniem
    checkpoint();

    return 0;
}