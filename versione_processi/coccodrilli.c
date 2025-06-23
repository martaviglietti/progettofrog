

const char *coccodrillo_sprite[2][2]={{"///////","XXXXXX0X0"},{"XXXXXXX","0X0XXXXXX"}};



//funzione che assegna una velocità a ogni flusso
void velocità_flussi(Flusso *flussi, int vel){
	
    int altezza_base=37;  //posizione del primo flusso (quello più in basso)
    int dimensione_flussi=3;  //larghezza dei flussi
    for (int i=0;i<8;i++) {
        flussi[i].y=altezza_base - i*dimensione_flussi;  //impostiamo anche l'altezza di ogni flusso 
	flussi[i].velocità= numero_random(vel - 20000,vel+20000);
    }
}




//funzione che assegna una direzione ad ogni flusso
void direzione_flussi(Flusso *flussi){
    for (int i=0;i<8;i++) {
	if (i>=1) {  //tutti i flussi dal secondo in poi hanno direzione opposta al flusso precedente
	    flussi[i].dir= -flussi[i-1].dir;

	} else {
	    //impostiamo la direzione del primo flusso in modo randomico
	    flussi[i].dir= rand()%2;
	    if (flussi[i].dir==0) flussi[i].dir=1;
	    else flussi[i].dir=-1;
	}
    }	
}

//funzione che gestisce il proiettile
void funzione_proiettile(int id, Coccodrillo coccodrillo,int vel,int pipe1[]){   
    close(pipe1[0]);
    Temp proiettile={ID_PROIETTILE+id,0,0,0};
    
    //impostiamo il proiettile in base al coccodrillo che lo ha sparato
    if (coccodrillo.dir==1) proiettile.x=coccodrillo.x+5;
    else proiettile.x=coccodrillo.x-5;	
    proiettile.y=coccodrillo.y;	

    while (true) {

	    
        write(pipe1[1], &proiettile, sizeof(Temp));  	
        		
        proiettile.x+=coccodrillo.dir;
        usleep(vel);
	
    }
}

//funzione di gestione dei coccodrilli
void funzione_gestione_coccodrilli(Flusso *flussi,int pipe1[],Parametri* parametri_gioco){
    close(pipe1[0]);
    
    int numero_coccodrilli_flussi[8]={0};  //variabile per tenere conto del numero di coccodrilli per ogni flusso
    int flusso_scelto;  //flusso scelto per lo spawn del coccodrillo
    Temp coccodrillo;
    Temp messaggio;   
    pid_t pid_coccodrillo;
    int numero_coccodrilli=0;  //numero di coccodrilli spawnati
    
    for(int i=0;i<NUMERO_COCCODRILLI;i++){
    
    	if ( numero_coccodrilli<10 ) {  //per i primi 10 coccodrilli lo spawn è completamente randomico
    	    do {
                flusso_scelto=numero_random(0,7);
            
            }while(numero_coccodrilli_flussi[flusso_scelto]==3);  //creiamo i coccodrilli in flussi casuali senza superare i 3 coccodrilli per flusso 
    	
    	} else {  //per i restanti 14 coccodrilli, prima dello spawn casuale controlliamo se ci sono flussi vuoti da riempire
    	    
    	    do {
                flusso_scelto=numero_random(0,7);
                for ( int j= 0; j<8; j++) {
                    if ( numero_coccodrilli_flussi[j]==0 ) {
                        flusso_scelto=j;
                        break;
            
                    }
                }
            } while (numero_coccodrilli_flussi[flusso_scelto]==3);  //creiamo i coccodrilli in flussi casuali senza superare i 3 coccodrilli per flusso 
    	}
        
       	
	//impostiamo il coccodrillo
	coccodrillo.id=i;
	coccodrillo.info=0;
	coccodrillo.y=flussi[flusso_scelto].y;  //il coccodrillo prende l'altezza del flusso in cui deve spawnare
	
	numero_coccodrilli_flussi[flusso_scelto]++;  //aumentiamo di uno il numero di coccodrilli nel flusso selezionato
	numero_coccodrilli++;  //aumentiamo i coccodrilli totali spawnati
	
	//impostiamo la posizione di spawn in base alla direzione del flusso
	if (flussi[flusso_scelto].dir==1) {
	    coccodrillo.x=SPAWN_SX_COCCODRILLO-1;   
	} else {
	    coccodrillo.x=SPAWN_DX_COCCODRILLO+1;
	}
	
	pid_coccodrillo=fork();
	if (pid_coccodrillo==-1) {
	    perror("Errore nella fork del coccodrillo: ");
	    exit(1);
	    
	} else if (pid_coccodrillo==0) {
		funzione_coccodrillo(coccodrillo,flussi,flusso_scelto,pipe1,parametri_gioco);
		
	} else {
	
	    //inviamo il messaggio al gestore grafico (contenente il pid) avvisandolo che è spawnato un nuovo coccodrillo    
	    messaggio.info=pid_coccodrillo;  
	    messaggio.id=i;
	    messaggio.y=ID_AGGIUNTAPID;  //serve per far capire che è un nuovo pid da aggiungere nella lista dei pid;      
	    write(pipe1[1], &messaggio,sizeof(Temp));       
	                   
	    usleep(numero_random((int)(500000*((float)parametri_gioco->velocità_coccodrilli/120000)),(int)(800000*((float)parametri_gioco->velocità_coccodrilli/120000))));  //pausa tra lo spawn di un coccodrillo e l'altro 
	}
	
	//pausa per migliorare lo spawn dei coccodrilli (una volta spawnati i primi 10 e riempiti gli eventuali flussi rimasti)
	if (i == NUMERO_COCCODRILLI/2) {
	    usleep(numero_random((int)(2000000*((float)parametri_gioco->velocità_coccodrilli/120000)),(int)3000000*((float)parametri_gioco->velocità_coccodrilli/120000)));  //a metà della creazione dei coc fermiamo la loro generazione per qualche secondo, così da aumentare la randomicità;
	}
    } 
}	
  
