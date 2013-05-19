#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char **argv){
    if(argc>2){
        FILE* fifo = fopen(argv[1],"w");
        int i=argc-2;
        //char buff[128];
        while(i--){
            fprintf(fifo,"%s\n",argv[i+2]);
            //printf("<%s>\n",argv[i+2]);
            fflush(fifo);
        }
        fclose(fifo);
        return 0;
    } else {
        printf("usage: [fifo file name] [file path] [file path] ...");
    }
    return 1;
}
