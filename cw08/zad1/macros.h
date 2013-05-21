#ifndef MACROS_H
#define MACROS_H

#define KPATH "/tmp/serv"
#define KVAL 66

#define TABDIM 4
#define MAXTAB 10


#define EMPTY_SEM 0 //remaining space
#define FULL_SEM 1 //items produced
#define MUTEX 2
#define ZWIEKSZ 1
#define ZMNIEJSZ -1

struct matrix
{
	int dimmension;
	double data[TABDIM][TABDIM];
};

#define CLEANING_HANDLER(X) if(X){printf("%s\n", strerror(errno));clean(0);exit(-1);}
#define HANDLER(X) if(X){printf("%s\n", strerror(errno));exit(-1);}

#endif
