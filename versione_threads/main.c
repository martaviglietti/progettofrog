#include "main.h"
#include "collisioni.h"
#include "FrogHandle.h"
#include "CrocHandle.h"
#include "Partita.h"
#include "Draws.h"


Temp buffer[DIM_BUFFER];
int indice_scrittura = 0;
int indice_lettura = 0;

pthread_mutex_t semaforo_buffer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semaforo_disegno = PTHREAD_MUTEX_INITIALIZER;

sem_t semafori_coccodrilli[NUMERO_COCCODRILLI];
sem_t spazi_occupati;
sem_t spazi_liberi;

int fine_gioco = 0;



int main(){
    printf("\e[8;%d;%dt", 49, 81);  //ridimensioniamo il terminale
    fflush(stdout);
    setlocale(LC_ALL, "");  //abilita UTF-8	
    sleep(1);  //pausa scelta da noi

    //Inizializziamo ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    resizeterm(49, 81);  //avvisiamo ncurses del cambio di dimensioni del terminale
    creazione_colori();  
      
    srand(time(NULL));
    
    int scelta;	        //variabile contenente la scelta dal menù principale
    int difficoltà;     //variabile contenente la difficoltà scelta dal menù delle difficoltà
    int ricomincia=1;  
    
    Parametri parametri_gioco={0,0,0,0,0};
    Statistiche  risultato;  //conterrà i risultati della partita
    WINDOW *finestra_gioco = newwin(LINES, COLS, 0, 0);  //finestra dell'area di gioco
    

    while (true){
    	if (ricomincia) {  //se 'ricomincia' è 0 non torniamo al menu;
            scelta= menu(finestra_gioco,"Menu Principale", opzioni, 3);  //se 'ricomincia' è 0 non torniamo al menu;
        }	
	
        if (scelta == 0) { 
         
   	    if (ricomincia) {  //se 'ricomincia' è 0 manteniamo la stessa difficoltà
                difficoltà=scelta_difficoltà(finestra_gioco);  //richiesta di quale difficoltà affrontare
            }
        
            impostazioni_gioco(&parametri_gioco, difficoltà);  //impostiamo il gioco in base alla difficoltà scelta
            
            risultato=Partita(finestra_gioco, parametri_gioco);  //facciamo partire il gioco
            
            if (risultato.gioco==1) {  //Vittoria
                scelta=game_vinto(finestra_gioco,risultato.punteggio); 
            	
            	if (scelta) {	
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con la stessa difficoltà
            	}	
            } else {  //Sconfitta
            	scelta=game_perso(finestra_gioco,risultato.punteggio);
            	if (scelta) {					
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con la stessa difficoltà
            	} 	
            }
        
        } else if (scelta==1) {  //schermata dei crediti
            crediti(finestra_gioco);
        	
        } else {  //richiesta di uscita dal programma
            wclear(finestra_gioco);
            mvwprintw(finestra_gioco, LINES / 2, COLS / 2 - 5, "Uscita...");
            wrefresh(finestra_gioco);
            sleep(1);  //pausa scelta da noi
            break; 
        }
    }

    delwin(finestra_gioco);
    endwin();
    return 0;
}




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

//funzione che gestisce il tempo
void* funzione_tempo(){
    Temp tempo={0,0,0,0};		
    tempo.id=ID_TIME;
    while (true) {
	sleep(1);
	scrittura_buffer(tempo); //manda un messaggio alla funzione di controllo ogni secondo
    }
}
