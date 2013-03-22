 #ifndef MEMLIB_H
 #define MEMLIB_H
 /* tutaj są wpisane deklaracje funkcji */

typedef struct blockListElement{
    void* begin;
    unsigned blocksNum;
    struct blockListElement* next;
} Lista;
void init(unsigned ilosc_blokow);
void printList(Lista* list);
void deallocation(void* wskaznik);
void* allocation(unsigned rozmiar);
void printFree();
void printOccupied();
void printDiagnostics();
 #endif
