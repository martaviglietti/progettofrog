a.out: clean coccodrilli.o collisioni.o draws.o main.o funzionegioca.o gestionegrafica.o menu.o rana.o windowgeneration.o
	gcc coccodrilli.o collisioni.o draws.o main.o funzionegioca.o gestionegrafica.o menu.o rana.o windowgeneration.o -o a.out -lncurses
coccodrilli.o: header.h coccodrilli.c
	gcc -c coccodrilli.c 
collisioni.o: header.h collisioni.c
	gcc -c collisioni.c 
draws.o: header.h draws.c
	gcc -c draws.c 
main.o: header.h main.c
	gcc -c main.c 
funzionegioca.o: header.h funzionegioca.c
	gcc -c funzionegioca.c 
gestionegrafica.o: header.h gestionegrafica.c	
	gcc -c gestionegrafica.c 
menu.o: header.h menu.c	
	gcc -c menu.c 
rana.o: header.h rana.c	
	gcc -c rana.c 
windowgeneration.o: header.h windowgeneration.c	
	gcc -c windowgeneration.c
clean:
	rm -f *.o a.out
