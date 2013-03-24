 #ifndef MEMLIB_H
 #define MEMLIB_H
 /* tutaj są wpisane deklaracje funkcji */
typedef struct diagnostics{
    unsigned long long freeListLen;
    unsigned long long occupiedListLen;
    unsigned long long biggestFree;
    unsigned long long smallestFree;
} Diagnostics;
typedef struct blockListElement{
    void* begin;
    unsigned long long blocksNum;
    struct blockListElement* next;
} Lista;
void init(unsigned long long ilosc_blokow);
void printList(Lista* list);
void deallocation(void* wskaznik);
void* allocation(unsigned long long rozmiar);
//void printFree();
//void printOccupied();
Diagnostics* diagnose();
void fianlizeMemory();

#endif
