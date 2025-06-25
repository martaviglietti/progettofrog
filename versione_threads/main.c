#include "header.h"
int main(){
    pthread_mutex_t mutex_ncurses = PTHREAD_MUTEX_INITIALIZER;

    printf("\e[8;%d;%dt", 49, 81);  //ridimensioniamo il terminale
    fflush(stdout);
    setlocale(LC_ALL, "");     // Abilita UTF-8	
    sleep(1);	

    //Inizializziamo ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    resizeterm(49, 81);  //avvisiamo ncurses del cambio di dimensioni del terminale
    creazione_colori();    
    srand(time(NULL));
   
    int height = LINES;
    int width = COLS;
    int scelta;	     //variabile contenente la scelta dal menù principale
    int difficoltà;  //variabile contenente la difficoltà scelta dal menù delle difficoltà
    int ricomincia=1;
    
    Stat_game stat_game={0,0,0,0};
    Game_struct risultato;  //conterrà i risultati del game 
    WINDOW *game = newwin(height, width, 0, 0);  //finestra dell'area gioco  
    
    pthread_mutex_t mutex_tane = PTHREAD_MUTEX_INITIALIZER;
    
    


    while (true){
    	if (ricomincia) {  //se 'ricomincia' è 0 non torniamo al menu;
            scelta= menu(game,"Menu Principale", OPZIONI, 3);  //se 'ricomincia' è 0 non torniamo al menu;
        }	
        if (scelta == 0) { 
            if (ricomincia) {  //se 'ricomincia' è 0 manteniamo la stessa difficoltà
                    difficoltà=scegliDifficolta(game);
                }
            switch (difficoltà) {
                case 0:  //Facile
                    stat_game.vite=10;			   //numero vite
                    stat_game.tempo=100;		   //tempo di gioco
                    stat_game.velocità_proiettili=30000;   //velocità proiettili
                    stat_game.velocità_coccodrilli=100000;  //velocità coccodrilli
                    break;
                case 1:  //Medio
                    stat_game.vite=10;
                    stat_game.tempo=30;
                    stat_game.velocità_proiettili=30000;
                    stat_game.velocità_coccodrilli=50000;
                    break;
                case 2:  //Difficile
                    stat_game.vite=10;
                    stat_game.tempo=30;
                    stat_game.velocità_proiettili=3000;
                    stat_game.velocità_coccodrilli=50000;
                    break;      
            }
            risultato=startGame(game,stat_game);
            if (risultato.game_over==1) {  //Vittoria
                scelta=gameWin(game,risultato.score); 
            	
            	if (scelta) {	
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con stessa difficoltà
            	}	
            } else {  //Sconfitta
            	scelta=gameOver(game,risultato.score);
            	if (scelta) {					
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con stessa difficoltà
            	} 	
            }
        
        } else if (scelta==1) {  //crediti
            credits(game);
        	
        } else {  //Uscita
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

int gameWin(WINDOW *game, int score){  //schermata in caso di vittoria
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


int gameOver(WINDOW *game, int score){  //schermata in caso di perdita
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



int scegliDifficolta(WINDOW *game) {  //schermata per scelta della difficoltà
    const char *difficolta[] = {"Facile", "Media", "Difficile"};
    return menu(game, "Scegli la Difficoltà", difficolta, 3);
}



Game_struct startGame(WINDOW *game, Stat_game stat_game){			
    //Inizializziamo variabili di gestione della partita
    int tane_occupate=0;
    Game_struct game_struct;
    game_struct.score=0; 		//contiene lo score di tutto il game
    game_struct.vite=stat_game.vite;    //contiene numero di vite rimaste
    game_struct.tempo=stat_game.tempo;  //contiene tempo rimasto nella manche
    game_struct.game_over=0;
    
    //inizializzazione delle tane a 0
    for (int i=0;i<TANE-1;i++) {
        game_struct.tane[i]=0;
    }

    //mutex per la struttura
    pthread_mutex_init(&game_struct.mutex_game, NULL);

    //inizializzaizone del buffer produttore-consumatore
    inizializza_buffer();

    //Definizione dei flussi
    Flusso flussi[8];
    def_vel_flussi(flussi,stat_game.velocità_coccodrilli); //definisco velocità di ogni flusso                
   
    //creaizone dei thread principali
    crea_thread_gioco(&game_struct, flussi, stat_game.velocità_proiettili, stat_game.tempo, game);
    int thread_granata_attivo = 0;

    //inizio della partita
    while (true) {
    game_struct.tempo=stat_game.tempo; 
    def_dir_flussi(flussi);  //definisce direzione di ogni flusso					

  
        for (int i=0;i<5;i++) {
            if(game_struct.tane[i]==1){
                tane_occupate++;
            }
        }
        if (tane_occupate==5) {		
            game_struct.game_over=1;
            game_struct.score+=100;
            return game_struct;  //uscita per tane chiuse
        }
        tane_occupate=0;
        wclear(game);
        wrefresh(game);     
        }
}



		
//funzione che mostra la barra del tempo rimanente
void print_tempo(WINDOW* game,Game_struct* game_struct, int tempo){
    wattron(game, COLOR_PAIR(7));
    mvwhline(game,46,15, ' ', (int)(62*((float)game_struct->tempo/tempo)));
    wattroff(game, COLOR_PAIR(7));
}





//funzione che assegna una velocità a ogni flusso
void def_vel_flussi(Flusso *flussi, int vel){
	
    int altezza_base=37;  //posizione del primo flusso (quello più in basso)
    int dimensione_flussi=3;  //larghezza dei flussi
    for (int i=0;i<8;i++) {
        flussi[i].y=altezza_base - i*dimensione_flussi;
	    flussi[i].speed= rand_funz(vel - 10000,vel+10000);
    }
}

//funzione che assegna una direzione ad ogni flusso
void def_dir_flussi(Flusso *flussi){
    for (int i=0;i<8;i++) {
	if (i>=1) {  //tutti i flussi dal secondo in poi hanno direzione opposta al flusso precedente
	    flussi[i].dir= -flussi[i-1].dir;

	} else {
	    //impostiamo la direzione del primo flusso in modo randomico
	    flussi[i].dir= rand()%2;
	    if (flussi[i].dir==0) flussi[i].dir=1;
	    else flussi[i].dir=-1;
	}
    }	
}





   

 


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