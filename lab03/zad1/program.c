#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#ifdef FHANDLE
#include <unistd.h>
#include <fcntl.h>
#endif
#define byte char

typedef struct s {
    int klucz;
    byte *dane;
} struktura;

void pokazStrukture(struktura* s, int rozmiar){
    printf("\n<%i>\t",s->klucz);
    while(rozmiar--){
        printf("%i ",s->dane[rozmiar]);
    }
}

void sortuj(char* plikNazwa){
    int rozmiar, ilosc;
	#ifndef FHANDLE
	FILE * fp = fopen(plikNazwa, "r+b");
	#else
	int fp = open(plikNazwa, O_RDWR);
	#endif
	#ifndef FHANDLE
	if(fp == NULL)
	#else
	if(fp == -1)
	#endif
	{
		printf("Nie znaleziono pliku!\n");
		exit(89);
	}
	#ifndef FHANDLE
	fread(&rozmiar, sizeof(int), 1, fp);
	fread(&ilosc, sizeof(int), 1, fp);
	#else
	read(fp, &rozmiar, sizeof(int));
	read(fp, &ilosc, sizeof(int));
	#endif
	printf("Rozmiar sortowanej struktury: %d\tLiczebnosc struktur: %d\n", rozmiar, ilosc);
    struktura *a,*b;
    a=(struktura*)malloc(sizeof(struktura));
    b=(struktura*)malloc(sizeof(struktura));
    a->dane=(byte*)malloc(sizeof(byte)*rozmiar);
    b->dane=(byte*)malloc(sizeof(byte)*rozmiar);
    int i,posortowane = 0;
    while(!posortowane){
        posortowane=1;
        for(i=0;i<ilosc-1;i++){
            #ifndef FHANDLE
			fseek(fp, 2*sizeof(int) + i * (sizeof(int) + sizeof(byte)*rozmiar), SEEK_SET);
			fread(a, 1, sizeof(int), fp);
			fread(a->dane, 1, sizeof(byte)*rozmiar, fp);
			fread(b, 1, sizeof(int), fp);
			fread(b->dane, 1, sizeof(byte)*rozmiar, fp);
			#else
			lseek(fp, 2*sizeof(int) + i * (sizeof(int) + sizeof(byte)*rozmiar), SEEK_SET);
			read(fp, a, sizeof(int));
            read(fp,a->dane,sizeof(byte)*rozmiar);
			read(fp, b, sizeof(int));
            read(fp,b->dane,sizeof(byte)*rozmiar);
			#endif
			#ifdef VERBOSE
			printf("\n\t<%i> vs <%i>",a->klucz,b->klucz);
			pokazStrukture(a,rozmiar);
			pokazStrukture(b,rozmiar);
            #endif
			if(a->klucz>b->klucz){
                #ifndef FHANDLE
                fseek(fp, 2*sizeof(int) + i * (sizeof(int) + sizeof(byte)*rozmiar), SEEK_SET);
                fwrite(b, sizeof(int),1,fp);
                fwrite(b->dane, sizeof(byte)*rozmiar, 1, fp);
               #include <unistd.h> fwrite(a, sizeof(int),1,fp);
                fwrite(a->dane, sizeof(byte)*rozmiar, 1, fp);
                #else
                lseek(fp, 2*sizeof(int) + i * (sizeof(int) + sizeof(byte)*rozmiar), SEEK_SET);
                write(fp, b, sizeof(int));
                write(fp, b->dane, sizeof(byte)*rozmiar);
                write(fp, a, sizeof(int));
                write(fp, a->dane, sizeof(byte)*rozmiar);
                #endif
                posortowane = 0;
			}
        }
        #ifdef VERBOSE
        printf("\n");
        #endif
    }
    free(a->dane);
    free(a);
    free(b->dane);
    free(b);
    #ifndef FHANDLE
	fclose(fp);
	#else
	close(fp);
	#endif
}

void generuj(unsigned rozmiar, unsigned ilosc, char* plikNazwa){
    printf("\ngeneruj\n");
    #ifndef FHANDLE
	FILE * fp = fopen(plikNazwa, "wb+");
	#else
	int fp = open(plikNazwa, O_CREAT | O_RDWR);
	#endif
	#ifndef FHANDLE
	if(fp == NULL)
	#else
	if(fp == -1)
	#endif
	{
		fprintf(stderr, "Nie moge otworzyc pliku: %s\n", plikNazwa);
		exit(98);
	}
	printf("Rozmiar struktury: %d\tLiczebnosc struktury: %d\n", rozmiar, ilosc);
    //naglowek
	#ifndef FHANDLE
	fwrite(&rozmiar, sizeof(int), 1, fp);
	fwrite(&ilosc, sizeof(int), 1, fp);
	#else
	write(fp, &rozmiar, sizeof(int));
	write(fp, &ilosc, sizeof(int));
	#endif
    //cialo funkcji
    srand(time(NULL));
	int i;
    for(i = 0 ; i < ilosc ; i++)
	{
            struktura* s = (struktura*)malloc(sizeof(byte)*rozmiar+sizeof(int));
		s->klucz = rand();
		int j;
		s->dane = (byte *)malloc(sizeof(byte) * rozmiar);
		for(j = 0 ; j < rozmiar ; j++){
			s->dane[j] = (byte)(rand() % 255);
		}
		#ifndef FHANDLE
		fwrite(s, sizeof(int),1,fp);
        fwrite(s->dane, sizeof(byte)*rozmiar, 1, fp);
		#else
		write(fp, s, sizeof(int));
		write(fp, s->dane, sizeof(byte)*rozmiar);
		#endif
	}
	#ifndef FHANDLE
	fclose(fp);
	#else
	close(fp);
	#endif
}

int main(int argc, byte **argv){
    switch(argc){
        case 3:
            if( strcmp(argv[1],"sortuj") ){
                printf("\nniepoprawne argumeny - tryb dzialania\n");
                return 2;
            } else {
                sortuj(argv[2]);
            }
        break;
        case 5:
            if( strcmp(argv[1],"generuj") ){
                printf("\nniepoprawne argumeny - tryb dzialania\n");
                return 3;
            } else {
                unsigned rozmiar = atoi(argv[2]);
                unsigned ilosc = atoi(argv[3]);
                if(!rozmiar&&argv[2][0]!='0') {
                    printf("\nniepoprawne argumeny - rozmiar struktur\n");
                    return 11;
                }
                if(!ilosc){
                    printf("\nniepoprawne argumeny - ilosc struktur\n");
                    return 12;
                }
                generuj(rozmiar,ilosc,argv[4]);
            }
        break;
        default:printf("usage:\n\tsortuj [nazwa pliku]\n\tgeneruj [rozmiar danych struktury w B] [ilosc struktur] [nazwa pliku wyjsciowego]\n");
                return 1;break;
    }
    return 0;
}
