#include <stdio.h>
#include <stdlib.h>
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
    printf("\nsortuj\n");

    int size, count;
	#ifndef SYS
	FILE * fp = fopen(plikNazwa, "r+b");
	#else
	int fp = open(plikNazwa, O_RDWR);
	#endif

	#ifndef SYS
	if(fp == NULL)
	#else
	if(fp == -1)
	#endif
	{
		printf("Nie znaleziono pliku!\n");
		exit(89);
	}

	#ifndef SYS
	fread(&size, sizeof(int), 1, fp);
	fread(&count, sizeof(int), 1, fp);
	#else
	read(fp, &size, sizeof(int));
	read(fp, &count, sizeof(int));
	#endif

	printf("Rozmiar struktury: %d\tLiczebnosc struktury: %d\n", size, count);

    struktura *a,*b;
    a=(struktura*)malloc(sizeof(struktura));
    b=(struktura*)malloc(sizeof(struktura));
    a->dane=(byte*)malloc(sizeof(byte)*size);
    b->dane=(byte*)malloc(sizeof(byte)*size);
    int i,posortowane = 0;
    while(!posortowane){
        posortowane=1;
        for(i=0;i<count-1;i++){
            #ifndef SYS
			fseek(fp, 2*sizeof(int) + i * (sizeof(int) + sizeof(char)*size), SEEK_SET);
			fread(a, 1, sizeof(int), fp);
			fread(a->dane, 1, sizeof(byte)*size, fp);
			fread(b, 1, sizeof(int), fp);
			fread(b->dane, 1, sizeof(byte)*size, fp);
			#else
			lseek(fp, 2*sizeof(int) + i * (sizeof(int) + sizeof(char)*size), SEEK_SET);
			read(fp, a, sizeof(int));
            read(fp,a->dane,sizeof(char)*size);
			read(fp, b, sizeof(int));
            read(fp,b->dane,sizeof(char)*size);
			#endif
			printf("\n\t<%i> vs <%i>",a->klucz,b->klucz);
			pokazStrukture(a,size);
			pokazStrukture(b,size);
			if(a->klucz>b->klucz){
                #ifndef SYS
                fseek(fp, 2*sizeof(int) + i * (sizeof(int) + sizeof(char)*size), SEEK_SET);
                fwrite(b, sizeof(int),1,fp);
                fwrite(b->dane, sizeof(byte)*size, 1, fp);
                fwrite(a, sizeof(int),1,fp);
                fwrite(a->dane, sizeof(byte)*size, 1, fp);
                #else
                lseek(fp, 2*sizeof(int) + i * (sizeof(int) + sizeof(char)*size), SEEK_SET);
                write(fp, b, sizeof(int));
                write(fp, b->dane, sizeof(char)*size);
                write(fp, a, sizeof(int));
                write(fp, a->dane, sizeof(char)*size);
                #endif
                posortowane = 0;
			}
        }
        printf("\n");
    }
    free(a->dane);
    free(a);
    free(b->dane);
    free(b);
    #ifndef SYS
	fclose(fp);
	#else
	close(fp);
	#endif
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
	printf("Rozmiar struktury: %d\tLiczebnosc struktury: %d\n", rozmiar, ilosc);
    //naglowek
	#ifndef SYS
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
		fwrite(s, sizeof(int),1,fp);
        fwrite(s->dane, sizeof(byte)*rozmiar, 1, fp);
		#else
		write(fp, s, sizeof(int));
		write(fp, s->dane, sizeof(char)*rozmiar);
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
        default:printf("\nniepoprawne argumeny - ilosc\n");return 1;break;
    }
    return 0;
}
