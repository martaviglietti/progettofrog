#include "header.h"


void gestore_grafica(WINDOW* finestra_gioco, int array_pid[],int velocità_proiettili, Statistiche * statistiche_gioco, pthread_mutex_t semafori_coccodrilli[]){			
	
    //Variabili per gestione proiettile
    struct timeval inizio, fine; 
    gettimeofday(&fine,NULL);		
    gettimeofday(&inizio,NULL);
    float numero_randomico=5;
	 
    Temp temp={-1,0,0,0};  //utilizzato per leggere dalla pipe1
    
    //Inizializzazione degli elementi di gioco
    Coccodrillo coccodrilli[NUMERO_COCCODRILLI];
    for (int i=0; i<NUMERO_COCCODRILLI;i++) {
        coccodrilli[i].id=i;        //id coccodrillo
	coccodrilli[i].x=-1;	    //posizione x coccodrillo
	coccodrilli[i].y=-1;        //posizione y coccodrillo
	coccodrilli[i].dir=0;       //direzione coccodrillo
	coccodrilli[i].vivo=0;      //stato del coccoddrillo
	coccodrilli[i].attesa=-1;}  //variabile per stato di attesa

    Granata granate[2];
    for (int i=0; i<2;i++) {
	granate[i].id=i;
	granate[i].x=-1;
	granate[i].y=-1;
	granate[i].vivo=0;}
		
    Rana rana;	
    rana.id=ID_RANA;
    rana.x=40; 
    rana.y=43;

    Proiettile proiettili[15];
    for (int i=0;i<NUMERO_PROIETTILI;i++) {
        proiettili[i].id=ID_PROIETTILE+i;
        proiettili[i].x=-2;
        proiettili[i].y=-1;
        proiettili[i].vivo=0;
    }
    
   
    int tempo=statistiche_gioco->tempo;  
    int coccodrillo_scelto;  //variabile utilizzata per indicare su quale coccodrillo posa la rana
    int distanze_coccodrilli[8];  //array utilizzato per tenere conto della distanza da mantenere tra i coccodrilli per ogni flusso (evita che si sovrappongano quando spawnano)
    
    for (int i=0; i<8;i++) {
	distanze_coccodrilli[i]=numero_random(13,16);  //impostiamo le distanze iniziali da mantenere tra i coccodrilli, per ogni flusso del fiume
    }
   
    int giocare=1;  //variabile utilizzata per terminare la manche quando si sono verificate le giuste condizioni
    
    int key;
    while (giocare) {
    	
	giocare=1;  //se durante il ciclo viene impostata a 0 allora si esce dal gioco
	
	
	//funzioni di gestione grafica del gioco
	pthread_mutex_lock(&semaforo_disegno);
	werase(finestra_gioco);
	
	generatore_finestra(finestra_gioco,COLS,LINES,statistiche_gioco);
        disegna_coccodrilli(finestra_gioco,coccodrilli);
        disegna_granate(finestra_gioco,granate);
        disegna_proiettile(finestra_gioco,proiettili);
        disegna_rana(finestra_gioco,&rana);
        barra_tempo(finestra_gioco,statistiche_gioco,tempo); //mostra la barra dinamica del tempo 
     
        wrefresh(finestra_gioco);
        pthread_mutex_unlock(&semaforo_disegno);
        
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

        
	temp=lettura_buffer();
	
	
	
	//se l'ID è della rana						
	if (temp.id==ID_RANA) {
	    if (rana_in_finestra(&rana,&temp)) {  //controllo per evitare che la rana fuoriesca dalla finestra di gioco
	        rana.x+=temp.x;
	        rana.y+=temp.y;	
	
		if (rana.y<10) {  //controllo se la rana si trova nella zona delle tane			
		    if (rana_su_tana(&rana,statistiche_gioco)) {  //controllo se la rana si trova su una delle tane        
		        statistiche_gioco->punteggio+=15;
			statistiche_gioco->punteggio+=(int)(15*(float)statistiche_gioco->tempo/100);
			giocare=0;						
		    } else {  //altrimenti la rana si trova nella zona circostante alle tane ( e muore)
		        statistiche_gioco->vite--;
			statistiche_gioco->punteggio-=10; 
		        giocare=0;
		    }
		}
		
		if(rana.y>=15 && rana.y<40){
		 
		    if(rana_su_coccodrillo(&rana,coccodrilli)==-1){ 
			statistiche_gioco->vite--;
		        statistiche_gioco->punteggio-=10;
			return;
					
		    }
		}
	    }  
        }
	
	//se l'id è del processo tempo
	if (temp.id==ID_TIME) {	
	    statistiche_gioco->tempo-=1;
	    if (statistiche_gioco->tempo==0) {  //controllo per verificare se il tempo è terminato
	        statistiche_gioco->vite--;
		statistiche_gioco->punteggio-=20;
		giocare=0;
	    }
	}
	
	
	//se l'ID è di un coccodrillo
	if (temp.id<NUMERO_COCCODRILLI && temp.id>=0) {
	        
		coccodrillo_scelto=rana_su_coccodrillo(&rana,coccodrilli);  //controllo di verifica che la rana sia già su un coccodrillo (qualsiasi) prima dello spostamento
		coccodrilli[temp.id].x=temp.x;
		coccodrilli[temp.id].y=temp.y;
	        coccodrilli[temp.id].dir=temp.info;  

                //controlli per lo spostamento della rana
		giocare=movimento_rana_su_coccodrillo(temp.id, coccodrillo_scelto,coccodrilli, &rana,statistiche_gioco, giocare);
                //controllo sullo stato del coccodrillo
		controllo_stato_coccodrillo(temp.id,coccodrilli);
		
		
		//gestione dello stato di attesa dei coccodrilli
		attesa_coccodrilli(temp.id, coccodrilli,distanze_coccodrilli,array_pid,semafori_coccodrilli);
		riattivazione_coccodrilli(coccodrilli, distanze_coccodrilli, array_pid,semafori_coccodrilli);
		
	}

        
        usleep(500);
        
        
    }
}   
