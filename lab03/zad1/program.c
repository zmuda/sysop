#include <stdio.h>
#include <stdlib.h>
#define byte char

typedef struct s {
    int klucz;
    byte *dane;
} struktura;

void sortuj(char* plikNazwa){
    printf("\nsortuj\n");
}

void generuj(unsigned rozmiar, unsigned ilosc, char* plikNazwa){
    printf("\ngeneruj\n");
    #ifndef SYS
	FILE * fp = fopen(plikNazwa, "wb+");
	#else
	int fp = open(plikNazwa, O_CREAT | O_RDWR);
	#endif
	#ifndef SYS
	if(fp == NULL)
	#else
	if(fp == -1)
	#endif
	{
		fprintf(stderr, "Nie moge otworzyc pliku: %s\n", plikNazwa);
		exit(98);
	}
	//naglowek
	#ifndef SYS
	fwrite(&ilosc, sizeof(int), 1, fp);
	fwrite(&rozmiar, sizeof(int), 1, fp);
	#else
	write(fp, &ilosc, sizeof(int));
	write(fp, &rozmiar, sizeof(int));
	#endif
    //cialo funkcji
    srand(time(NULL));
	int i;
    for(i = 0 ; i < ilosc ; i++)
	{
		struktura * s = (struct struktura *)malloc(sizeof(char)*rozmiar+sizeof(int));
		if(s == NULL)
		{
			printf("malloc error\n");
			exit(97);
		}
		s->klucz = rand() % 100;
        //printf("Wygenerowano klucz: %d\n", s->klucz);
		int j;
		s->dane = (char *)malloc(sizeof(char) * rozmiar);
		if(s->dane == NULL)
		{
			printf("malloc error\n");
			exit(96);
		}
		for(j = 0 ; j < rozmiar ; j++){
			s->dane[j] = (char)(rand() % 255);
            //printf("%c",s->dane[j]);
		}
		#ifndef SYS
		fwrite(s, sizeof(int) + sizeof(char)*rozmiar, 1, fp);
		#else
		write(fp, s, sizeof(int) + sizeof(char)*rozmiar);
		#endif
	}
	#ifndef SYS
	fclose(fp);
	#else
	close(fp);
	#endif
}

int main(int argc, char **argv){
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
                unsigned ilosc = atoi(argv[2]);
                if(!rozmiar) {
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
        default:printf("\nniepoprawne argumeny - ilosc\n");return 1;break;
    }
    return 0;
}
