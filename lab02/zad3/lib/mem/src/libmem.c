#include <stdio.h>
#include <stdlib.h>
#include "libmem.h"
#include "config.h"

//difolcik
#ifndef BLOCK_SIZE
#define BLOCK_SIZE 1
#endif
//kolejny difolcik
#ifndef FIRST_STRATEGY
#ifndef SMALL_STRATEGY
#ifndef BIG_STRATEGY
#define FIRST_STRATEGY
#endif
#endif
#endif



void init(unsigned long long ilosc_blokow,MemoryManager* man) {
    //MemoryManager* man = malloc(sizeof(MemoryManager));
    man->mem = malloc(ilosc_blokow*BLOCK_SIZE);
    //inicjalizacja root'ow
    man->occupiedList = malloc(sizeof(Lista));
    man->occupiedList->begin = 0;
    man->occupiedList->blocksNum = 0;
    man->occupiedList->next = 0;
    man->freeList = malloc(sizeof(Lista));
    man->freeList->begin = 0;
    man->freeList->blocksNum = 0;
    man->freeList->next = 0;
    //dodaj wolny obszar
    man->freeList->next = malloc(sizeof(Lista));
    man->freeList->next->begin = man->mem;
    man->freeList->next->blocksNum = ilosc_blokow;
    man->freeList->next->next = 0;

}
/*
void printList(Lista* list) {
    printf("\n\n--begin --\n");
    Lista* tmp = list;
    while( tmp->next!=0) {
        printf("blok o rozmiarze %u od %u\n",tmp->next->blocksNum,tmp->next->begin);
        tmp=tmp->next;
    }
    if(tmp == list) {
        printf("pusto!\n\n");
    }
    printf("--end--\n\n");
}
*/
void addToList(Lista* dodajPo,void* poczatek, unsigned long long ilosc_blokow) {
    Lista* tmp = malloc(BLOCK_SIZE*ilosc_blokow);
    tmp->next=dodajPo->next;
    tmp->begin=poczatek;
    tmp->blocksNum=ilosc_blokow;
    dodajPo->next=tmp;
}

Lista* removeFromListRaw(Lista* usunPo) {
    usunPo->next=usunPo->next->next;
}

void* allocateInFollowingBlock(Lista* tmp, size_t rozmiar,MemoryManager* man) {
    unsigned long long nowyRozmiar=rozmiar/BLOCK_SIZE;
    if(rozmiar%BLOCK_SIZE) {
        nowyRozmiar++;
    }
    if(tmp!=0 && tmp->next!=0 && (tmp->next->blocksNum)>=nowyRozmiar) {
        void* ret = tmp->next->begin;
        void* poczatek = tmp->next->begin;
        addToList(man->occupiedList, poczatek, nowyRozmiar);
        if((tmp->next->blocksNum)==nowyRozmiar) {
            removeFromListRaw(tmp);
        } else {
            tmp->next->blocksNum-=nowyRozmiar;
            tmp->next->begin+=nowyRozmiar*BLOCK_SIZE;
        }
        return ret;
    } else {
        return 0;
    }
}
void* firstSuisficientStrategyAllocation(size_t rozmiar,MemoryManager* man) {
    Lista* tmp=man->freeList;
    while( tmp->next!=0 && tmp->next->blocksNum<rozmiar) {
        tmp=tmp->next;
    }
    return allocateInFollowingBlock(tmp,rozmiar,man);
}
Lista* smallestPredecessor(size_t rozmiar,MemoryManager* man) {
    Lista* tmp=man->freeList;
    unsigned long long smallestSize = 64000;
    Lista* smallestPred = 0;
    while( tmp->next!=0) {
        unsigned long long size = tmp->next->blocksNum * BLOCK_SIZE;
        if(size>=rozmiar && size<smallestSize) {
            smallestPred=tmp;
            smallestSize=tmp->next->blocksNum;
        }
        tmp=tmp->next;
    }
    return smallestPred;
}
void* smallestBlockStrategyAllocation(size_t rozmiar,MemoryManager* man) {
    return allocateInFollowingBlock(smallestPredecessor(rozmiar,man),rozmiar,man);
}
Lista* biggestPredecesor(size_t rozmiar,MemoryManager* man) {
    Lista* tmp=man->freeList;
    unsigned long long biggestSize = 0;
    Lista* biggestPred = 0;
    while( tmp->next!=0) {
        unsigned long long size = tmp->next->blocksNum * BLOCK_SIZE;
        if(size>=rozmiar && size>biggestSize) {
            biggestPred=tmp;
            biggestSize=tmp->next->blocksNum;;
        }
        tmp=tmp->next;
    }
    return biggestPred;
}
void* biggestBlockStrategyAllocation(size_t rozmiar,MemoryManager* man) {
    return allocateInFollowingBlock(biggestPredecesor(rozmiar,man),rozmiar,man);
}

