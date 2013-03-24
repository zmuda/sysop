#include <stdio.h>
#include <stdlib.h>
#include "libmatrix.h"

int main(int argc, char **argv)
{
    init(1000);
    Macierz* a = inputs(3,2);
    a->tab[1][0]=9.1;
    printMatrix(a);

    Macierz* b = inputs(2,3);
    printMatrix(b);

    printMatrix(mul(a,b));


    printFree();
    printOccupied();
    printDiagnostics();

    return 0;
}
