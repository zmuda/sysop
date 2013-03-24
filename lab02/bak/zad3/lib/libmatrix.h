 #ifndef MATLIB_H
 #define MATLIB_H
 /* tutaj są wpisane deklaracje funkcji */
 typedef struct matrix{
    unsigned width;
    unsigned height;
    double** tab;
} Macierz;
Macierz* zeros(unsigned width, unsigned height);
Macierz* inputs(unsigned width, unsigned height);
void printMatrix(Macierz* m);
Macierz* createWithoutFilling(unsigned width, unsigned height);
void dispose(Macierz* m);
Macierz* sub(Macierz* a,Macierz* b);
Macierz* mul(Macierz* a,Macierz* b);
Macierz* add(Macierz* a,Macierz* b);
 #endif
