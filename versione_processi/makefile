CFLAGS = -g -Wall -I/usr/include  -I/usr/include/x86_64-linux-gnu# Include ncurses headers if needed

LDFLAGS = -lncurses

OBJECTS = coccodrilli.o collisioni.o draws.o main.o funzionegioca.o gestionegrafica.o menu.o rana.o windowgeneration.o

a.out: $(OBJECTS)
	gcc $(OBJECTS) -o a.out $(LDFLAGS)

coccodrilli.o: header.h coccodrilli.c
	gcc $(CFLAGS) -c coccodrilli.c

collisioni.o: header.h collisioni.c
	gcc $(CFLAGS) -c collisioni.c

draws.o: header.h draws.c
	gcc $(CFLAGS) -c draws.c

main.o: header.h main.c
	gcc $(CFLAGS) -c main.c

funzionegioca.o: header.h funzionegioca.c
	gcc $(CFLAGS) -c funzionegioca.c

gestionegrafica.o: header.h gestionegrafica.c
	gcc $(CFLAGS) -c gestionegrafica.c

menu.o: header.h menu.c
	gcc $(CFLAGS) -c menu.c

rana.o: header.h rana.c
	gcc $(CFLAGS) -c rana.c

windowgeneration.o: header.h windowgeneration.c
	gcc $(CFLAGS) -c windowgeneration.c

clean:
	rm *.o a.out
