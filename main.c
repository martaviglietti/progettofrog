#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "menu.h" // Include il file header del menu

int main() {
    // Imposta la dimensione del terminale
    printf("\e[8;%d;%dt", 30, 80); // Dimensione predefinita: 30 righe x 80 colonne
    fflush(stdout);
    sleep(1); // Concede al sistema il tempo di ridimensionare il terminale

    // Inizializza NCURSES
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    // Controllo del supporto colori
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Colore normale
        init_pair(12, COLOR_WHITE, COLOR_BLUE);  // Colore evidenziato
    }

    // Ottieni le dimensioni del terminale
    int height = LINES;
    int width = COLS;

    // Crea la finestra principale (contenitore)
    WINDOW *game = newwin(LINES, COLS, 0, 0); // Finestra che occupa tutto lo schermo
    box(game, 0, 0); // Disegna il bordo della finestra principale
    wrefresh(game);

    // Chiama il menu e gestisce il risultato
    int result = menu(game);

    if (result == 0) {
        // L'utente ha selezionato "Inizia gioco"
        mvwprintw(game, height / 2, width / 2 - 10, "Inizio Gioco...");
        wrefresh(game);
        sleep(2); // Placeholder per il gioco
    } else if (result == 2) {
        // L'utente ha selezionato "Esci"
        mvwprintw(game, height / 2, width / 2 - 5, "Uscita...");
        wrefresh(game);
        sleep(1);
    }

    // Pulisci e termina NCURSES
    delwin(game);
    endwin();

    return 0;
}
