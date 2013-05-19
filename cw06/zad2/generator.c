#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void set(long num){
    int fd = open("lock", O_RDWR | O_CREAT, 0666);
    flock(fd,LOCK_EX);
    char buff[32];
    snprintf(buff, 10,"0%ld\n",num);
    write(fd,buff,sizeof(buff));
    close(fd);
    flock(fd,LOCK_UN);

}

int main(int argc, char **argv){
    if(argc<3){
        printf("\nusage: fifo_name clients_num\n");
        return -1;
    }
    char * path = argv[1];
    set(atoi(argv[2]));
    if(argc<2){
        printf("\nilu klientow? podaj prosze w argumencie!\n");
        return 1;
    }
    mkfifo(path, 0666);
    srand(time(0));
    int fifo = open(path,O_WRONLY);
    char buff[128];
    while(1){
        snprintf(buff, 10,"%d\n",rand()%100000000);
        write(fifo,buff,sizeof(buff));
    }
    return 0;
}
