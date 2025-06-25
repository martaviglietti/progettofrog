#include "header.h"

BufferC buffer;

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
    
    gameConfig gameConfig = {};
    Game_struct risultato;  //conterrà i risultati del game 
    risultato.game = newwin(height, width, 0, 0);  //finestra dell'area gioco  
    
    while (true){
    	if (ricomincia) {  //se 'ricomincia' è 0 non torniamo al menu;
            scelta= menu(risultato.game,"Menu Principale", OPZIONI, 3);  //se 'ricomincia' è 0 non torniamo al menu;
        }	
        if (scelta == 0) { 
            if (ricomincia) {  //se 'ricomincia' è 0 manteniamo la stessa difficoltà
                    difficoltà=scegliDifficolta(risultato.game);
                }
            switch (difficoltà) {
                case 0:  //Facile
                    gameConfig.vite=10;			   //numero vite
                    gameConfig.tempo=100;		   //tempo di gioco
                    gameConfig.velocità_proiettili=30000;   //velocità proiettili
                    gameConfig.velocità_coccodrilli=100000;  //velocità coccodrilli
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
            risultato=startGame(risultato.game, gameConfig);
            if (risultato.game_over==1) {  //Vittoria
                scelta=gameWin(risultato.game,risultato.score); 
            	
            	if (scelta) {	
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con stessa difficoltà
            	}	
            } else {  //Sconfitta
            	scelta=gameOver(risultato.game,risultato.score);
            	if (scelta) {					
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con stessa difficoltà
            	} 	
            }
        
        } else if (scelta==1) {  //crediti
            credits(risultato.game);
        	
        } else {  //Uscita
            wclear(risultato.game);
            mvwprintw(risultato.game, height / 2, width / 2 - 5, "Uscita...");
            wrefresh(risultato.game);
            sleep(1);
            break; // Esci dal programma
        }
    }

    delwin(risultato.game);
    endwin();
    return 0;
}
