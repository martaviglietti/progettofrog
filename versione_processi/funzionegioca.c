#include "header.h"

Game_struct startGame(WINDOW *game, Stat_game stat_game) {			
    
    //CREAZIONE PIPE-----------------------------------------------------------------------------------------------------
    
    int pipe1[2];	//quella per comunicare tra consumatore e produttore;
    int pipe2[2];	//quella per comunicare alla granata se è morta;
    pipe(pipe1);        
    pipe(pipe2);	



    //INIZIALIZZAZIONE STATO DEL GIOCO-------------------------------------------------------------------------------------------------
    int tane_occupate=0; //inizializzo le tane che dovranno essere occupate
    Game_struct game_struct; // creo una variabile di tipo struttura che tiene traccia dello stato del gioco 
    game_struct.score=0; //inizializzo il punteggio a 0 
    game_struct.vite=stat_game.vite; //inizializzo il numero di vite co le ipostazioni iniziali 
    game_struct.game;


    //Imposto ogni tana come non occupata--------------------------------------------------------------------------------------------
        for(int i=0;i<TANE -1;i++){
    	game_struct.tane[i]=0;
    }
    
    //creo un array di flussi di tipo FLUSSO----------------------------------------------------------------
    Flusso flussi[8];  
    def_vel_flussi(flussi,stat_game.velocità_coccodrilli); 

    //creo un array di pid per tenere traccia dei processi creati --------------------------------------------------------------------                           
    int array_pid[N_PID];  
    for(int i=0;i<N_PID;i++){	
    	array_pid[i]=0;
    }
    
    //LOOP PRINCIPALE DEL GIOCO-------------------------------------------------------------------------------------------
    while(true){
    	game_struct.tempo=stat_game.tempo; //resetto tempo manche;
    	def_dir_flussi(flussi);	//definisco le direzioni dei flussi
    	creazione_processi(flussi,array_pid, pipe1, game);//creo i processi necessari per la gestione del gioco	
    	Gestione_grafica(game,pipe1,pipe2,array_pid,stat_game.velocità_proiettili,&game_struct);
		//aggiorno graficamente  il gico basandomi sullo stato corrente
    	kill_processi(array_pid);		//killiamo tutto per ricominciare dopo ogni ciclo
   	
        //le modifiche al numero di vite e allo score sono fatte all interno della render;
        if(game_struct.vite==0){ //sono finite le vite
 	    	game_struct.game=0;
 	    	return game_struct; 
    	}

        //le modifiche alle tane occupate sono fatte nella render
   	    for(int i=0;i<5;i++){
 	        if(game_struct.tane[i]==1){
  	        	tane_occupate++;
  	        }
        }
        if(tane_occupate==5){		//tutte le tane occupate;
            game_struct.game==1;
            return game_struct;
        }
        tane_occupate=0;   //riazzero per il prossimo conteggio;
        
        wclear(game);
        wrefresh(game);     
    }
 }

