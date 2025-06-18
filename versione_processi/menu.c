#include "header.h"
int menu(WINDOW *game, const char *title, const char *options[], int num_options){
    flushinp();  //elimina input residuo
    keypad(game, true);  //abilita l'input da tastiera

    int position = 0;
    int gameLINES = getmaxy(game);
    int gameCOLS = getmaxx(game);

    while (true) {
        wclear(game);
        box(game, ACS_VLINE, ACS_HLINE);

        mvwprintw(game, gameLINES / 2 - 5, gameCOLS / 2 - strlen(title)/2, "%s", title);
        for (int i = 0; i < num_options; i++) {
            if (position == i) {
                wattron(game, COLOR_PAIR(9));
            }

            int x = gameCOLS / 2 - (strlen(options[i]) / 2);
            mvwprintw(game, gameLINES / 2 - 1 + i, x, "%s", options[i]);

            
            wattroff(game, COLOR_PAIR(9));
        }

        int choice = wgetch(game);
        switch (choice) {
            case KEY_DOWN:
                position++;
                if (position >= num_options) position = 0;
                break;
            case KEY_UP:
                position--;
                if (position < 0) position = num_options-1;
                break;
            case 10: // Invio
                return position;
        }

        wrefresh(game);
    }
}


//funzione che mostra i crediti
void credits(WINDOW *game){
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


//funzione utile per creare un numero random tra un minimo e un massimo (compresi)
int rand_funz(int min, int max){

    return min + rand() % (max-min+1);

}


