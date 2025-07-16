# === Impostazioni di base ===

# Directory dei sorgenti e di output
SRC_DIR = versione_threads
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/$(SRC_DIR)

# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -g -I$(SRC_DIR)
LDFLAGS = -lncurses -lpthread

# File sorgenti
SOURCES = CrocHandle.c Draws.c FrogHandle.c\
          Partita.c WindowGen.c collisioni.c main.c menu.c

# File oggetto generati
OBJECTS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.c=.o))

# Nome eseguibile
EXEC = $(BUILD_DIR)/$(SRC_DIR)/frog_game_threads

# === Target principali ===

# Target predefinito: build + esegui
default: run

# Compila tutti i file
build: $(EXEC)

# Compilazione ed esecuzione
run: $(EXEC)
	./$(EXEC)

# Regola per creare l’eseguibile
$(EXEC): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Regola per compilare ogni .c in .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Pulizia degli oggetti e build
clean:
	rm -rf $(BUILD_DIR)

.PHONY: default build run clean
