# === Makefile semplice per cartella singola ===

# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses -lpthread

# File sorgenti
SRC = CrocHandle.c Draws.c FrogHandle.c Partita.c WindowGen.c collisioni.c main.c menu.c

# File oggetto
OBJ = $(SRC:.c=.o)

# Eseguibile
EXEC = frog_game_threads

# Regole
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)

.PHONY: all clean
