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
typedef struct manager{
    void* mem;
    Lista* freeList;
    Lista* occupiedList;
    Diagnostics* diags;
} MemoryManager;
void init(unsigned long long ilosc_blokow,MemoryManager* man);
//void printList(Lista* list);
void deallocation(void* wskaznik,MemoryManager* man);
void* allocation(unsigned long long rozmiar,MemoryManager* man);
//void printFree();
//void printOccupied();
Diagnostics* diagnose(MemoryManager* man);
void fianlizeMemory(MemoryManager* man);

#endif
