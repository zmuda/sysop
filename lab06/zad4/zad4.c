#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char **argv){
    FILE* rura = popen("sort","w");
    char buff[128];
    char* result = gets(buff);
    while(result!=NULL){
        fprintf(rura,"%s\n",buff);
        result = gets(buff);
    }
    printf("\n-\t-\t-\t-\t-\t-\t\n");
    return 0;
}
