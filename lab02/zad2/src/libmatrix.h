 #ifndef MATLIB_H
 #define MATLIB_H
 #include "../lib/libmem.h"
 /* tutaj są wpisane deklaracje funkcji */
 typedef struct matrix{
    unsigned width;
    unsigned height;
    double** tab;
} Macierz;
Macierz* zeros(unsigned width, unsigned height, MemoryManager* man);
Macierz* inputs(unsigned width, unsigned height, MemoryManager* man);
void printMatrix(Macierz* m);
Macierz* createWithoutFilling(unsigned width, unsigned height, MemoryManager* man);
void dispose(Macierz* m, MemoryManager* man);
Macierz* sub(Macierz* a,Macierz* b, MemoryManager* man);
Macierz* mul(Macierz* a,Macierz* b, MemoryManager* man);
Macierz* add(Macierz* a,Macierz* b, MemoryManager* man);
 #endif
