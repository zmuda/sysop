#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>


unsigned long zlicz2(char* dirPath){
    DIR *dp;
    struct dirent *ep;
    struct stat fileStat;
    dp = opendir (dirPath);
    unsigned long suma = 0;
    if (dp != NULL){
        while (ep = readdir (dp)){
            if(ep->d_type==DT_DIR){
                //printf("%i %s\n",ep->d_reclen,ep->d_name);
                if( strcmp(ep->d_name,"..") && strcmp(ep->d_name,".") ){
                    char tmp[80];
                    strcpy(tmp,dirPath);
                    strcat(tmp,ep->d_name);
                    strcat(tmp,"/");
                    //printf("%s\n",tmp);
                    suma+=zlicz2(tmp);
                }
            } else {
                char tmp[80];
                strcpy(tmp,dirPath);
                strcat(tmp,ep->d_name);
                if(stat(tmp,&fileStat) < 0)
                    return 1;
                printf("%iB\t%s\n",fileStat.st_size,tmp);
                suma+=fileStat.st_size;
            }
        }
        (void) closedir (dp);
    }
    else
        perror ("Couldn't open the directory");
    return suma;
}


int main (int argc, char **argv){
    char tmp[80];
    if(argc<2){
        strcpy(tmp,"./");
    } else {
        strcpy(tmp,argv[1]);
        strcat(tmp,"/");
        // wielokrotne '/' nie zmienia sensu ale brak zadnego juz tak - dodajemy
    }
    printf("\n\t%luB\n",zlicz2(tmp));


    return 0;
}
