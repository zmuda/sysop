#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <assert.h>

FILE* fd = 0;
unsigned interval = 0;
time_t prevTime;
off_t limit = 0;
char* name;

int dateChanged(){
    struct stat sb;
    stat(name,&sb);
    if(sb.st_mtime>prevTime){
        prevTime=sb.st_mtime;
        return 1;
    } else return 0;
}
int sizeExceded(){
    struct stat sb;
    stat(name,&sb);
    return (sb.st_size>limit);
}

int main (int argc, char **argv) {
    if(argc<4){
        printf("usage: [file name] [interval (sec)] [limit (B)]\n");
        return 1;
    } else {
        name=argv[1];
        limit = atoi(argv[3]);
        if(limit<1){
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
    }
    struct stat sb;
    stat(name,&sb);
    prevTime=sb.st_mtime;
    time_t rawtime;
    struct tm timeinfo;

    while(1)
    {
        sleep(interval);
        if(dateChanged()&&sizeExceded()){
            int PID = fork();
            if(PID==0){
                char * tmp = (char *)malloc(sizeof(char) * (strlen(argv[1])));
                time(&rawtime);
                timeinfo=*localtime(&rawtime);
                sprintf(tmp, "./archiwum/%s_%d-%d-%d_%d-%d-%d", argv[1],1900+timeinfo.tm_year,timeinfo.tm_mon+1,timeinfo.tm_mday,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
                if(execlp("cp", "cp", "i.txt", tmp, "-fp") < 0)printf("Blad exec\n");
            } else {
                assert(PID>-1);
                int b = -1;
                wait(&b);
                FILE* fd = fopen(name,"w");
                fclose(fd);
            }
        }
    }
}
