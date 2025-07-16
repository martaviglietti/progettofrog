
#include "main.h"






Statistiche  Partita(WINDOW *finestra_gioco,Parametri parametri_gioco){			
    
    
   
    //Inizializziamo variabili di gestione della partita
    Statistiche  statistiche_gioco;
    statistiche_gioco.punteggio=0; 		    //tiene conto del punteggio di gioco
    statistiche_gioco.vite=parametri_gioco.vite;    //tiene conto del numero di vite rimaste
    statistiche_gioco.tempo=parametri_gioco.tempo;  //tiene conto del tempo rimasto nella manche
   
    int tane_occupate=0;
    for (int i=0;i<TANE-1;i++) {
        statistiche_gioco.tane[i]=0;  //impostiamo come 'aperte' tutte le tane
    }
    
    Flusso flussi[8];
    velocità_flussi(flussi,parametri_gioco.velocità_coccodrilli);  //definiamo la velocità di ogni flusso 
                   
    Thread_id thread_id[NUMERO_TID];  //array contenente i pid dei processi creati (utilizzato per uccidere o mettere in pausa i processi) 
    for (int i=0; i<NUMERO_TID; i++){
        thread_id[i].valido=0;

    }
    
    Parametri_coccodrillo parametri_coccodrillo[NUMERO_COCCODRILLI];
   
    
    
    
    while (true) {
        
       //init------------------------------------------------
        pthread_mutex_init(&semaforo_buffer, NULL);
        pthread_mutex_init(&semaforo_disegno, NULL);
        for (int i = 0; i < NUMERO_COCCODRILLI; i++) {
            if (sem_init(&semafori_coccodrilli[i], 0, 1) != 0) {
            perror("sem_init fallita");
            exit(1);
        }
}
        sem_init(&sem_posti_occupati, 0, 0);        // inizialmente nessun elemento occupato
        sem_init(&sem_posti_liberi, 0, DIM_BUFFER); // inizialmente tutti gli spazi sono liberi
        
        //-------------------------------------------------------
        
        //creazione thread
        direzione_flussi(flussi);  //definiamo la direzione di ogni flusso
        
        
        pthread_create(&thread_id[ID_RANA].id, NULL, &funzione_rana, (void *)finestra_gioco);
        pthread_detach(thread_id[ID_RANA].id);
        thread_id[ID_RANA].valido=1;
        
        pthread_create(&thread_id[ID_TIME].id, NULL, &funzione_tempo, NULL);
        pthread_detach(thread_id[ID_TIME].id);
        thread_id[ID_TIME].valido=1;


        funzione_gestione_coccodrilli(flussi, &parametri_gioco, thread_id, parametri_coccodrillo);

        



        //-----------------------------
       			
    	
	gestore_grafica(finestra_gioco,parametri_gioco.velocità_proiettili,&statistiche_gioco, thread_id);  //funzione di gestione grafica (gestisce collisioni e la grafica)
	
	
	//cancel e destroy------------------
        cancel_thread(thread_id);
	usleep(100000);
	
	for (int i = 0; i < DIM_BUFFER; i++) {
        buffer[i].id = 0;
        buffer[i].x = 0;
        buffer[i].y = 0;
        buffer[i].info = 0;
        }
        indice_scrittura=0;
        indice_lettura=0;
	
	
	pthread_mutex_destroy(&semaforo_buffer);
	pthread_mutex_destroy(&semaforo_disegno);
	for (int i = 0; i < NUMERO_COCCODRILLI; i++) {
            if (sem_destroy(&semafori_coccodrilli[i]) != 0) {
            perror("sem_destroy fallita");
            // qui puoi decidere se continuare o uscire, in genere si continua
        }
}
        sem_destroy(&sem_posti_occupati);
        sem_destroy(&sem_posti_liberi);
	//-------------------------------------
	
	
    	statistiche_gioco.tempo=parametri_gioco.tempo;  //riportiamo a default il tempo di gioco 	

        
			
        
        //CONTROLLI DI FINE MANCHE
        
        if (statistiche_gioco.vite==0) { //controllo sul numero di vite
 	    statistiche_gioco.gioco=0;
 	    statistiche_gioco.punteggio-=50;
 	    return statistiche_gioco;  //uscita per vite finite
	}
	
   	for (int i=0;i<5;i++) {
 	    if (statistiche_gioco.tane[i]==1) {//controllo collisione rana-proiettile
	
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



void cancel_thread(Thread_id thread_id[]){
    for(int i=0; i< NUMERO_PID; i++){
        if(thread_id[i].valido){
            pthread_cancel(thread_id[i].id);
            thread_id[i].valido=0;    
        }

    }
}



//funzione che restituisce un numero numero_random tra due estremi (compresi), di tipo float
float numero_random_float(float minimo, float massimo) {
    return minimo + ((float)rand() / (float)RAND_MAX) * (massimo - minimo);
}

