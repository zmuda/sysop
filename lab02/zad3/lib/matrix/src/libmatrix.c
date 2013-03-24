#include <stdio.h>
#include <stdlib.h>
#include "libmatrix.h"
#include "../lib/mem/src/libmem.h"

//#define DYNAMIC 0
#ifdef DYNAMIC
#include<dlfcn.h>
#define diagnose (*diagnosRuntime)
#define fianlizeMemory (*finalizeMemorRuntime)
#define diagnose (*diagnosRuntime)
#define deallocation (*deallocatioRuntime)
#define allocation (*allocatioRuntime)
#define init (*iniRuntime)
    typedef MemoryManager* (*type1)(unsigned long long,MemoryManager*);
    type1 iniRuntime;
    typedef void (*type2)(void*,MemoryManager*);
    type2 deallocatioRuntime;
    typedef void* (*type3)(unsigned long long,MemoryManager*);
    type3 allocatioRuntime;
    typedef Diagnostics* (*type4)(MemoryManager*);
    type4 diagnosRuntime ;
    typedef void (*type0)(MemoryManager*);
    type0 finalizeMemorRuntime;
#endif


Macierz* createWithoutFilling(unsigned width, unsigned height, MemoryManager* man){

    #ifdef DYNAMIC
    int boolean = 0;
    if(boolean){
        void* libka = dlopen("lib/mem/src/.libs/libmem.so", RTLD_LAZY);
        iniRuntime = (type1)dlsym(libka,"init");
        deallocatioRuntime = (type2)dlsym(libka,"deallocation");
        allocatioRuntime = (type3)dlsym(libka,"allocation");
        diagnosRuntime = (type4)dlsym(libka,"diagnose");
        finalizeMemorRuntime = (type0)dlsym(libka,"fianlizeMemory");
        boolean++;
    }
    #endif

    Macierz* ret = allocation(sizeof(Macierz),man);
    ret->tab = allocation(height*sizeof(Macierz*),man);
    int i;
    for(i=0;i<height;i++){
        ret->tab[i]=allocation(width*sizeof(double),man);
    }
    ret->width=width;
    ret->height=height;
    return ret;
}
Macierz* zeros(unsigned width, unsigned height, MemoryManager* man){
    Macierz* ret = createWithoutFilling(width,height,man);
    int i,j;
    for(i=0;i<width;i++){
        for(j=0;j<height;j++){
            ret->tab[j][i]=.0;
        }
    }
    return ret;
}
Macierz* inputs(unsigned width, unsigned height, MemoryManager* man){
    Macierz* ret = createWithoutFilling(width,height,man);
    int i,j;
    for(i=0;i<width;i++){
        for(j=0;j<height;j++){
            printf("\n[%d][%d]=",i,j);
            double tmp;
            scanf("%lf",&tmp);
            ret->tab[j][i]=tmp;
        }
    }
    return ret;
}
void printMatrix(Macierz* m){
    printf("\n");
    int i,j;
    for(i=0;i<m->height;i++){
        for(j=0;j<m->width;j++){
            printf("%f\t",m->tab[i][j]);
        }
        printf("\n");
    }
}
void inc(Macierz* a,Macierz* m){
    int i,j;
    for(i=0;i<m->height;i++){
        for(j=0;j<m->width;j++){
            a->tab[i][j]+=m->tab[i][j];
        }
    }
}
void dec(Macierz* a,Macierz* m){
    int i,j;
    for(i=0;i<m->height;i++){
        for(j=0;j<m->width;j++){
            a->tab[i][j]-=m->tab[i][j];
        }
    }
}
Macierz* add(Macierz* a,Macierz* b, MemoryManager* man){
    unsigned width = (a->width>b->width)?(a->width):(b->width);
    unsigned height = (a->height>b->height)?(a->height):(b->height);
    Macierz* ret = zeros(width,height,man);
    inc(ret,a);
    inc(ret,b);
    return(ret);
}

Macierz* sub(Macierz* a,Macierz* b, MemoryManager* man){
    unsigned width = (a->width>b->width)?(a->width):(b->width);
    unsigned height = (a->height>b->height)?(a->height):(b->height);
    Macierz* ret = zeros(width,height,man);
    inc(ret,a);
    dec(ret,b);
    return(ret);
}


Macierz* mul(Macierz* a,Macierz* b, MemoryManager* man){
    if(a->width!=b->height || a->height!=b->width)return 0;
    Macierz* ret = zeros(a->height,a->height,man);
    int i,j,k;
    for(i=0;i<a->height;i++){
        for(j=0;j<a->height;j++){
            for(k=0;k<a->width;k++){
                ret->tab[i][j]+=a->tab[i][k]*b->tab[k][j];
            }
        }
    }
    return(ret);
}

void dispose(Macierz* m, MemoryManager* man){
    int i;
    for(i=0;i<m->height;i++){
        deallocation(m->tab[i],man);
    }
    deallocation(m,man);
}

