# === Impostazioni ===
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses -lpthread

# Lista dei sorgenti e oggetti
SOURCES = CrocHandle.c Draws.c FrogHandle.c Partita.c collisioni.c main.c
OBJECTS = $(SOURCES:.c=.o)

# Nome eseguibile
EXEC = frog_game_threads

# Target predefinito
all: $(EXEC)

# Regola per l'eseguibile
$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Regola per compilare ogni file .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Pulizia
clean:
	rm -f $(OBJECTS) $(EXEC)

.PHONY: all clean
