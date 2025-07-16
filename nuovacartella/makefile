# Compilatore
CC = gcc

# Flag per la compilazione
CFLAGS = -Wall -Wextra -g

# Librerie da linkare
LIBS = -lncursesw -lpthread


#nome output
TARGET = frog_game
    
   
SRCS = main.c 
   
   
   


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
