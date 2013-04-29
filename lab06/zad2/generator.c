#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>



int main (int argc, char **argv){
    char * path = "fifo";
    if(!mkfifo(path, 0666)){
        exit(2);
    }
    srand(time(0));
    printf("yolo");
    int fifo = open(path,O_WRONLY);
    printf("yolo");
    char buff[128];
    while(1){
        snprintf(buff, 10,"%d\n",rand()%100000000);
        write(fifo,buff,sizeof(buff));
    }
    return 0;
}
