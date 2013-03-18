#dodatkowy argument nazywa sie LOGS;
#make LOGS=yes
#(tak uaktywnic logowanie na konsole)

VPATH=src/include:src/operacje:src/we_wy:src/reszta:src/

CC = gcc $(FLAG) $(INC) $(COMS)
INC = -I src/include
FLAG = -Wall
PROJECT = macierze
COMS =
	ifeq ($(LOGS),yes)
		COMS = -D KOMUNIKATY
	endif

ZR_KONS = wczytaj_konsola.c wypisz_konsola.c
ZR_PLIK = wczytaj_plik.c wypisz_plik.c pliki.c
ZR_RESZTA = suma.c roznica.c iloczyn.c macierz.c

O_KONS=$(ZR_KONS:.c=.o)
O_PLIK=$(ZR_PLIK:.c=.o)
O_RESZTA=$(ZR_RESZTA:.c=.o)

konsola : $(O_KONS) $(O_RESZTA) main_k.o 
	$(CC) $^ -o $(PROJECT)

plik : $(O_PLIK) $(O_RESZTA) main_p.o
	$(CC) $^ -o $(PROJECT)

iloczyn.o : iloczyn.c iloczyn.h macierz.h
	$(CC) -c $< -o $@

suma.o : suma.c suma.h macierz.h
	$(CC) -c $< -o $@

roznica.o : roznica.c roznica.h macierz.h
	$(CC) -c $< -o $@

pliki.o : pliki.c pliki.h
	$(CC) -c $< -o $@

wczytaj_konsola.o : wczytaj_konsola.c wczytaj.h macierz.h
	$(CC) -c $< -o $@

wczytaj_plik.o : wczytaj_plik.c wczytaj.h macierz.h pliki.h
	$(CC) -c $< -o $@

wypisz_konsola.o : wypisz_konsola.c wczytaj.h macierz.h
	$(CC) -c $< -o $@

wypisz_plik.o : wypisz_plik.c wczytaj.h macierz.h pliki.h
	$(CC) -c $< -o $@

main_k.o : main.c
	$(CC) -c $< -o $@

main_p.o : main.c pliki.h
	$(CC) -D WEWY_PLIKI -c $< -o $@

macierz.o : reszta/macierz.c macierz.h
	$(CC) -c $< -o $@

clean :
	rm -f *.o
	rm $(PROJECT)

.PHONY : clean
