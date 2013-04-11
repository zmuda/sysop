#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

//zmienne odpowiadajace parametrom
int w = 0, v = 1;
char* exeName =0;
char** extensions=0;
int tokens=0;

//funkcja sprawdzajaca folder
void checkDir(char **argv) {
    int sum = 0, proc = 0;
    //ustalamy adres katalogu
    char* filename = getenv("PATH_TO_BROWSE");
    if (!filename) {
        filename = malloc(20 * sizeof (char));
        filename = ".";
    }
    //ustalamy rozszerzenie
    DIR * d = opendir(filename);
    assert(d != NULL);
    struct dirent *t;
    char a[256];
    char b[256];
    //sprawdzamy katalog
    while ((t = readdir(d))) {
        if (t->d_type == 4) {
            if (strcmp(".", t->d_name) != 0 && strcmp("..", t->d_name) != 0) {
                sprintf(a, "%s/%s", filename, t->d_name);
                proc++;
                int PID = fork();
                if (PID == 0) {
                    assert(setenv("PATH_TO_BROWSE", a, 1)>-1);
                    assert(execv(exeName, argv) > -1);
                }
            }
        } else {
            sprintf(b, "%s/%s", filename, t->d_name);
            if (extensions) {
                char *tmp = strrchr(b, '.');
                int i=0;
                while(i<tokens && strcmp(extensions[i],tmp)){
                    i++;
                }
                if (i<tokens) {
                    sum++;
                    if(v)printf("<%s>\n",b);
                }
            } else {
                sum++;
                if(v)printf("<%s>\n",b);
            }

        }
    }
    if (w) sleep(15);
    int sumback = sum;
    assert(closedir(d)>-1);
    while (proc) {
        int r = -1;
        wait(&r);
        sum += WEXITSTATUS(r);
        proc--;
    }
    if (v) {
        printf("Katalog: %s || Plikow:  %d  || Sumarycznie:  %d\n", filename, sumback, sum);
    }
    exit(sum);
}
void parseExtensions(){
    char* exts = getenv("EXT_TO_BROWSE");
    if(exts){
        int len = strlen(exts);
        int pos=0;
        tokens=0;
        while(pos<len-2){
            tokens++;
            pos+=strcspn(exts,":");
        }
        extensions = malloc(sizeof(char*)*tokens);
        pos=0;
        int i,size;
        size=strcspn(exts,":");
        extensions[0]=malloc(sizeof(char)*size);
        strcpy(extensions[0],strtok(exts,":"));
        pos=size;
        for(i=1;i<tokens;i++){
            size=strcspn(exts,":");
            pos+=size;
            extensions[i]=malloc(sizeof(char)*size);
            strcpy(extensions[i],strtok(NULL,":"));
        }
    } else {
        extensions=0;
    }
}
//main sprawdzanie argumentow i wywolanie funkcji
int main(int argc, char** argv) {
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp("-w", argv[i]) == 0) w = 1;
        if (strcmp("-v", argv[i]) == 0) v = 1;
    }
    exeName=argv[0];
    parseExtensions();
    checkDir(argv);
    return (EXIT_SUCCESS);
}