void defrag(MemoryManager* man) {
    Lista* tmp = man->freeList;
    while(tmp && tmp->next!=0) {
        Lista* tmp2 = man->freeList;
        void* begin = tmp->next->begin;
        Lista* backup = tmp->next;
        while(tmp2->next!=0) {
            void* tmp2end = tmp2->next->begin + tmp2->next->blocksNum * BLOCK_SIZE;
            if(tmp2end==begin) {
                if(tmp2end==begin) {
                    tmp2->next->blocksNum+=tmp->next->blocksNum;
                    backup=tmp;
                    removeFromListRaw(tmp);
                }
                break;
            }
            tmp2=tmp2->next;
        }
        tmp=backup;
    }
}
void* allocation(unsigned long long rozmiar,MemoryManager* man) {
    void* ret = 0;
#ifdef FIRST_STRATEGY
    ret= firstSuisficientStrategyAllocation(rozmiar,man);
#endif
#ifdef BIG_STRATEGY
    ret= biggestBlockStrategyAllocation(rozmiar,man);
#endif
#ifdef SMALL_STRATEGY
    ret= smallestBlockStrategyAllocation(rozmiar,man);
#endif
    if(!ret) {
        defrag(man);
#ifdef FIRST_STRATEGY
        ret= firstSuisficientStrategyAllocation(rozmiar,man);
#endif
#ifdef BIG_STRATEGY
        ret= biggestBlockStrategyAllocation(rozmiar,man);
#endif
#ifdef SMALL_STRATEGY
        ret= smallestBlockStrategyAllocation(rozmiar.man);
#endif
    }
    return ret;
}

void deallocation(void* wskaznik,MemoryManager* man) {
    Lista* tmp=man->occupiedList;
    while(tmp->next!=0 && tmp->next->begin!=wskaznik) {
        tmp=tmp->next;
    }
    if(tmp->next!=0 && tmp->next->begin==wskaznik) {
        addToList(man->freeList,wskaznik,tmp->next->blocksNum);
        removeFromListRaw(tmp);
    }
}
/*
void printFree() {
    printList(freeList);   //TOGO
}
void printOccupied() {
    printList(occupiedList);   //TOGO
}
*/
unsigned long long listLen(Lista* tmp) {
    unsigned long long ret =0;
    while(tmp->next!=0) {
        ret++;
        tmp=tmp->next;
    }
    return ret;
}

Diagnostics* diagnose(MemoryManager* man){
    if(man->diags==0)man->diags = malloc(sizeof(Diagnostics));
    man->diags->freeListLen=listLen(man->freeList);
    man->diags->occupiedListLen=listLen(man->occupiedList);
    Lista* tmp =biggestPredecesor(0,man);
    if(tmp){
        man->diags->biggestFree=tmp->next->blocksNum*BLOCK_SIZE;
    } else {
        man->diags->biggestFree=0;
    }
    tmp= smallestPredecessor(0,man);
    if(tmp){
        man->diags->smallestFree=tmp->next->blocksNum*BLOCK_SIZE;
    } else {
        man->diags->smallestFree=0;
    }
    return man->diags;
};
/*
void printDiagnostics() {
    diagnose();
    printf("\n\nDlugosc listy wolnych obszarow: %u",diags->freeListLen);
    printf("\nDlugosc listy zajetych obszarow: %u",diags->occupiedListLen);
    printf("\nNajwiekszy wolny: %uB",diags->biggestFree);
    printf("\nNajmniejszy wolny: %uB\n",diags->biggestFree);
}
*/
void fianlizeMemory(MemoryManager* man){
    free(man->mem);
}

