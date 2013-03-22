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

void* mem = NULL;
Lista* freeList;
Lista* occupiedList;

void init(unsigned ilosc_blokow) {
    mem = malloc(ilosc_blokow*BLOCK_SIZE);
    //inicjalizacja root'ow
    occupiedList = malloc(sizeof(Lista));
    occupiedList->begin = 0;
    occupiedList->blocksNum = 0;
    occupiedList->next = 0;
    freeList = malloc(sizeof(Lista));
    freeList->begin = 0;
    freeList->blocksNum = 0;
    freeList->next = 0;
    //dodaj wolny obszar
    freeList->next = malloc(sizeof(Lista));
    freeList->next->begin = mem;
    freeList->next->blocksNum = ilosc_blokow;
    freeList->next->next = 0;

}

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

void addToList(Lista* dodajPo,void* poczatek, unsigned ilosc_blokow) {
    Lista* tmp = malloc(BLOCK_SIZE*ilosc_blokow);
    tmp->next=dodajPo->next;
    tmp->begin=poczatek;
    tmp->blocksNum=ilosc_blokow;
    dodajPo->next=tmp;
}

Lista* removeFromListRaw(Lista* usunPo) {
    usunPo->next=usunPo->next->next;
}

void* allocateInFollowingBlock(Lista* tmp, size_t rozmiar) {
    if(tmp!=0 && tmp->next!=0 && (tmp->next->blocksNum*BLOCK_SIZE)>=rozmiar) {
        void* ret = tmp->next->begin;
        unsigned nowyRozmiar=rozmiar/BLOCK_SIZE;
        if(rozmiar%BLOCK_SIZE) {
            nowyRozmiar++;
        }
        void* poczatek = tmp->next->begin;
        addToList(occupiedList, poczatek, nowyRozmiar);
        if((tmp->next->blocksNum*BLOCK_SIZE)==rozmiar) {
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
void* firstSuisficientStrategyAllocation(size_t rozmiar) {
    Lista* tmp=freeList;
    while( tmp->next!=0 && tmp->next->blocksNum<rozmiar) {
        tmp=tmp->next;
    }
    return allocateInFollowingBlock(tmp,rozmiar);
}
Lista* smallestPredecessor(size_t rozmiar) {
    Lista* tmp=freeList;
    unsigned smallestSize = 64000;
    Lista* smallestPred = 0;
    while( tmp->next!=0) {
        unsigned size = tmp->next->blocksNum * BLOCK_SIZE;
        if(size>=rozmiar && size<smallestSize) {
            smallestPred=tmp;
            smallestSize=tmp->next->blocksNum;
        }
        tmp=tmp->next;
    }
    return smallestPred;
}
void* smallestBlockStrategyAllocation(size_t rozmiar) {
    return allocateInFollowingBlock(smallestPredecessor(rozmiar),rozmiar);
}
Lista* biggestPredecesor(size_t rozmiar) {
    Lista* tmp=freeList;
    unsigned biggestSize = 0;
    Lista* biggestPred = 0;
    while( tmp->next!=0) {
        unsigned size = tmp->next->blocksNum * BLOCK_SIZE;
        if(size>=rozmiar && size>biggestSize) {
            biggestPred=tmp;
            biggestSize=tmp->next->blocksNum;;
        }
        tmp=tmp->next;
    }
    return biggestPred;
}
void* biggestBlockStrategyAllocation(size_t rozmiar) {
    return allocateInFollowingBlock(biggestPredecesor(rozmiar),rozmiar);
}

void defrag() {
    Lista* tmp = freeList;
    while(tmp && tmp->next!=0) {
        Lista* tmp2 = freeList;
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
void* allocation(unsigned rozmiar) {
    void* ret = 0;
#ifdef FIRST_STRATEGY
    ret= firstSuisficientStrategyAllocation(rozmiar);
#endif
#ifdef BIG_STRATEGY
    ret= biggestBlockStrategyAllocation(rozmiar);
#endif
#ifdef SMALL_STRATEGY
    ret= smallestBlockStrategyAllocation(rozmiar);
#endif
    if(!ret) {
        defrag();
#ifdef FIRST_STRATEGY
        ret= firstSuisficientStrategyAllocation(rozmiar);
#endif
#ifdef BIG_STRATEGY
        ret= biggestBlockStrategyAllocation(rozmiar);
#endif
#ifdef SMALL_STRATEGY
        ret= smallestBlockStrategyAllocation(rozmiar);
#endif
    }
    return ret;
}

void deallocation(void* wskaznik) {
    Lista* tmp=occupiedList;
    while(tmp->next!=0 && tmp->next->begin!=wskaznik) {
        tmp=tmp->next;
    }
    if(tmp->next!=0 && tmp->next->begin==wskaznik) {
        addToList(freeList,wskaznik,tmp->next->blocksNum);
        removeFromListRaw(tmp);
    }
}
void printFree() {
    printList(freeList);   //TOGO
}
void printOccupied() {
    printList(occupiedList);   //TOGO
}

unsigned listLen(Lista* tmp) {
    unsigned ret =0;
    while(tmp->next!=0) {
        ret++;
        tmp=tmp->next;
    }
    return ret;
}
Diagnostics* diags = 0;
Diagnostics* diagnose(){
    if(diags==0)diags = allocation(sizeof(Diagnostics));
    diags->freeListLen=listLen(freeList);
    diags->occupiedListLen=listLen(occupiedList);
    diags->biggestFree=biggestPredecesor(0)->next->blocksNum*BLOCK_SIZE;
    diags->smallestFree=smallestPredecessor(0)->next->blocksNum*BLOCK_SIZE;
    return diags;
};

void printDiagnostics() {
    diagnose();
    printf("\n\nDlugosc listy wolnych obszarow: %u",diags->freeListLen);
    printf("\nDlugosc listy zajetych obszarow: %u",diags->occupiedListLen);
    printf("\nNajwiekszy wolny: %uB",diags->biggestFree);
    printf("\nNajmniejszy wolny: %uB\n",diags->biggestFree);
}

void fianlizeMemory(){
    free(mem);
}
