#include <stdio.h>
#include <stdlib.h>

//difolcik
#ifndef BLOCK_SIZE
    #define BLOCK_SIZE 1
#endif

typedef struct blockListElement{
    void* begin;
    unsigned blocksNum;
    struct blockListElement* next;
} Lista;

void* mem = NULL;
Lista* freeList;
Lista* occupiedList;

void init(unsigned ilosc_blokow){
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

void printList(Lista* list){
    printf("\n\n--begin --\n");
    Lista* tmp = list;
    while( tmp->next!=0){
        printf("blok o rozmiarze %u od %u\n",tmp->next->blocksNum,tmp->next->begin);
        tmp=tmp->next;
    }
    if(tmp == list){
        printf("pusto!\n\n");
    }
    printf("--end--\n\n");
}

void addToList(Lista* dodajPo,void* poczatek, unsigned ilosc_blokow){
    Lista* tmp = malloc(BLOCK_SIZE*ilosc_blokow);
    tmp->next=dodajPo->next;
    tmp->begin=poczatek;
    tmp->blocksNum=ilosc_blokow;
    dodajPo->next=tmp;
}

Lista* removeFromListRaw(Lista* usunPo){
    usunPo->next=usunPo->next->next;
}

void* allocateInFollowingBlock(Lista* tmp, size_t rozmiar){
    if(tmp->next!=0 && (tmp->next->blocksNum*BLOCK_SIZE)>=rozmiar){
        void* ret = tmp->next->begin;
        unsigned nowyRozmiar=rozmiar/BLOCK_SIZE;
        if(rozmiar%BLOCK_SIZE){
            nowyRozmiar++;
        }
        void* poczatek = tmp->next->begin;
        addToList(occupiedList, poczatek, nowyRozmiar);
        if((tmp->next->blocksNum*BLOCK_SIZE)==rozmiar){
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
void* firstSuisficientStrategyAllocation(size_t rozmiar){
    Lista* tmp=freeList;
    while( tmp->next!=0 && tmp->next->blocksNum<rozmiar){
        tmp=tmp->next;
    }
    return allocateInFollowingBlock(tmp,rozmiar);
}
void* smallestBlockStrategyAllocation(size_t rozmiar){
    Lista* tmp=freeList;
    unsigned smallestSize = 64000;
    Lista* smallestPredecesor = 0;
    while( tmp->next!=0){
        unsigned size = tmp->next->blocksNum * BLOCK_SIZE;
        if(size>=rozmiar && size<smallestSize){
            smallestPredecesor=tmp;
            smallestSize=tmp->next->blocksNum;
        }
        tmp=tmp->next;
    }
    return allocateInFollowingBlock(smallestPredecesor,rozmiar);
}
void* biggestBlockStrategyAllocation(size_t rozmiar){
    Lista* tmp=freeList;
    unsigned biggestSize = 0;
    Lista* biggestPredecesor = 0;
    while( tmp->next!=0){
        unsigned size = tmp->next->blocksNum * BLOCK_SIZE;
        if(size>=rozmiar && size>biggestSize){
            biggestPredecesor=tmp;
            biggestSize=tmp->next->blocksNum;;
        }
        tmp=tmp->next;
    }
    allocateInFollowingBlock(biggestPredecesor,rozmiar);
}


void deallocation(void* wskaznik){
    Lista* tmp=occupiedList;
    while(tmp->next!=0 && tmp->next->begin!=wskaznik){
        tmp=tmp->next;
    }
    if(tmp->next!=0 && tmp->next->begin==wskaznik){
        addToList(freeList,wskaznik,tmp->next->blocksNum);
        removeFromListRaw(tmp);
    }
}
#define SMALL_STRATEGY
#define FIRST_STRATEGY
void* allocation(size_t rozmiar){
#ifdef FIRST_STRATEGY
    return firstSuisficientStrategyAllocation(rozmiar);
#endif
#ifdef BIG_STRATEGY
    return biggestBlockStrategyAllocation(rozmiar);
#endif
#ifdef SMALL_STRATEGY
    return smallestBlockStrategyAllocation(rozmiar);
#endif
}
int main(int argc, char **argv)
{
    init(100);
    void* tmp = allocation(75);
    //firstSuisficientStrategyAllocation(20);
    deallocation(tmp);
    allocation(13);

    printList(freeList);
    printList(occupiedList);
	return 0;
}
