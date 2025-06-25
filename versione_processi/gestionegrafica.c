#include "header.h"


void gestore_grafica(WINDOW* finestra_gioco,int pipe1[], int pipe2[], int array_pid[],int velocità_proiettili, Statistiche * statistiche_gioco){			
	
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
    
    while (giocare) {
    	
	giocare=1;  //se durante il ciclo viene impostata a 0 allora si esce dal gioco
	temp.id=-1;  //valore di default 
	
	//funzioni di gestione grafica del gioco
	werase(finestra_gioco);
	generatore_finestra(finestra_gioco,COLS,LINES,statistiche_gioco);
        disegna_coccodrilli(finestra_gioco,coccodrilli);
        disegna_granate(finestra_gioco,granate);
        disegna_proiettile(finestra_gioco,proiettili);
        disegna_rana(finestra_gioco,&rana);
        barra_tempo(finestra_gioco,statistiche_gioco,tempo); //mostra la barra dinamica del tempo  
        wrefresh(finestra_gioco);
	
	
        read(pipe1[0],&temp,sizeof(Temp));  //leggiamo il messaggio nella pipe
	
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
	    if (temp.y!=ID_AGGIUNTAPID) {  //controlliamo se il messaggio proviene dal generatore dei coccodrilli
		coccodrillo_scelto=rana_su_coccodrillo(&rana,coccodrilli);  //controllo di verifica che la rana sia già su un coccodrillo (qualsiasi) prima dello spostamento
		coccodrilli[temp.id].x=temp.x;
		coccodrilli[temp.id].y=temp.y;
	        coccodrilli[temp.id].dir=temp.info;  
		
		//controlli per lo spostamento della rana
		giocare=movimento_rana_su_coccodrillo(temp.id, coccodrillo_scelto,coccodrilli, &rana,statistiche_gioco, giocare);	
		
		//controllo sullo stato del coccodrillo
		controllo_stato_coccodrillo(temp.id,coccodrilli);
			
			
		//gestione dello stato di attesa dei coccodrilli
		attesa_coccodrilli(temp.id, coccodrilli,distanze_coccodrilli,array_pid);
		riattivazione_coccodrilli(coccodrilli, distanze_coccodrilli, array_pid);
		
	    } else {  //altrimenti aggiungiamo il pid del coccodrillo all'array dei pid
	        array_pid[temp.id]=temp.info; //salva l id del coccodrillo creato;	
	    }
	}
	
	

        
	//se l'id è delle granate
	if (temp.id>=ID_GRANATE && temp.id<=ID_GRANATE+2) {
	    if (temp.id==ID_GRANATE+2) {  //id relativo alla richiesta di sparo delle granate
	        if (granate[0].vivo==0 && granate[1].vivo==0) {  //devono essere entrambe morte;		
	            spara_granata(rana.x,rana.y,velocità_proiettili,array_pid,pipe1,pipe2);
	           
	            granate[0].vivo=1;
                    granate[1].vivo=1;
          
                    
	            
	        }
	    } else {  
	        granate[temp.id-ID_GRANATE].x=temp.x;	
		granate[temp.id-ID_GRANATE].y=temp.y;
		
			   
	    }	  
	}
	

	
	//se l'id è di un proiettile
	if (temp.id>=ID_PROIETTILE && temp.id<ID_PROIETTILE+NUMERO_PROIETTILI) {	
	    proiettili[temp.id-ID_PROIETTILE].x=temp.x;
	    proiettili[temp.id-ID_PROIETTILE].y=temp.y;
	    
	}

        //controllo se sparare un nuovo proiettile
        controllo_sparo_proiettile(pipe1, array_pid,coccodrilli,proiettili, &inizio,&fine, &numero_randomico, velocità_proiettili);  	
	
	//controllo collisione proiettile-bordi
	collisione_proiettili_confine(array_pid, proiettili);
	
	//controllo collisione granate-bordi
	collisione_granate_confine(pipe2[1],granate);
	
	//controllo collisione rana-proiettile
	giocare=collisione_rana_proiettili(&rana, proiettili,statistiche_gioco, giocare);   
	
	//controllo collisione granata-proiettile
	collisione_granate_proiettili(pipe2[1],granate, proiettili, array_pid,statistiche_gioco);
	
	//controllo per killare il processo granate una volta morte
	uccidi_granate(granate, array_pid);

    }
}















