#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE* fd = 0;
unsigned limit = 0;
unsigned interval = 0;
struct tm prevTime;

int dateChanged(){
    //TODO ocen date modyfikacji
    return 1;
}
int sizeExceded(){
    //TODO ocen, czy rozmiar odpowiednio duzy
    return 1;
}

int main (int argc, char **argv) {
    if(argc<4){
        printf("usage: [file name] [interval (sec)] [limit (B)]\n");
        return 1;
    } else {
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
        fd = fopen(argv[1],"r");
        if(!fd){
            printf("could not open file: %s\n",argv[1]);
            return 4;
        }
    }
    //TODO ustaw prevTime
    //while(1)
    {
        sleep(interval);
        if(dateChanged()&&sizeExceded()){
            int PID = fork();
            char * tmp = (char *)malloc(sizeof(char) * (strlen(argv[1])));
            sprintf(tmp, "./bak/%s", argv[1]);
			if(execlp("cp", "cp", tmp, "./", NULL) < 0)
					printf("Blad exec\n");
        }
    }
}
