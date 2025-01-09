#include "menu.h"
#include <string.h>

// Opzioni del menu
static const char *OPZIONI[] = {
    "Inizia gioco", "Crediti", "Esci"
};

int menu(WINDOW *game) {
    flushinp();            // Elimina input residuo
    keypad(game, true);    // Abilita l'input da tastiera

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

            wattroff(game, COLOR_PAIR(1));
            wattroff(game, COLOR_PAIR(12));
        }

        int choice = wgetch(game);
        switch (choice) {
            case KEY_DOWN:
                position++;
                if (position >= 3) position = 0;
                break;
            case KEY_UP:
                position--;
                if (position < 0) position = 2;
                break;
            case 10: // Invio
                return position;
        }

        wrefresh(game);
    }
}

void credits(WINDOW *game) {
    wclear(game);
    box(game, ACS_VLINE, ACS_HLINE);

    int gameLINES = getmaxy(game);
    int gameCOLS = getmaxx(game);

    mvwprintw(game, gameLINES / 2 - 2, gameCOLS / 2 - 15, "Marta Viglietti");
    mvwprintw(game, gameLINES / 2 - 1, gameCOLS / 2 - 15, "Gabriele Stampatori");
    mvwprintw(game, gameLINES / 2 + 1, gameCOLS / 2 - 17, "Premi un tasto per tornare al menu...");

    wrefresh(game);
    wgetch(game);
}
