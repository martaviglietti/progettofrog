#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "rana.h"

const char *frog_sprite[ALTEZZARANA] = {
    "@ @",
    " - "
};



void funzione_rana(int pipe_fd[2]) {
    WINDOW *game = newwin(36, 85, 0, 0); // Crea una nuova finestra di gioco
    frog(game, pipe_fd); // Avvia il processo della rana
    delwin(game); // Elimina la finestra di gioco
}



// Funzione per disegnare la rana
void draw_frog(WINDOW *game, int x, int y) {
    for (int i = 0; i < ALTEZZARANA; i++) {
        for (int j = 0; j < LARGHEZZARANA; j++) {
            mvwaddch(game, y + i, x + j, frog_sprite[i][j]);
        }
    }
    wrefresh(game);
}

// Funzione per cancellare la rana
void delete_frog(WINDOW *game, int x, int y) {
    for (int i = 0; i < ALTEZZARANA; i++) {
        for (int j = 0; j < LARGHEZZARANA; j++) {
            mvwaddch(game, y + i, x + j, ' ');
        }
    }
    wrefresh(game);
}

void granata(int startX, int startY, WINDOW *game) {
    int posX_sinistra = startX - 1; // Posizione iniziale a sinistra
    int posX_destra = startX + LARGHEZZARANA; // Posizione iniziale a destra
    int posY = startY;

    int alive_sinistra = 1; // Stato attivo della granata a sinistra
    int alive_destra = 1;   // Stato attivo della granata a destra


//continuo finche almeno una delle granta è attiva
    while (alive_sinistra || alive_destra) {
        // Cancella le posizioni precedenti
        if (alive_sinistra) mvwaddch(game, posY, posX_sinistra, ' ');
        if (alive_destra) mvwaddch(game, posY, posX_destra, ' ');

        // Aggiorna le posizioni
        if (alive_sinistra) 
            posX_sinistra--;
        if (alive_destra) 
            posX_destra++;

        // Controlla i bordi dello schermo
        if (posX_sinistra < 1) alive_sinistra = 0; // La granata sinistra esce dallo schermo
        if (posX_destra >= getmaxx(game) - 1) alive_destra = 0; // La granata destra esce dallo schermo

        // Disegna le granate nelle nuove posizioni
        if (alive_sinistra) {
            wattron(game, COLOR_PAIR(8)); // Colore della granata
            mvwaddch(game, posY, posX_sinistra, '*');
            wattroff(game, COLOR_PAIR(8));
        }
        if (alive_destra) {
            wattron(game, COLOR_PAIR(8));
            mvwaddch(game, posY, posX_destra, '*');
            wattroff(game, COLOR_PAIR(8));
        }

        wrefresh(game);
        usleep(GRANATA_VELOCITA); // Velocità del movimento
    }

    // Cancella i residiu delle granate(?)
    if (alive_sinistra) mvwaddch(game, posY, posX_sinistra, ' ');
    if (alive_destra) mvwaddch(game, posY, posX_destra, ' ');
    wrefresh(game);

    _exit(0); // Termina il processo
}


// Funzione per sparare una granata
void sparaGranata(int startX, int startY, WINDOW *game) {
    pid_t pid = fork(); // Crea un nuovo processo per la granata

    if (pid < 0) {
        perror("Errore nella creazione del processo granata");
        return;
    }

    if (pid == 0) {
        // Processo figlio: gestisce la granata
        granata(startX, startY, game);
    }
}


void funzione_rana(int pipe_fd[2]) {
    WINDOW *game = newwin(36, 85, 0, 0); // Crea una nuova finestra di gioco
    frog(game, pipe_fd); // Avvia il processo della rana
    delwin(game); // Elimina la finestra di gioco
}

// Processo della rana
void frog(WINDOW *game, int pipe_fd[2]) {
    int x = 40, y = 28; // Posizione iniziale della rana (nel marciapiede)
    int key;

    // Abilita l'input da tastiera
    keypad(stdscr, true);
    nodelay(stdscr, true); // Input non bloccante

    while (1) {
        // Disegna la rana
        draw_frog(game, x, y);

        // Cattura l'input da tastiera
        key = getch();

        // Cancella la rana dalla posizione precedente
        delete_frog(game, x, y);

        switch (key) {
            case KEY_UP:
                y = (y > 1) ? y - 1 : y;
                break;
            case KEY_LEFT:
                x = (x > 1) ? x - 1 : x;
                break;
            case KEY_RIGHT:
                x = (x < getmaxx(game) - LARGHEZZARANA - 1) ? x + 1 : x;
                break;
            case 's': // Sparare granata a sinistra
                sparaGranata(x, y, game);
                break;
            case 'q': // Esci dal gioco
                return;
        }

        // Invia posizione aggiornata al processo principale tramite pipe
        write(pipe_fd[1], &x, sizeof(int));
        write(pipe_fd[1], &y, sizeof(int));

        usleep(30000); // Riduce il carico sulla CPU
    }
}
