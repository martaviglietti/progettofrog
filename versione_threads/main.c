#include "header.h"
#include <ncurses.h>

void seed_random() {
    srand((unsigned int)time(NULL));
}

messageBuffer myBuffer;

int main(){

    const char *OPZIONI[] = {
        "Inizia gioco", "Crediti", "Esci"
    };

    seed_random(); 
    
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
    int scelta = 0;	     //variabile contenente la scelta dal menù principale
    int difficoltà = 0;  //variabile contenente la difficoltà scelta dal menù delle difficoltà
    int ricomincia=1;
   
    gameConfig gameConfig = {};
    WINDOW* game = newwin(height, width, 0, 0);  //finestra dell'area gioco  
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
                    gameConfig.vite=10;			   //numero vite
                    gameConfig.tempo=30;		   //tempo di gioco
                    gameConfig.velocità_proiettili=15;   //velocità proiettili
                    gameConfig.velocità_coccodrilli=10;  //velocità coccodrilli
                    break;
                case 1:  //Medio
                    gameConfig.vite=10;
                    gameConfig.tempo=30;
                    gameConfig.velocità_proiettili=30000;
                    gameConfig.velocità_coccodrilli=50000;
                    break;
                case 2:  //Difficile
                    gameConfig.vite=10;
                    gameConfig.tempo=30;
                    gameConfig.velocità_proiettili=3000;
                    gameConfig.velocità_coccodrilli=50000;
                    break;      
            }
            Game_struct* risultato = startGame(game, &gameConfig);

            if (risultato->win) {  //Vittoria
                //scelta=gameWin(game,risultato->score); 
                scelta=0;
            	
            	if (scelta) {	
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con stessa difficoltà
            	}	
            } else {  //Sconfitta
            	scelta=gameOver(game,risultato->score);
            	if (scelta) {					
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con stessa difficoltà
            	} 	
            }
            free(risultato);
        
        } else if (scelta==1) {  //crediti
            credits(game);
        	
        } else {  //Uscita
            //wclear(game);
            //mvwprintw(game, height / 2, width / 2 - 5, "Uscita...");
            //wrefresh(game);
            //sleep(1);
            break; // Esci dal programma
        }
    }

    //delwin(game);
    endwin();
    return 0;
}
