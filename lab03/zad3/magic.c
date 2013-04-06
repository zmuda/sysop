#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
char* locks;

int open_file(const char* name, int flag){
    return open(name, flag);
}

int set_lock(int fd, short byte, int flag){
    struct flock* fl = (struct flock*)malloc(sizeof(struct flock));
    fl->l_type = flag;
    fl->l_whence = SEEK_SET;
    fl->l_start = byte;
    fl->l_len = 1;
    if(fcntl(fd, F_SETLK, fl)==-1){
        printf("Nie mozna utworzyc rygla. Poza zakresem pliku?\n");
        return -1;
    }
    locks[byte]=flag;
    if(flag==F_UNLCK){
        printf("Odblokowanie rygla z %d znaku pliku.\n", byte);
    } else {
        printf("Rygiel poprawnie zalozony na %d znaku pliku. Typ: ", byte);
        if(flag!=F_RDLCK){
            printf("zapis+odczyt\n");
        }else{
            printf("odczyt\n");
        }
    }
    return 0;
}

int show_locked(int fd){
    int i,length = lseek(fd, 0, SEEK_END);
    struct flock fl;
    fl.l_whence = SEEK_SET;
    fl.l_len = 1;
    for(i=0; i<length; i++){
        fl.l_type = F_WRLCK;
        fl.l_start = i;
        fcntl(fd, F_GETLK, &fl);
        if(fl.l_type!=LOCK_EX){
            printf("-> Rygiel do odczytu na znaku %d\n", i);
        }
        fl.l_type = F_RDLCK;
        fcntl(fd, F_GETLK, &fl);
        if(fl.l_type!=LOCK_EX){
            printf("-> Rygiel do zapisu na znaku %d\n", i);
        }
    }
    for(i=0; i<length; i++){
        if(locks[i]!=F_UNLCK){
            printf("-> Wlasny rygiel na pozycji %d - typ: ", i);
            if(locks[i]!=F_RDLCK){
                printf("zapis+odczyt\n");
            }else{
                printf("odczyt\n");
            }
        }
    }
    return 0;
}

void odczytajZPliku(int fd, int byte){
    struct flock fl;
    fl.l_type = F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = byte;
    fl.l_len = 1;
    if(fcntl(fd, F_SETLK, &fl)<0){
        printf("Odczyt zablokowany przez  inny proces.\n");
        return;
    }
    lseek(fd, byte, SEEK_SET);
    char c;
    read(fd, &c, sizeof(char));
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    printf("%c\n", c);
}

int wpiszDoPliku(int fd, int byte, char c){
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = byte;
    fl.l_len = 1;
    if(fcntl(fd, F_SETLK, &fl)==-1){
        printf("Zapis zablokowany przez inny proces.\n");
        return -1;
    }
    lseek(fd, byte, SEEK_SET);
    write(fd, &c, sizeof(char));
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    return 0;
}

int main(int argc, char** argv){
    char *file, *cmd = (char*)malloc(sizeof(char)*256);
    char *type = malloc(sizeof(char)*256);
    int length;
    int fd, tmp;
    char c;
    if(argc<2){
        char* tmp = strtok(argv[0],"/");
        char* lastTmp=tmp;
        while(tmp!=NULL){
            lastTmp=tmp;
            tmp=strtok(NULL,"/");
        }
        printf("usage:\n\t%s [nazwa pliku]",lastTmp);
        return -1;
    }
    file = argv[1];
    if((fd=open_file(file, O_RDWR))==-1){
        printf("Nie udalo sie otworzyc pliku\n");
        return -2;
    }
    length = lseek(fd, 0, SEEK_END);
    locks = malloc(length*sizeof(char));
    for(tmp=0; tmp<length; tmp++){
        locks[tmp] = F_UNLCK;
    }
    printf("Wpisz help, aby uzyskac pomoc\n");
    while(strcmp(cmd, "bye")){
        scanf("%s", cmd);
        if(!strcmp(cmd, "help")){
            printf("usage:\n\tlock pozycja [r|w|u]");
            printf("\n\t- blokuje do odczytu(r), do zapisu i odczytu(w) lub odblokowuje(u) znak pliku");
            printf("\n\tdisplay - listuje dostepne rygle\n");
            printf("\tread pozycja - odczytuje znak na danej pozycji\n");
            printf("\twrite pozycja znak- zapisuje znak na pozycji\n");
            printf("\tbye - konczy program\n");
            printf("rygiel do odczytu pozwala na odczyt a zabrania zapisu\n");
            printf("rygiel do zapisu nie pozwala ani na odczyt ani na zapis\n");

        }
        else if(!strcmp(cmd, "lock")){
            scanf("%d", &tmp);
            scanf("%s", type);
            if(!strcmp(type, "r")) set_lock(fd, tmp,F_RDLCK);
            else if(!strcmp(type, "w")) set_lock(fd, tmp, F_WRLCK);
            else if(!strcmp(type, "u")) set_lock(fd, tmp, F_UNLCK);
            else printf("Zly typ rygla. Wpisz help, aby uzyskac pomoc.\n");
        }
        else if(!strcmp(cmd, "display")){
            show_locked(fd);
            printf("-koniec-\n");
        }
        else if(!strcmp(cmd, "read")){
            scanf("%d", &tmp);
            odczytajZPliku(fd, tmp);
        }
        else if(!strcmp(cmd, "write")){
            scanf("%d", &tmp);
            scanf("%c", &c);//przeskocz spacje
            scanf("%c", &c);
            wpiszDoPliku(fd, tmp, c);
        }
        else if (strcmp(cmd, "bye")){
            printf("Tak tego nie zrobisz - sprawdz help\n");
        }
    }
    close(fd);
    return 0;
}
