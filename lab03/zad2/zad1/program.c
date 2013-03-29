#include <stdio.h>
#include <stdlib.h>

void sortuj(char* plikNazwa){
    printf("\nsortuj\n");
}

void generuj(unsigned rozmiar, unsigned ilosc, char* plikNazwa){
    printf("\ngeneruj\n");
}

int main(int argc, char **argv){
    switch(argc){
        case 3:
            if( strcmp(argv[1],"sortuj") ){
                return 2;
            } else {
                sortuj(argv[2]);
            }
        break;
        case 5:
            if( strcmp(argv[1],"generuj") ){
                return 3;
            } else {
                unsigned rozmiar = atoi(argv[2]);
                unsigned ilosc = atoi(argv[2]);
                if(rozmiar<sizeof(int)) return 11;
                if(!ilosc) return 12;
                generuj(rozmiar,ilosc,argv[4]);
            }
        break;
        default:return 1;break;
    }
    return 0;
}
