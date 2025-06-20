# Nome dell'eseguibile finale
TARGET = frog_game

# Compilatore
CC = gcc

# Flag per la compilazione
CFLAGS = -Wall -Wextra -g

# Librerie da linkare
LIBS = -lncurses -lpthread

# Lista dei file sorgente
SRCS = \
    threads1.c \
    gestionegrafica.c \
    comunicazione.c \
    collisioni.c \
    gestionecoccodrilli.c \
    disegni.c \
    windowsgeneration.c

# File oggetto generati automaticamente
OBJS = $(SRCS:.c=.o)

# Regola principale
all: $(TARGET)

# Come creare l'eseguibile
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Pulizia
clean:
	rm -f $(TARGET) $(OBJS)

# Ricompilazione completa
rebuild: clean all
