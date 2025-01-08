#include "menu.h"
#include <string.h>

// Array di stringhe che rappresentano le opzioni del menu
static const char *OPZIONI[] = {
    "Inizia gioco", "Crediti", "Esci"
};

// Funzione che gestisce il menu
int menu(WINDOW *game) {
    nodelay(game, false); // Rendo bloccante la getch
    keypad(game, true);   // Abilito l'input da tastiera

    char choice;
    int position = 0;

    int gameLINES = getmaxy(game);
    int gameCOLS = getmaxx(game);

    while (true) {
        wclear(game);
        box(game, ACS_VLINE, ACS_HLINE);

        mvwprintw(game, gameLINES / 2 - 5, gameCOLS / 2 - 4, "Frogger");
        for (int i = 0; i < 3; i++) {
            if (position == i) {
                wattron(game, COLOR_PAIR(12));
            } else {
                wattron(game, COLOR_PAIR(1));
            }

            int x = gameCOLS / 2 - (strlen(OPZIONI[i]) / 2);
            mvwprintw(game, gameLINES / 2 - 1 + i, x, "%s", OPZIONI[i]);

            if (position == i) {
                wattroff(game, COLOR_PAIR(12));
            } else {
                wattroff(game, COLOR_PAIR(1));
            }
        }

        choice = wgetch(game);
        switch (choice) {
            case 2: // Freccia giù
                position++;
                if (position >= 3) {
                    position = 0;
                }
                break;
            case 3: // Freccia su
                position--;
                if (position < 0) {
                    position = 2;
                }
                break;
            case 10: // Invio
                if (position == 0 || position == 2) {
                    wrefresh(game);
                    flushinp();
                    nodelay(game, true);
                    keypad(game, false);
                    return position;
                } else {
                    credits(game);
                }
                break;
        }

        wrefresh(game);
        flushinp();
    }

    nodelay(game, true);
    keypad(game, false);
    return 0;
}

// Funzione che mostra i crediti
void credits(WINDOW *game) {
    wclear(game);
    box(game, ACS_VLINE, ACS_HLINE);

    int gameLINES = getmaxy(game);
    int gameCOLS = getmaxx(game);

    mvwprintw(game, gameLINES / 2 - 2, gameCOLS / 2 - 15, "Marta Viglietti");
    mvwprintw(game, gameLINES / 2 - 1, gameCOLS / 2 - 15, "Gabriele Stampatori");
    mvwprintw(game, gameLINES / 2 + 1, gameCOLS / 2 - 17, "Premi un tasto per tornare al menu...");

    wrefresh(game);
    flushinp();
    wgetch(game);
}
