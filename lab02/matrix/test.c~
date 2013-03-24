#include <stdio.h>
#include <stdlib.h>
#include <libmem.h>

//#define BLOCK_SIZE 2
//#define SMALL_STRATEGY 1
int main(int argc, char **argv)
{
    init(100);
    void* tmp4 = allocation(11);
    void* tmp3 = allocation(12);
    void* tmp2 = allocation(13);
    void* tmp = allocation(64);
    //firstSuisficientStrategyAllocation(20);

    //allocation(10);

    deallocation(tmp2);
    tmp2 = allocation(9);
    deallocation(tmp);
    deallocation(tmp3);
    deallocation(tmp4);
    deallocation(tmp2);
    allocation(71);
    //allocation(63);
    //allocation(63);
    printFree();
    printOccupied();
    printDiagnostics();
    return 0;
}
