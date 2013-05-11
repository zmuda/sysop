#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


#define true 1
#define false 0

long power(long a, long n, long mod){
     long power=a,result=1;
     while(n){
        if(n&1){
            result=(result*power)%mod;
        }
        power=(power*power)%mod;
        n>>=1;
     }
     return result;
}

int witness(long a, long n){
    long t,u,i;
    long prev,curr=0;
    u=n/2;
    t=1;
    while(!(u&1)){
        u/=2;
    ++t;
    }
    prev=power(a,u,n);
    for(i=1;i<=t;++i){
        curr=(prev*prev)%n;
        if((curr==1)&&(prev!=1)&&(prev!=n-1)){
            return true;
        }
        prev=curr;
    }
    if(curr!=1){
        return true;
    }
    return false;
}

//inline bool IsPrime( int number ){
inline int IsPrime( long number ){
    if ( ( (!(number & 1)) && number != 2 ) || (number < 2) || (number % 3 == 0 && number != 3) )
        return (false);
    if(number<1373653){
        int k;
        for( k = 1; 36*k*k-12*k < number;++k)
            if ( (number % (6*k+1) == 0) || (number % (6*k-1) == 0) )
                return (false);
        return true;
    }

    if(number < 9080191){
        if(witness(31,number)) return false;
        if(witness(73,number)) return false;
        return true;
    }


    if(witness(2,number)) return false;
    if(witness(7,number)) return false;
    if(witness(61,number)) return false;
    return true;

 /*WARNING: Algorithm deterministic only for numbers < 4,759,123,141 (unsigned int's max is 4294967296)
   if n < 1,373,653, it is enough to test a = 2 and 3.
   if n < 9,080,191, it is enough to test a = 31 and 73.
   if n < 4,759,123,141, it is enough to test a = 2, 7, and 61.
   if n < 2,152,302,898,747, it is enough to test a = 2, 3, 5, 7, and 11.
   if n < 3,474,749,660,383, it is enough to test a = 2, 3, 5, 7, 11, and 13.
   if n < 341,550,071,728,321, it is enough to test a = 2, 3, 5, 7, 11, 13, and 17.*/
}
void aqquire(){
    int fd = open("lock",O_RDWR);
    flock(fd,LOCK_EX);
    char buff[32];
    read(fd,buff,sizeof(buff));
    long num = atoi(buff);
    if(!num)exit(98);
    num--;
    lseek(fd,0,SEEK_SET);
    snprintf(buff, 10,"0%d\n",num);
    write(fd,buff,sizeof(buff));
    close(fd);
    flock(fd,LOCK_UN);
}
void release(){
    int fd = open("lock",O_RDWR);
    flock(fd,LOCK_EX);
    char buff[32];
    read(fd,buff,sizeof(buff));
    long num = atoi(buff);
    num++;
    lseek(fd,0,SEEK_SET);
    snprintf(buff, 10,"%d\n",num);
    write(fd,buff,sizeof(buff));
    close(fd);
    flock(fd,LOCK_UN);
}
int main (int argc, char **argv){
    if(argc<2){
        printf("\nusage: fifo_name\n");
        return -1;
    }
    aqquire();
    char * path = argv[1];
    int fifo = open(path,O_RDONLY);
    char buff[128];
    int r;
    while(1){
        r=read(fifo,buff,sizeof(buff));
        if(!r){
            sleep(1);
        }
        long num = atoi(buff);
        printf("%ld is prime: %d\n",num,IsPrime(num));
        sleep(2);
    }
    release();
    return 0;
}
