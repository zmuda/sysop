#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <time.h>
#include <limits.h>
time_t data;
int mode;

time_t dataNormalna(int date[])
{
    struct tm t;
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 0;
    t.tm_mday = date[0];
    t.tm_mon = date[1]-1;
    t.tm_year = date[2]-1900;
    return mktime(&t);
}
int onlyDateComparision(time_t a,time_t b) {
    struct tm aa=*localtime(&a);
    struct tm bb=*localtime(&b);
    return aa.tm_year*12*31+aa.tm_mon*31+aa.tm_mday -bb.tm_year*12*31-bb.tm_mon*31-bb.tm_mday;
}
// to bedzie wywolywane przez ftw
int print(const char * filename, const struct stat *s, int flag) {
    if(!filename || !s)
        return -1;

    if(flag == FTW_F) {
        char* res = (char *)malloc(sizeof(char) * 256);

        switch(mode) {
        case -1:

            if(onlyDateComparision(s->st_mtime,data)<0)printf("-> %s\n", realpath(filename,res));

            break;
        case 0:

            if(onlyDateComparision(s->st_mtime,data)==0)printf("-> %s\n", realpath(filename,res));

            break;
        case 1:

            if(onlyDateComparision(s->st_mtime,data)>0)printf("-> %s\n", realpath(filename,res));

            break;
        default:
            exit(99);//nic nie piszemy, bo innego mode nie bedzie (porawnosc zapewniona)
        }
    }

    return 0; //dla ftw mamy taki format, ze trzeba zwracac int'a - zero jezeli ok
}

int main(int argc, char ** argv)
{
    int date[3];

    if(argc < 4)
    {
        printf("usage: %s [dir] ['<' albo '>' albo '='] [dd-MM-yyyy]\n", argv[0]);
        return 1;
    }

    mode=1000;

    if(!strcmp(argv[2],"<"))mode=-1;

    if(!strcmp(argv[2],">"))mode=1;

    if(!strcmp(argv[2],"="))mode=0;

    if(mode==1000) {
        printf("wrong mode character; use eg. <\n\tused: %s\n",argv[2]);
        return 2;
    }

    int i = 0;
    char * d = strtok(argv[3], " -:");

    while(d && (i < 3))
    {
        date[i++] = atoi(d);
        // nastepne slowo
        d = strtok(NULL, " -:");
    }

    data = dataNormalna(date);
    ftw(argv[1], print, 1024);
    return 0;
}
