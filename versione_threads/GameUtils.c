#include "header.h"
#include <stdlib.h>

//___________________________________________________________________________________________________
//funzione utile per creare un numero random tra un minimo e un massimo (compresi)
int rand_funz(int min, int max){
    return min + rand() % (max-min+1);
}

//___________________________________________________________________________________________________
int menu(WINDOW *game, const char *title, const char *options[], int num_options){
    flushinp();  // Elimina input residuo
    keypad(game, true);  // Abilita l'input da tastiera

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


//___________________________________________________________________________________________________
//schermata per scelta della difficoltà
int scegliDifficolta(WINDOW *game) { 
    const char *difficolta[] = {"Facile", "Media", "Difficile"};
    return menu(game, "Scegli la Difficoltà", difficolta, 3);
}

//___________________________________________________________________________________________________
//Inizializzazione del gioco e loop di gioco
Game_struct* startGame(WINDOW *game, gameConfig *gameConfig){		
    
    printf("Inizializziamo variabili di gestione della partita\n");
    buffer.buffer[IDX_GAME] = malloc(sizeof(Game_struct));
    if (buffer.buffer[IDX_GAME]== NULL) {
        fprintf(stderr, "malloc failed at Time Initialization at index %d\n", IDX_GAME);
        exit(EXIT_FAILURE);
    }
    
    Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];
    game_struct->game = game;
    game_struct->score=0; 		//contiene lo score di tutto il game
    game_struct->vite=gameConfig->vite;    //contiene numero di vite rimaste
    game_struct->win=0;
    game_struct->tempo = gameConfig->tempo;  //contiene tempo rimasto nella manche
    printf("Inizializziamo tempo a %f\n", game_struct->tempo);
    
    //inizializzazione delle tane a 0
    for (int i=0;i<NTANE-1;i++) {
        game_struct->tane[i]=0;
    }

    //Definizione dei flussi
    fluxInit(gameConfig); //definisco velocità di ogni flusso
    
    // Inizializzazione oggetti
    frogInit();
    GranateInit();
    CrocodileInit(gameConfig->flussi);
    ProjectileInit();
   
    //creaizone dei thread principali e inizilizzazione degli oggetti
    crea_thread_gioco(gameConfig);

    //inizio della partita - loop di gioco
    int tane_occupate=0;
    while (1) {		
        
        pthread_mutex_lock(&buffer.mutex);

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            pthread_mutex_unlock(&buffer.mutex);
            break;
        }

        tane_occupate=0;
        for (int i=0;i<NTANE;i++) {
            if(game_struct->tane[i]==1){
                tane_occupate++;
            }
        }
        if (tane_occupate==5) {		
            game_struct->win = 1;
            game_struct->score += 100;
              //uscita per tane chiuse
        }

        pthread_mutex_unlock(&buffer.mutex);
        
        //wclear(game_struct->game);
        //wrefresh(game_struct->game);     
    }
    exit(1);
    // !!!!!! funzione per eliinare gli oggetti e liberare memoria
    return game_struct;
}

//______________________________________________________________________________________________________________________
//CREAZIONE THREAD PRINCIPALI GIOCO -------------------------------------------------------------------------
void crea_thread_gioco(gameConfig *gameConfig){

    //inizializzaizone del buffer produttore-consumatore
    pthread_mutex_init(&buffer.mutex, NULL);

    // Thread grafica/consumatore
    pthread_t t_grafica, t_rana, t_tempo;
    pthread_t t_coccodrilli, t_proiettili, t_granate;

    // Creazione threads
    printf("...lancio i threads...\n");
    //pthread_create(&t_grafica, NULL, Gestione_grafica, (void*)gameConfig);
    pthread_create(&t_rana, NULL, thread_rana, (void *)gameConfig);
    pthread_create(&t_tempo, NULL, thread_tempo, (void *)gameConfig);
    pthread_create(&t_coccodrilli, NULL, thread_coccodrillo, (void*)gameConfig);
    //pthread_create(&t_granate, NULL, thread_granata, (void *)gameConfig);
    //pthread_create(&t_proiettili, NULL, thread_proiettile, (void *)gameConfig);
    
}

//___________________________________________________________________________________________________
//funzione che rinizializza la manche
void newManche(Game_struct* game_struct, gameConfig* gameConfig){
    game_struct->tempo = gameConfig->tempo;

    Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
    frog->x = RANA_XINIT;
    frog->y = RANA_YINIT;
    frog->alive = true;
    frog->tempo_prec = -1;

}

//_________________________________________________________________________________________________
//schermata in caso di vittoria
int gameWin(WINDOW *game, int score){  
    wclear(game);
    box(game, ACS_VLINE, ACS_HLINE);
    mvwprintw(game, 10, 10, "HAI VINTO! Punteggio: %d. Vuoi giocare ancora? (s/n)", score);
    wrefresh(game);
    char decision = wgetch(game);
    while (decision!= 's' && decision!='n') {
        decision = wgetch(game);
    }
    if (decision == 's') {  //se rispondiamo di si ripetiamo il game (con stessa difficoltà) da capo
        return 0;
    } else {  //altrimenti torniamo al menù
        return 1;
    }
}

//___________________________________________________________________________________________________
//schermata in caso di perdita
int gameOver(WINDOW *game, int score){  
    wclear(game);
    box(game, ACS_VLINE, ACS_HLINE);
    mvwprintw(game, 10, 10, "GAME OVER! Punteggio: %d. Vuoi giocare ancora? (s/n)", score);
    wrefresh(game);
    char decision = wgetch(game);
    while (decision!= 's' && decision!='n') {
    	 decision = wgetch(game);
    }
    if (decision == 's') {  //se rispondiamo di si ripetiamo il game (con stessa difficoltà) da capo
        return 0;
    } else {  //altrimenti torniamo al menù
        return 1;
    }
}

//___________________________________________________________________________________________________
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