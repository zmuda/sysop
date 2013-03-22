#include <stdio.h>
#include <stdlib.h>
#include "libmatrix.h"

Macierz* createWithoutFilling(unsigned width, unsigned height){
    Macierz* ret = allocation(sizeof(Macierz));
    ret->tab = allocation(height*sizeof(Macierz*));
    int i;
    for(i=0;i<height;i++){
        ret->tab[i]=allocation(width*sizeof(double));
    }
    ret->width=width;
    ret->height=height;
    return ret;
}
Macierz* zeros(unsigned width, unsigned height){
    Macierz* ret = createWithoutFilling(width,height);
    int i,j;
    for(i=0;i<width;i++){
        for(j=0;j<height;j++){
            ret->tab[j][i]=.0;
        }
    }
    return ret;
}
Macierz* inputs(unsigned width, unsigned height){
    Macierz* ret = createWithoutFilling(width,height);
    int i,j;
    for(i=0;i<width;i++){
        for(j=0;j<height;j++){
            printf("\n[%d][%d]=",i,j);
            double tmp;
            scanf("%lf",&tmp);
            ret->tab[j][i]=tmp;
        }
    }
    return ret;
}
void printMatrix(Macierz* m){
    printf("\n");
    int i,j;
    for(i=0;i<m->height;i++){
        for(j=0;j<m->width;j++){
            printf("%f\t",m->tab[i][j]);
        }
        printf("\n");
    }
}
void inc(Macierz* a,Macierz* m){
    int i,j;
    for(i=0;i<m->height;i++){
        for(j=0;j<m->width;j++){
            a->tab[i][j]+=m->tab[i][j];
        }
    }
}
void dec(Macierz* a,Macierz* m){
    int i,j;
    for(i=0;i<m->height;i++){
        for(j=0;j<m->width;j++){
            a->tab[i][j]-=m->tab[i][j];
        }
    }
}
Macierz* add(Macierz* a,Macierz* b){
    unsigned width = (a->width>b->width)?(a->width):(b->width);
    unsigned height = (a->height>b->height)?(a->height):(b->height);
    Macierz* ret = zeros(width,height);
    inc(ret,a);
    inc(ret,b);
    return(ret);
}

Macierz* sub(Macierz* a,Macierz* b){
    unsigned width = (a->width>b->width)?(a->width):(b->width);
    unsigned height = (a->height>b->height)?(a->height):(b->height);
    Macierz* ret = zeros(width,height);
    inc(ret,a);
    dec(ret,b);
    return(ret);
}


Macierz* mul(Macierz* a,Macierz* b){
    if(a->width!=b->height || a->height!=b->width)return 0;
    Macierz* ret = zeros(a->height,a->height);
    int i,j,k;
    for(i=0;i<a->height;i++){
        for(j=0;j<a->height;j++){
            for(k=0;k<a->width;k++){
                ret->tab[i][j]+=a->tab[i][k]*b->tab[k][j];
            }
        }
    }
    return(ret);
}

