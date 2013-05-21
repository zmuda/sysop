#ifndef MACROS_H
#define MACROS_H

#define FPATH "/tmp/fifo"

#define MAXTAB 10000

#define CLEANING_HANDLER(X) if(X){printf("%s\n", strerror(errno));clean(0);exit(-1);}
#define HANDLER(X) if(X){printf("%s\n", strerror(errno));exit(-1);}


#endif
