#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>



int main (int argc, char **argv){
    char * path = "fifo";
    if(!mkfifo(path, 0666)){
        exit(2);
    }
    srand(time(0));
    int fifo = open(path,O_WRONLY);
    char buff[128];
    while(1){
        snprintf(buff, 10,"%d\n",rand()%100000000);
        write(fifo,buff,sizeof(buff));
    }
    return 0;
}
