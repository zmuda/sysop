#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

unsigned long zlicz2(char* dirPath) {
    DIR *dp;
    struct dirent *ep;
    struct stat fileStat;
    dp = opendir (dirPath);
    unsigned long suma = 0;

    if (dp != NULL) {
        while ((ep=readdir(dp))) {
            char tmp[256];

            if(ep->d_type==DT_DIR) {
                if( strcmp(ep->d_name,"..") && strcmp(ep->d_name,".") ) {
                    strcpy(tmp,dirPath);
                    strcat(tmp,ep->d_name);
                    strcat(tmp,"/");
                    suma+=zlicz2(tmp);
                }
            } else {
                strcpy(tmp,dirPath);
                strcat(tmp,ep->d_name);

                if(stat(tmp,&fileStat) < 0)
                    return 1;

                int przyrost =fileStat.st_size;
                printf("%iB\t%s\n",przyrost,tmp);
                suma+=przyrost;
            }
        }

        closedir (dp);
    }
    else
        printf("Couldn't open the directory\n");

    return suma;
}

int main (int argc, char **argv) {
    char tmp[80];

    if(argc<2) {
        realpath("./",tmp);
    } else {
        realpath(argv[1],tmp);
    }

    strcat(tmp,"/");//bedziemy szukac wewnarz katalogu
    printf("\n\t%luB\n",zlicz2(tmp));
    return 0;
}
