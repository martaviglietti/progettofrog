#include "header.h"
#include <pthread.h>
#include <stdlib.h>

//___________________________________________________________________________________________________
//funzione utile per creare un numero random tra un minimo e un massimo (compresi)
int rand_funz(int min, int max){
    return min + rand() % (max-min+1);
}

void push_event(messageBuffer* b, Message* m) {
    sem_wait(&b->empty); // wait for free slot
    pthread_mutex_lock(&b->mutex);

    b->buffer[b->head] = *m;
    b->head = (b->head + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&b->mutex);
    sem_post(&b->full); // signal new message
}

Message pop_event(messageBuffer* b) {
    
    sem_wait(&b->full); // wait for available message
    pthread_mutex_lock(&b->mutex);

    Message m = b->buffer[b->tail];
    b->tail = (b->tail + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&b->mutex);
    sem_post(&b->empty); // signal one more free slot
    return m;
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

    //Definizione dei flussi
    fluxInit(gameConfig); //definisco velocità di ogni flusso

    //Buffer initialization
    bufferInit();
    
    // Initializzazione stato di gioco
    printf("Inizializziamo variabili di gestione della partita");
    Game_struct* game_struct =  malloc(sizeof(Game_struct));
    if (!game_struct) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    game_struct->score=0; 		          //contiene lo score di tutto il game
    game_struct->vite = gameConfig->vite;    //contiene numero di vite rimaste
    game_struct->win=0;
    game_struct->gameCfg = gameConfig;
    game_struct->game = game;
    
    for (int i=0;i<NTANE-1;i++) {
        game_struct->tane[i]=0;
    }
    game_struct->tane_count = 0;

    //inizio della partita - loop di gioco
    pthread_t t_graph;
    pthread_create(&t_graph, NULL, thread_grafica, (void *)game_struct);
    pthread_join(t_graph, NULL);

    return game_struct;
}

//______________________________________________________________________________________________________________________
//CREAZIONE THREAD PRINCIPALI GIOCO -------------------------------------------------------------------------
void bufferInit(){

    myBuffer.head = 0;
    myBuffer.tail = 0;

    // Initialize semaphores
    // empty = BUFFER_SIZE slots free at start
    sem_init(&myBuffer.empty, 0, BUFFER_SIZE);

    // full = 0 items available initially
    sem_init(&myBuffer.full, 0, 0);

    // Initialize the mutex
    pthread_mutex_init(&myBuffer.mutex, NULL);
    pthread_mutex_init(&myBuffer.mutexProj, NULL);

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


