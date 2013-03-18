#dodatkowy argument nazywa sie LOGS;
#make LOGS=yes
#(tak uaktywnic logowanie na konsole)
#make plik
#(we-wy z plikow)

VPATH=src/include:src/operacje:src/we_wy:src/reszta:src/

CC = gcc $(FLAG) $(INC) $(COMS)
INC = -I src/include
FLAG = -Wall
PROJECT = macierze
COMS =
	ifeq ($(LOGS),yes)
		COMS = -D KOMUNIKATY
	endif

KONS = wczytaj_konsola.o wypisz_konsola.o
PLIK = wczytaj_plik.o wypisz_plik.o pliki.o
RESZTA = suma.o roznica.o iloczyn.o macierz.o

konsola : $(KONS) $(RESZTA) main.o 
	$(CC) $^ -o $(PROJECT)

plik : $(PLIK) $(RESZTA) main_p.o
	$(CC) $^ -o $(PROJECT)

main_p.o : main.c
	$(CC) -D WEWY_PLIKI -c $< -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean :
	rm -f *.o
	rm $(PROJECT)

.PHONY : clean
