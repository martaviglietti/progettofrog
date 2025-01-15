#ifndef RANA_H
#define RANA_H

#include <ncurses.h>

// Definizione delle costanti
#define ALTEZZARANA 2     // Altezza dello sprite della rana
#define LARGHEZZARANA 3   // Larghezza dello sprite della rana
#define GRANATA_VELOCITA 50000 // Velocità di movimento della granata (in microsecondi)

// Sprite della rana (dichiarato nel file di implementazione)
extern const char *frog_sprite[ALTEZZARANA];

// Struttura per la gestione delle granate
typedef struct {
    int x;          // Posizione X
    int y;          // Posizione Y
    int direction;  // Direzione: -1 (sinistra), 1 (destra)
    int alive;      // Stato: 1 se attivo, 0 se terminato
} Proiettile;

// Dichiarazioni delle funzioni relative alla rana
void draw_frog(WINDOW *game, int x, int y);  // Disegna la rana
void delete_frog(WINDOW *game, int x, int y); // Cancella la rana

// Dichiarazioni delle funzioni relative alla gestione delle granate
void granata(int startX, int startY, int direction, WINDOW *game); // Processo per la gestione della granata
void sparaGranata(int startX, int startY, int direction, WINDOW *game); // Funzione per sparare una granata

// Dichiarazione del processo della rana
void frog(WINDOW *game, int pipe_fd[2]); // Processo per la gestione della rana

#endif // RANA_H
