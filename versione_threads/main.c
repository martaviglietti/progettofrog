#include "header.h"


Temp buffer[DIM_BUFFER];
int contatore=0;
int indice_scrittura=0;
int indice_lettura=0;
pthread_mutex_t semaforo_buffer= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semaforo_disegno= PTHREAD_MUTEX_INITIALIZER;

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

void impostazioni_gioco(Parametri* parametri_gioco, int difficoltà ){
    switch (difficoltà) {
        case 0:  //Facile
            parametri_gioco->livello_difficoltà=1;         //livello di difficoltà
            parametri_gioco->vite=10;			   //numero vite
            parametri_gioco->tempo=100;		           //tempo di gioco
            parametri_gioco->velocità_proiettili=40000;    //velocità proiettili
            parametri_gioco->velocità_coccodrilli=120000;  //velocità coccodrilli        
            break;
            
        case 1:  //Medio
            parametri_gioco->livello_difficoltà=2;
            parametri_gioco->vite=5;
            parametri_gioco->tempo=50;
            parametri_gioco->velocità_proiettili=30000;
            parametri_gioco->velocità_coccodrilli=80000;
            
            break;
        case 2:  //Difficile
            parametri_gioco->livello_difficoltà=3;
            parametri_gioco->vite=3;
            parametri_gioco->tempo=30;
            parametri_gioco->velocità_proiettili=20000;
            parametri_gioco->velocità_coccodrilli=60000;
            
            break;      
    }
}

int game_vinto(WINDOW *finestra_gioco, int punteggio){  //schermata in caso di vittoria
    wclear(finestra_gioco);
    box(finestra_gioco, ACS_VLINE, ACS_HLINE);
    mvwprintw(finestra_gioco, 10, 10, "HAI VINTO! Punteggio: %d. Vuoi giocare ancora? (s/n)", punteggio);
    wrefresh(finestra_gioco);
    char decisione = wgetch(finestra_gioco);
    while (decisione!= 's' && decisione!='n') {
        decisione = wgetch(finestra_gioco);
    }
    if (decisione == 's') {  //se rispondiamo di si ripetiamo il gioco (con la stessa difficoltà) da capo
        return 0;
    } else {  //altrimenti torniamo al menù
        return 1;
    }
}


int game_perso(WINDOW *finestra_gioco, int punteggio){  //schermata in caso di perdita
    wclear(finestra_gioco);
    box(finestra_gioco, ACS_VLINE, ACS_HLINE);
    mvwprintw(finestra_gioco, 10, 10, "finestra_gioco OVER! Punteggio: %d. Vuoi giocare ancora? (s/n)", punteggio);
    wrefresh(finestra_gioco);
    char decisione = wgetch(finestra_gioco);
    while (decisione!= 's' && decisione!='n') {
    	 decisione = wgetch(finestra_gioco);
    }
    if (decisione == 's') {  //se rispondiamo di si ripetiamo il gioco (con la stessa difficoltà) da capo
        return 0;
    } else {  //altrimenti torniamo al menù
        return 1;
    }
}



int scelta_difficoltà(WINDOW *finestra_gioco) {  //schermata per scelta della difficoltà
    const char *difficolta[] = {"Facile", "Media", "Difficile"};
    return menu(finestra_gioco, "Scegli la Difficoltà", difficolta, 3);
}



void scrittura_buffer(Temp messaggio){
    
    
    
    while(contatore == DIM_BUFFER)
    pthread_mutex_lock(&semaforo_buffer);
    buffer[indice_scrittura]=messaggio;
    indice_scrittura= (indice_scrittura+1)%DIM_BUFFER;
    contatore+=1;
    pthread_mutex_unlock(&semaforo_buffer);
    
    
}

Temp lettura_buffer(){
    
    Temp messaggio={-1,0,0,0};
    
    if( contatore>0){
	    pthread_mutex_lock(&semaforo_buffer);
	    messaggio=buffer[indice_lettura];
	    indice_lettura= (indice_lettura+1)%DIM_BUFFER;
	    contatore-=1;
	    pthread_mutex_unlock(&semaforo_buffer);
    }
    return messaggio;
    
}


Statistiche  Partita(WINDOW *finestra_gioco,Parametri parametri_gioco){			
    
    
   
    //Inizializziamo variabili di gestione della partita
    int tane_occupate=0;
    Statistiche  statistiche_gioco;
    statistiche_gioco.punteggio=0; 		    //tiene conto del punteggio di gioco
    statistiche_gioco.vite=parametri_gioco.vite;    //tiene conto del numero di vite rimaste
    statistiche_gioco.tempo=parametri_gioco.tempo;  //tiene conto del tempo rimasto nella manche
   
    for (int i=0;i<TANE-1;i++) {
        statistiche_gioco.tane[i]=0;  //impostiamo come 'aperte' tutte le tane
    }
    
    Flusso flussi[8];
    velocità_flussi(flussi,parametri_gioco.velocità_coccodrilli);  //definiamo la velocità di ogni flusso                
    int array_pid[NUMERO_PID];  //array contenente i pid dei processi creati (utilizzato per uccidere o mettere in pausa i processi) 
    
    Parametri_coccodrillo parametri_coccodrillo[NUMERO_COCCODRILLI];
    pthread_t rana,tempo;
    pthread_t thread_coccodrilli[NUMERO_COCCODRILLI];
    pthread_mutex_t semafori_coccodrilli[NUMERO_COCCODRILLI]; //creo un mutex per ogni coccodrillo per gestire le pause
    for (int i = 0; i < NUMERO_COCCODRILLI; i++) {
        if (pthread_mutex_init(&semafori_coccodrilli[i], NULL) != 0) {
            perror("pthread_mutex_init fallita");
            exit(1);
        }
    }
    while (true) {
        
       
        pthread_create(&rana, NULL, &funzione_rana, (void *)finestra_gioco);
        pthread_create(&tempo, NULL, &funzione_tempo, NULL);
        funzione_gestione_coccodrilli(flussi, array_pid, &parametri_gioco, thread_coccodrilli, parametri_coccodrillo, semafori_coccodrilli);
        
        direzione_flussi(flussi);  //definiamo la direzione di ogni flusso					
    	
	gestore_grafica(finestra_gioco,array_pid,parametri_gioco.velocità_proiettili,&statistiche_gioco, semafori_coccodrilli);  //funzione di gestione grafica (gestisce collisioni e la grafica)
	
	
    	statistiche_gioco.tempo=parametri_gioco.tempo;  //riportiamo a default il tempo di gioco 	

        
			
        
        //CONTROLLI DI FINE MANCHE
        
        if (statistiche_gioco.vite==0) { //controllo sul numero di vite
 	    statistiche_gioco.gioco=0;
 	    statistiche_gioco.punteggio-=50;
 	    return statistiche_gioco;  //uscita per vite finite
	}
	
   	for (int i=0;i<5;i++) {
 	    if (statistiche_gioco.tane[i]==1) {
  	        tane_occupate++;
  	    }
 	}
 	
 	if (tane_occupate==5) {	 //controllo sul numero di tane chiuse	
 	    statistiche_gioco.gioco=1;
 	    statistiche_gioco.punteggio+=100;
	    return statistiche_gioco;  //uscita per tane chiuse
 	}
	tane_occupate=0;
   
	wclear(finestra_gioco);
	wrefresh(finestra_gioco);     
	}
}




//funzione utile per creare un numero numero_random tra un minimo e un massimo (compresi)
int numero_random(int min, int max){

    return min + rand() % (max-min+1);

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
