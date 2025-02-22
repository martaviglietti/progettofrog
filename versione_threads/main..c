
//IN QUESTO FILE SONO PRESENTI IL MAIN
// LE FUNZIONI DI GESTIONE DELLE SCHERMATE DI FINE GIOCO
// LA FUNZIONE DI GESTIONE DELLA SCELTA DELLA DIFFICOLTA'
// LA FUNZIONE DI CREAZIONE DEI PROCESSI

#include "header.h"


//sprite rana
const char *frog_sprite[2] = {
    "@ @",
    " - "
};

const char *coc_sprite[2][2]={{"XXXXXXX","XXXXXX0X0"},{"XXXXXXX","0X0XXXXXX"}};
            


int main() {
    printf("\e[8;%d;%dt", 49, 80);   //imposta la dimensione del terminale
    fflush(stdout);                  //controlla che tutti i dati siano visulizzati su terminale
    sleep(1);                        //pausa l'esecuzione per 1 secondo
    
    // Inizializza NCURSES--------------------------------------------------------------------------------------------------------------------------------
    initscr();
    noecho(); //i tasti premuti non saranno visualizzati sullo schermo
    cbreak();
    curs_set(0); //nasconde il cursore nel terminale
    resizeterm(49, 80);  //avvisare ncurses del cambio di dimensioni
    creazione_colori();

    int height = LINES;
    int width = COLS;
    int menuchoice;
    int continue_playing=1;
    int difficoltà;
    int ricomincia=1;
    Stat_game stat_game={0,0,0,0};
    Game_struct risultato;
    WINDOW *game = newwin(height, width, 0, 0); //creo una nuova finestra 
    box(game, ACS_VLINE, ACS_HLINE); //disegna un borto attorno alla finestra di gioco
    wrefresh(game);
    bool first_match=true;


    //LOOP PRINCIPALE DEL GIOCO-----------------------------------------------------------------------------------------------------------------------------------
    while (true) { //questo ciclo continua finchè non si esce dal menu di gioco

        if (!first_match){      //solo se stiamo ricominciando, se non é il primo match
            wclear(game);
            mvwprintw(game, height / 2, width / 2 - 5, "Uscita...");
            wrefresh(game);
            sleep(1);
        }

        menuchoice=menu(game,"Menu Principale", OPZIONI, 3);
        
        if (menuchoice == 0) {  // "Inizia gioco"
            
            difficoltà=scegliDifficolta(game);
            
            switch(difficoltà){
                case 0: //principiante
                    stat_game.vite=10;
                    stat_game.tempo=30;
                    stat_game.velocità_proiettili=3000;
                    stat_game.velocità_coccodrilli=500000;
                    break;
                case 1: //intermedio
                    stat_game.vite=10;
                    stat_game.tempo=30;
                    stat_game.velocità_proiettili=3000;
                    stat_game.velocità_coccodrilli=50000;
                    break;
                case 2: //difficile
                    stat_game.vite=10;
                    stat_game.tempo=30;
                    stat_game.velocità_proiettili=3000;
                    stat_game.velocità_coccodrilli=50000;
                    break;
            }
            //funzione che esegue il gioco e ritorna lo stato finale del gioco in una struttura risultato
            risultato=startGame(game,stat_game);
            if(risultato.game==1){	//abbiamo vinto
                wclear(game);
                continue_playing=gameWin(game,risultato.score);
                if(continue_playing==1){		//ricomincia la stessa partita 
                    break;  
                }else{
                    continue; //se alla fine di una partita scelgo di non ricominciare mi riporta al manu tramita ricomincia=1
                }
            }
            else{ //caso di pardita
                wclear(game);
                continue_playing=gameOver(game,risultato.score);
                if(continue_playing==1){					
                    break;
                }else{            
                    continue;     
                }
            }
        }
        else if(menuchoice==1){ //crediti
            credits(game);
        } 
    }

    wclear(game);
    mvwprintw(game, height / 2, width / 2 - 5, "Uscita...");
    wrefresh(game);
    sleep(1);

    delwin(game);
    endwin();
    return 0;
}

//FUNZIONE DI VINCITA DELLA PARTITA---------------------------------------------------------------------------------------------------------------------------------------------------------------------
int gameWin(WINDOW *game, int score) {
    werase(game);
    mvwprintw(game, 10, 10, "HAI VINTO! Punteggio: %d. Vuoi giocare ancora? (s/n)", score);
    wrefresh(game);
   
    char decision = wgetch(game);
    while(decision!= 's' && decision!='n'){

    	 decision = wgetch(game);
    }
    if (decision == 'n') {
        return 0;
    } else {
        return 1;
    }
}

//FUNZIONE DI PERDITA DELLA PARTITA---------------------------------------------------------------------------------------------------------------------------------------------------------------------
int gameOver(WINDOW *game, int score) {
    werase(game);
    mvwprintw(game, 10, 10, "GAME OVER! Punteggio: %d. Vuoi tornare al menù? (s/n)", score);
    wrefresh(game);
    char decision = wgetch(game);
    while(decision!= 's' && decision!='n'){
    	 decision = wgetch(game);
    }
    if (decision == 'n') {
        return 0;
    } else {
        return 1;
    }
}



int scegliDifficolta(WINDOW *game) {
    const char *difficolta[] = {"Facile", "Media", "Difficile"};
    return menu(game, "Scegli la Difficoltà", difficolta, 3);
}



//FUNZIONE PER LA CREAZIONE DEI PROCESSI DEL GIOCO--------------------------------------------------------------------------------------------------------------------


void creazione_processi(Flusso *flussi, int array_pid[N_PID], int pipe1[], WINDOW* game){
	
    //resetto tutti i pid dell'array per ricominciare a creare i processi
	for(int i=0;i<N_PID;i++){
		array_pid[i]=0;}	
	
    //chiamo la fork per creare il nuovo processo della rana
    array_pid[IDRANA]=fork();
	if(array_pid[IDRANA]==-1){
		perror("Erorre nella fork della rana : ");
		exit(1);}
        else if(array_pid[IDRANA]==0){
            frog(game,pipe1); 
        }                       
	else {
        array_pid[IDTIME]=fork();
		if(array_pid[IDTIME]==-1){
			perror("Erorre nella fork della rana : ");
			exit(1);}
		else if(array_pid[IDTIME]==0){
			tempo(pipe1);       
        } 
	    else {
            array_pid[MAX_CROCODILES]=fork();
            if(array_pid[MAX_CROCODILES]==-1){
                perror("Errore nella fork del generatore coccodrilli: ");
                exit(-1);}
            else if(array_pid[MAX_CROCODILES]==0){
                funzione_gestione_coccodrilli(flussi,pipe1);
                exit(-1);}
            else return;
            }
        }
}
