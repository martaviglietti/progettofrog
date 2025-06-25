#include "header.h"

// Opzioni del menu
const char *opzioni[] = {
    "Inizia gioco", "Crediti", "Esci"
};

int menu(WINDOW *finestra_gioco, const char *title, const char *options[], int num_options){
    flushinp();  //elimina input residuo
    keypad(finestra_gioco, true);  //abilita l'input da tastiera

    int posizione = 0;
    int gameLINES = getmaxy(finestra_gioco);
    int gameCOLS = getmaxx(finestra_gioco);
    int scelta;

    while (true) {
        wclear(finestra_gioco);
        box(finestra_gioco, ACS_VLINE, ACS_HLINE);

        mvwprintw(finestra_gioco, gameLINES / 2 - 5, gameCOLS / 2 - strlen(title)/2, "%s", title);
        
        //mostriamo le opzioni
        for (int i = 0; i < num_options; i++) {
            if (posizione == i) {
                wattron(finestra_gioco, COLOR_PAIR(9));
            }

            int x = gameCOLS / 2 - (strlen(options[i]) / 2);
            mvwprintw(finestra_gioco, gameLINES / 2 - 1 + i, x, "%s", options[i]);

            
            wattroff(finestra_gioco, COLOR_PAIR(9));
        }
        
        //gestiamo l'eventuale movimento del cursore sulle opzioni
        scelta = wgetch(finestra_gioco);
        switch (scelta) {
            case KEY_DOWN:
                posizione++;
                if (posizione >= num_options) posizione = 0;
                break;
            case KEY_UP:
                posizione--;
                if (posizione < 0) posizione = num_options-1;
                break;
            case 10: // Invio
                return posizione;
        }

        wrefresh(finestra_gioco);
    }
}


//funzione che mostra i crediti
void crediti(WINDOW *finestra_gioco){
    wclear(finestra_gioco);
    box(finestra_gioco, ACS_VLINE, ACS_HLINE);

    int gameLINES = getmaxy(finestra_gioco);
    int gameCOLS = getmaxx(finestra_gioco);

    mvwprintw(finestra_gioco, gameLINES / 2 - 2, gameCOLS / 2 - 15, "Marta Viglietti");
    mvwprintw(finestra_gioco, gameLINES / 2 - 1, gameCOLS / 2 - 15, "Gabriele Stampatori");
    mvwprintw(finestra_gioco, gameLINES / 2 + 1, gameCOLS / 2 - 17, "Premi un tasto per tornare al menu...");

    wrefresh(finestra_gioco); 
    wgetch(finestra_gioco);
}


//funzione utile per creare un numero numero_random tra un minimo e un massimo (compresi)
int numero_random(int min, int max){

    return min + rand() % (max-min+1);
}
