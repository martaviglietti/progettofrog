# Compilatore
CC = gcc

# Flag per la compilazione
CFLAGS = -Wall -Wextra -g

# Librerie da linkare
LIBS = -lncursesw

#nome del output finale

all: clean $(TARGET)
# regola principale
TARGET = frog_game
    
   
SRCS = \
    controlli.c \
    draws.c \
    main.c \
    funzionegioca.c \
    gestionegrafica.c \
    menu.c \
    rana.c \
    coccodrilli.c


OBJS = $(SRCS:.c=.o)

# Regola principale
all: $(TARGET)

#compilazione di tutti i sorgenti (senza linking)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Come creare l'eseguibile
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Pulizia
clean:
	rm -f $(TARGET) $(OBJS)

# Ricompilazione completa
rebuild: clean all
