#include <stdio.h>
#include <stdlib.h>
#include "../lib/libmatrix.h"
#include "../lib/libmem.h"

int main(int argc, char **argv)
{
    init(1000);
    printf("Zaalokowano 1000");
    fianlizeMemory();
    return 0;
}
