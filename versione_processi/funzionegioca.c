#include "header.h"

Game_struct startGame(WINDOW *game,Stat_game stat_game){			
    
    //Creazione pipe;
    int pipe1[2];  //per comunicare tra consumatore e produttori;
    int pipe2[2];  //per comunicare alla granata se è morta;
    pipe(pipe1);        
    pipe(pipe2);
    sblocca_fd(pipe1[0]);  //sblocchiamo la pipe1	
    sblocca_fd(pipe2[0]);  //sblocchiamo la pipe2
   
    //Inizializziamo variabili di gestione della partita
    int tane_occupate=0;
    Game_struct game_struct;
    game_struct.score=0; 		//contiene lo score di tutto il game
    game_struct.vite=stat_game.vite;    //contiene numero di vite rimaste
    game_struct.tempo=stat_game.tempo;  //contiene tempo rimasto nella manche
   
    for (int i=0;i<TANE-1;i++) {
        game_struct.tane[i]=0;
    }
    
    Flusso flussi[8];
    def_vel_flussi(flussi,stat_game.velocità_coccodrilli); //definisco velocità di ogni flusso                
    int array_pid[N_PID];  //array contenente i pid dei processi creati  
    
    int a;
    
    while (true) {
        
        def_dir_flussi(flussi);  //definisce direzione di ogni flusso					
    	creazione_processi(flussi,array_pid, pipe1,game);  //crea tutti i processi
	Gestione_grafica(game,pipe1,pipe2,array_pid,stat_game.velocità_proiettili,&game_struct);  //funzione di gestione grafica
    	game_struct.tempo=stat_game.tempo; 	
	svuota_pipe(pipe1[0]);  //svuotiamo la pipe per eliminare eventuali elementi rimasti all'interno
	
	if(read(pipe1[0],&a,sizeof(int))!= -1){
	    perror("roba nella pipe");
	}
	


   	kill_processi(array_pid);		
   	
        //Controlli di fine manche
        if (game_struct.vite==0) { 
 	    game_struct.game=0;
 	    game_struct.score-=50;
 	    return game_struct;  //uscita per vite finite
	}
	
   	for (int i=0;i<5;i++) {
 	    if(game_struct.tane[i]==1){
  	        tane_occupate++;
  	    }
 	}
 	
 	if (tane_occupate==5) {		
 	    game_struct.game=1;
 	    game_struct.score+=100;
	    return game_struct;  //uscita per tane chiuse
 	}
	tane_occupate=0;
   
	wclear(game);
	wrefresh(game);     
	}
}

