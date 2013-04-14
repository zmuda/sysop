#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

FILE* fd = 0;
unsigned interval = 0;
unsigned bytes = 0;
char* name;


int main (int argc, char **argv) {
    if(argc<4){
        printf("usage: [file name] [interval (sec)] [limit (B)]\n");
        return 1;
    } else {
        name=argv[1];
        bytes = atoi(argv[3]);
        if(bytes<1){
            printf("wrong limit value\n");
            return 2;
        }
        interval = atoi(argv[2]);
        if(interval<1){
            printf("wrong interval value\n");
            return 3;
        }
        fd = fopen(name,"r");
        if(!fd){
            printf("could not open file: %s\n",argv[1]);
            return 4;
        }
        fclose(fd);
    }
    time_t rawtime;
    struct tm timeinfo;

    FILE* fd = fopen(name,"ab");
    srand(time(NULL));
    int i;
    while(1)
    {
        sleep(interval);
        fseek(fd,0,SEEK_END);
        time(&rawtime);
        timeinfo=*localtime(&rawtime);
        fprintf(fd,"\n%d %d-%d-%d_%d-%d-%d ",getpid(),1900+timeinfo.tm_year,timeinfo.tm_mon+1,timeinfo.tm_mday,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
        for(i=0;i<bytes;i++){
            fprintf(fd,"%c",(char)rand());
        }
    }
    fclose(fd);
}
