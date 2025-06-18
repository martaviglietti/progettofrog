


#include "header.h"



int main(){
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
    

    while (true){
    	if (ricomincia) {  //se 'ricomincia' è 0 non torniamo al menu;
            scelta= menu(game,"Menu Principale", OPZIONI, 3);  //se 'ricomincia' è 0 non torniamo al menu;
        }	
	
        if (scelta == 0) { 
         
   	    if (ricomincia) {  //se 'ricomincia' è 0 manteniamo la stessa difficoltà
                difficoltà=scegliDifficolta(game);
            }
        
            impostazioni_gioco(&stat_game, difficoltà);
            
        
        
        
            risultato=startGame(game, stat_game);
            
            if (risultato.game==1) {  //Vittoria
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

void impostazioni_gioco(Stat_game* stat_game, int difficoltà ){
    switch (difficoltà) {
                case 0:  //Facile
                    stat_game->vite=10;			   //numero vite
                    stat_game->tempo=100;		   //tempo di gioco
                    stat_game->velocità_proiettili=30000;   //velocità proiettili
                    stat_game->velocità_coccodrilli=100000;  //velocità coccodrilli
                    break;
                case 1:  //Medio
                    stat_game->vite=10;
                    stat_game->tempo=30;
                    stat_game->velocità_proiettili=30000;
                    stat_game->velocità_coccodrilli=50000;
                    break;
                case 2:  //Difficile
                    stat_game->vite=10;
                    stat_game->tempo=30;
                    stat_game->velocità_proiettili=3000;
                    stat_game->velocità_coccodrilli=50000;
                    break;      
            }
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

