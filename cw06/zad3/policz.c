#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

long nonzerobytescount(char* name){
    int file=open(name,O_RDONLY);
    if(file<0)return file;
    int res=1;
    char byte;
    long unsigned ret =0;
    while(res){
        res = read(file,&byte,sizeof(char));
        if(byte)ret++;
    }
    return ret;
}


int main (int argc, char **argv){
    if(argc<2)return 1;
    char * path = argv[1];
    if(!mkfifo(path, 0660)){
        exit(2);
    }
    int fifo = open(path,O_RDONLY|O_NONBLOCK);
    FILE* wyniki = fopen("wyniki.log","w");
    char buff[128];
    int res;
    while(1){
        res = read(fifo,buff,sizeof(buff));
        if(!res){
            sleep(1);
        } else {
            char* ptr = strtok(buff,"\n");
            while(ptr!=NULL){
                printf("->%s %ld\n",ptr,nonzerobytescount(ptr));
                fprintf(wyniki,"%s: %ld\n",ptr,nonzerobytescount(ptr));
                fflush(wyniki);
                ptr=strtok(NULL,"\n");
            }
        }
    }
    fclose(wyniki);
    close(fifo);
    return 0;
}
