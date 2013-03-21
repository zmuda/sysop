#include <stdio.h>
#include <stdlib.h>

#define rozmiar_bloku 1

typedef struct blockListElement{
    void* begin;
    unsigned blocksNum;
    struct blockListElement* next;
} Lista;

void* mem = NULL;
Lista* freeList;
Lista* occupiedList;

void init(unsigned ilosc_blokow){
    mem = malloc(ilosc_blokow*rozmiar_bloku);
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
    printf("--begin --\n");
    Lista* tmp = list;
    while( tmp->next!=0){
        printf("blok o rozmiarze %u od %u\n",tmp->next->blocksNum,tmp->next->begin);
        tmp=tmp->next;
    }
    if(tmp == list){
        printf("pusto!\n");
    }
    printf("--end--\n");
}

void addToList(Lista* dodajPo,void* poczatek, unsigned ilosc_blokow){
    Lista* tmp = malloc(rozmiar_bloku*ilosc_blokow);
    tmp->next=dodajPo->next;
    tmp->begin=poczatek;
    tmp->blocksNum=ilosc_blokow;
    dodajPo->next=tmp;
}

Lista* removeFromListRaw(Lista* usunPo){
    usunPo->next=usunPo->next->next;
}

void* allocateInFollowingBlock(Lista* tmp, size_t rozmiar){
    if(tmp->next!=0 && (tmp->next->blocksNum*rozmiar_bloku)>=rozmiar){
        void* ret = tmp->next->begin;
        unsigned nowyRozmiar=rozmiar/rozmiar_bloku;
        if(rozmiar%rozmiar_bloku){
            nowyRozmiar++;
        }
        void* poczatek = tmp->next->begin;
        //poczatek+=(tmp->blocksNum*rozmiar_bloku);
        //printf("<< dodano od %llu>>",tmp->blocksNum);
        addToList(occupiedList, poczatek, nowyRozmiar);
        if((tmp->next->blocksNum*rozmiar_bloku)==rozmiar){
            //tmp->next=tmp->next->next;//usuniecie zajetego bloku
            removeFromListRaw(tmp);
        } else {
            tmp->next->blocksNum-=nowyRozmiar;
            tmp->next->begin+=nowyRozmiar*rozmiar_bloku;
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
        unsigned size = tmp->next->blocksNum * rozmiar_bloku;
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
        unsigned size = tmp->next->blocksNum * rozmiar_bloku;
        if(size>=rozmiar && size>biggestSize){
            biggestPredecesor=tmp;
            biggestSize=tmp->next->blocksNum;;
        }
        tmp=tmp->next;
    }
    allocateInFollowingBlock(biggestPredecesor,rozmiar);
}


void free(void* wskaznik){
    Lista* tmp=occupiedList;
    while(tmp->next!=0 && tmp->next->begin!=wskaznik){
        tmp=tmp->next;
    }
    if(tmp->next!=0 && tmp->next->begin==wskaznik){
        addToList(freeList,wskaznik,tmp->next->blocksNum);
        removeFromListRaw(tmp);
    }
}

int main(int argc, char **argv)
{
    init(100);
    void* tmp = firstSuisficientStrategyAllocation(75);
    //firstSuisficientStrategyAllocation(20);
    free(tmp);
    firstSuisficientStrategyAllocation(13);
    //smallestBlockStrategyAllocation(13);
    //biggestBlockStrategyAllocation(13);
    printList(freeList);
    printList(occupiedList);
	return 0;
}
