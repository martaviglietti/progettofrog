SRC_DIR = .
BUILD_DIR = ../build

OBJ_DIR_PROCESSI = ../obj/versione_processi
OBJ_DIR_THREADS = ../obj/versione_threads

PROCESSI_SRC = $(SRC_DIR)/versione_processi
THREADS_SRC = $(SRC_DIR)/versione_threads

OBJECTS_PROCESSI = $(OBJ_DIR_PROCESSI)/coccodrilli.o \
				$(OBJ_DIR_PROCESSI)/collisioni.o \
				$(OBJ_DIR_PROCESSI)/draws.o \
				$(OBJ_DIR_PROCESSI)/main.o \
				$(OBJ_DIR_PROCESSI)/funzionegioca.o \
				$(OBJ_DIR_PROCESSI)/gestionegrafica.o \
				$(OBJ_DIR_PROCESSI)/menu.o \
				$(OBJ_DIR_PROCESSI)/rana.o \
				$(OBJ_DIR_PROCESSI)/windowgeneration.o

OBJECTS_THREADS = $(OBJ_DIR_THREADS)/gestionecoccodrilli.o \
				$(OBJ_DIR_THREADS)/collisioni.o \
				$(OBJ_DIR_THREADS)/disegni.o \
				$(OBJ_DIR_THREADS)/threads1.o \
				$(OBJ_DIR_THREADS)/comunicazione.o \
				$(OBJ_DIR_THREADS)/gestionegrafica.o \
				$(OBJ_DIR_THREADS)/windowgeneration.o

CFLAGS = -g -Wall -I/usr/include  -I/usr/include/x86_64-linux-gnu # Include ncurses headers if needed
LDFLAGS = -lncurses

PROCESSI_EXEC = $(BUILD_DIR)/versione_processi/a.out
THREADS_EXEC = $(BUILD_DIR)/versione_threads/a.out

# Default target: build both projects
#all: $(PROCESSI_EXEC) $(THREADS_EXEC)

# Project 1 executable (build ont one executable)
#$(PROCESSI_EXEC): $(OBJECTS_PROCESSI)
#	gcc $(OBJECTS_PROCESSI) -o $(PROCESSI_EXEC) $(LDFLAGS)
#
#$(OBJ_DIR_PROCESSI)/%.o: $(PROCESSI_SRC)/%.c
#	gcc $(CFLAGS) -c $< -o $@
#
#clean:
#	rm -f $(OBJ_DIR)*.o $(BUILD_DIR)/versione_processi/a.out

# Project 1 executable (build ont one executable)
$(THREADS_EXEC): $(OBJECTS_THREADS)
	gcc $(OBJECTS_THREADS) -o $(THREADS_EXEC) $(LDFLAGS)

$(OBJ_DIR_THREADS)/%.o: $(THREADS_SRC)/%.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)*.o $(BUILD_DIR)/versione_threads/a.out
