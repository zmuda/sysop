#include <stdio.h>
#include <stdlib.h>
#include<sys/times.h>
#include<time.h>
#include<unistd.h>

#include "../lib/libmatrix.h"
#include "../lib/libmem.h"

void printDiagnostics() {
    Diagnostics* diags = diagnose();
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



#define SIZE 63
#define TIMES 34
int main(int argc, char **argv)
{
    printf("Rozpoczecie wykonania");
    checkpoint();

    //init(SIZE*SIZE*TIMES*3*sizeof(double));
    init(131000);
    printf("Zaalokowano bufor na %i bloków",-1);
    checkpoint();

    Macierz* a= createWithoutFilling(SIZE,SIZE);
    Macierz* b= createWithoutFilling(SIZE,SIZE);
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
        a=mul(a,b);
        dispose(tmp);
    }
    printf("Pomnozono a*b %d razy",TIMES);
    checkpoint();

    for(i=0;i<TIMES;i++){
        tmp=a;
        a=sub(a,b);
        dispose(tmp);
    }

    printf("Odjeto a-b %d razy",TIMES);
    checkpoint();

    fianlizeMemory();
    printf("Zwolniono bufor; Zakonczenie wykonania");
    previousTime=0;//nieelegancko steruje sterowaniem
    checkpoint();

    return 0;
}