//funzione di controllo del coccodrillo
void funzione_coccodrillo(Temp coccodrillo,Flusso flussi[8],int id_flusso_scelto, int pipe1[],Parametri* parametri_gioco){
    
    
    coccodrillo.info=flussi[id_flusso_scelto].dir;
    
    
    while (true) {

	
	coccodrillo.x+=coccodrillo.info;
	write(pipe1[1], &coccodrillo, sizeof(Temp)); 
	usleep(flussi[id_flusso_scelto].velocità);
	
	
	//se il coccodrillo esce fuori dalla mappa allora deve prepararsi a ricominciare il suo tragitto dall'inizio del flusso
	if ((coccodrillo.x>=SPAWN_DX_COCCODRILLO && coccodrillo.info==1) || (coccodrillo.x<=SPAWN_SX_COCCODRILLO && coccodrillo.info==-1)) {  
	  
	
	    if (coccodrillo.info==1) {
	    
	        coccodrillo.x=SPAWN_SX_COCCODRILLO-1;
	        
	    } else {
	    
	        coccodrillo.x=SPAWN_DX_COCCODRILLO+1;
	    }
	    
	    usleep(numero_random((int)(300000*(1-parametri_gioco->livello_difficoltà*0.15)),(int)(3000000*(1-parametri_gioco->livello_difficoltà*0.15))));  //sleep basata sul livello di difficoltà scelto
	}
	    	
    }
}

//funzione che gestisce la creazione del processo proiettile
void spara_proiettile(int id,int identificatore_coccodrillo, Coccodrillo* coccodrilli,int velocità_proiettili, int pid_array[], int pipe1[]){
   
    pid_array[ID_PROIETTILE+id]= fork(); // Crea un nuovo processo per la granata
    
    
    if (pid_array[ID_PROIETTILE+id] < 0) {
        perror("Errore nella creazione del processo proiettile: ");
        exit(1);
    }

    if (pid_array[ID_PROIETTILE+id] == 0) {
        
        funzione_proiettile(id,coccodrilli[identificatore_coccodrillo],velocità_proiettili,pipe1);  //chiamiamo la funzione proiettile all'interno del processo figlio
        
    }
}
