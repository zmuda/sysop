#include <stdio.h>
#include <stdlib.h>
#include "src/libmatrix.h"
#include "lib/libmem.h"

int main(int argc, char **argv)
{
    init(1000);
    Macierz* a = inputs(3,2);
    a->tab[1][0]=9.1;
    printMatrix(a);

    Macierz* b = inputs(2,3);
    printMatrix(b);

    printMatrix(mul(a,b));

    printDiagnostics();
    Diagnostics* d = diagnose();
    printf("<%u>",d->smallestFree);

    return 0;
}
