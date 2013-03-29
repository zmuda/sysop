#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

unsigned long zlicz(char* dirPath){
    DIR *dp;
    struct dirent *ep;
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
                    suma+=zlicz(tmp);
                }
            } else {
                suma+=ep->d_reclen;
            }
        }
        (void) closedir (dp);
    }
    else
        perror ("Couldn't open the directory");
    return suma;
}

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
                printf("%s\n",tmp);
                suma+=fileStat.st_size;
                //suma+=ep->d_reclen;
            }
        }
        (void) closedir (dp);
    }
    else
        perror ("Couldn't open the directory");
    return suma;
}


int main (int argc, char **argv){
    printf("%luB",zlicz2("./"));


    return 0;
}
