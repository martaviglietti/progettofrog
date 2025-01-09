#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "menu.h"

// Definizione delle costanti per le sezioni del gioco
#define TANE 5
#define FIUME 10
#define PRATO 5
#define MARCIAPIEDE 3

// Dichiarazione delle funzioni
void windowGeneration(WINDOW *game, int maxX, int maxY);
void startGame(WINDOW *game);

int main() {
    printf("\e[8;%d;%dt", 30, 80); // Dimensione predefinita: 30 righe x 80 colonne
    fflush(stdout);
    sleep(1);

    // Inizializza NCURSES
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    if (has_colors()) {       //verifica il supporto ai colori da parte del terminale
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Normale
        init_pair(12, COLOR_WHITE, COLOR_BLUE);  // Evidenziato
    }

    int height = LINES;
    int width = COLS;

    WINDOW *game = newwin(height, width, 0, 0);
    box(game, 0, 0);
    wrefresh(game);

    while (true) {
        int result = menu(game);

        if (result == 0) {  // "Inizia gioco"
            startGame(game);
        } else if (result == 2) {  // "Esci"
            wclear(game);
            mvwprintw(game, height / 2, width / 2 - 5, "Uscita...");
            wrefresh(game);
            sleep(1);
            break; // Esci dal programma
        }
    }

    delwin(game);
    endwin();
    return 0;
}

void windowGeneration(WINDOW *game, int maxX, int maxY) {
    start_color();
    init_pair(2, COLOR_WHITE, COLOR_BLUE);    // Fiume
    init_pair(3, COLOR_WHITE, COLOR_YELLOW);  // Tane
    init_pair(4, COLOR_BLACK, COLOR_RED);     // Marciapiede
    init_pair(5, COLOR_WHITE, COLOR_MAGENTA); // Punteggio e vite

    int offsetSum = 1;

    // Sezione superiore: Punteggio e vite
    wattron(game, COLOR_PAIR(5));
    mvwhline(game, offsetSum, 1, ' ', maxX - 2);
    mvwprintw(game, offsetSum, 2, "Punteggio: 0 | Vite: 3");
    wattroff(game, COLOR_PAIR(5));
    offsetSum += 1;

    // Sezione Tane
    for (int i = 0; i < TANE; i++) {
        wattron(game, COLOR_PAIR(3));
        mvwhline(game, offsetSum + i, 1, ' ', maxX - 2);
        wattroff(game, COLOR_PAIR(3));
    }
    offsetSum += TANE;

    // Sezione Fiume
    for (int i = 0; i < FIUME; i++) {
        wattron(game, COLOR_PAIR(2));
        mvwhline(game, offsetSum + i, 1, ' ', maxX - 2);
        wattroff(game, COLOR_PAIR(2));
    }
    offsetSum += FIUME;

  

    // Sezione Marciapiede
    for (int i = 0; i < MARCIAPIEDE; i++) {
        wattron(game, COLOR_PAIR(4));
        mvwhline(game, offsetSum + i, 1, ' ', maxX - 2);
        wattroff(game, COLOR_PAIR(4));
    }

    wrefresh(game);
}

















void startGame(WINDOW *game) {
    int maxX = getmaxx(game);
    int maxY = getmaxy(game);

    wclear(game);
    box(game, 0, 0);
    wrefresh(game);

    // Genera la finestra di gioco
    windowGeneration(game, maxX, maxY);

    // Ciclo per tornare al menu
    int ch;
    while (true) {
        mvwprintw(game, maxY - 2, 2, "Premi 'm' per tornare al menu...");
        wrefresh(game);

        ch = wgetch(game);
        if (ch == 'm' || ch == 'M') {
            break;
        }
    }

    // Pulizia finale
    wclear(game);
    box(game, 0, 0);
    wrefresh(game);
}


