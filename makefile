SRC_DIR = ../progettofrog
OBJ_DIR = ../obj
BUILD_DIR = ../build

VERSIONE_PROCESSI_SRC = $(SRC_DIR)/versione_processi

CFLAGS = -g -Wall -I/usr/include  -I/usr/include/x86_64-linux-gnu # Include ncurses headers if needed
LDFLAGS = -lncurses

OBJECTS = $(OBJ_DIR)/coccodrilli.o $(OBJ_DIR)/collisioni.o $(OBJ_DIR)/draws.o $(OBJ_DIR)/main.o $(OBJ_DIR)/funzionegioca.o $(OBJ_DIR)/gestionegrafica.o $(OBJ_DIR)/menu.o $(OBJ_DIR)/rana.o $(OBJ_DIR)/windowgeneration.o

PROCESSI_EXEC = $(BUILD_DIR)/versione_processi/a.out

$(PROCESSI_EXEC): $(OBJECTS)
	gcc $(OBJECTS) -o $(PROCESSI_EXEC) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(VERSIONE_PROCESSI_SRC)/%.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)*.o $(BUILD_DIR)/versione_processi/a.out
