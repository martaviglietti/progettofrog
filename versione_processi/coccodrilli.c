void controllo_stato_coccodrillo(int id,Coccodrillo* coccodrilli){

    if ((coccodrilli[id].x>=POS_SPAWN_COC_DESTRA && coccodrilli[id].dir==1) || (coccodrilli[id].x<=POS_SPAWN_COC_SINISTRA && coccodrilli[id].dir==-1)) {	//verifichiamo se il coccodrillo è all'interno della mappa
		    coccodrilli[id].alive=0;  //coccodrillo fuori dalla mappa = non attivo/morto
		} else {  
		    coccodrilli[id].alive=1;  //coccoddrillo dentro alla mappa = attivo/vivo
		}


}

void attesa_coccodrilli(int id, Coccodrillo* coccodrilli, int distanze_coc[], int array_pid[]){

    int indice_flusso;
    //gestione dello stato di attesa 
		if ((coccodrilli[id].x==POS_SPAWN_COC_DESTRA && coccodrilli[id].dir==-1) || (coccodrilli[id].x==POS_SPAWN_COC_SINISTRA && coccodrilli[id].dir==1)) {  //controlliamo che il coccodrillo corrente si trova a inizio tragitto			
		    for (int i=0;i<MAX_CROCODILES;i++) {
		        indice_flusso=(coccodrilli[i].y-37)/-3;  //individuiamo l'indice del flusso del coccoddrillo corrente
		
			//controlliamo se il coccodrillo attuale stia spawnando con coccodrilli troppo vicini nelle stesso flusso

			if (i!=coccodrilli[id].id && coccodrilli[i].y==coccodrilli[id].y && ((coccodrilli[id].x-distanze_coc[indice_flusso]<coccodrilli[i].x && coccodrilli[id].dir==-1) || (coccodrilli[id].x+distanze_coc[indice_flusso]>coccodrilli[i].x && coccodrilli[id].dir==1)) && coccodrilli[i].wait!=1){
			   coccodrilli[id].wait=1;    //lo mettiamo in wait perchè ci sono coccodrilli non in wait vicino a dove spawna

			   kill(array_pid[id],SIGSTOP);  //lo fermiamo finchè questa situazioni permane

			}
		    }
		}

}

void riattivare_coccodrilli(Coccodrillo* coccodrilli, int distanze_coc[], int array_pid[]){

    int indice_flusso;
    int riattivare;
    for (int i=0;i<MAX_CROCODILES;i++) {  //controlliamo se dobbiamo sbloccare qualche coccodrillo
		    if (coccodrilli[i].wait==1) {
			indice_flusso=(coccodrilli[i].y-37)/-3;
		        	
			riattivare=1;
			
			for (int j=0;j<MAX_CROCODILES;j++) {  //per ogni coccodrillo in wait controlliamo se ci sono ancora coccodrilli nelle vicinanze (quelli in wait non contano)
			    if(i!=j && coccodrilli[j].y==coccodrilli[i].y && ((coccodrilli[i].x-distanze_coc[indice_flusso]<coccodrilli[j].x && coccodrilli[i].dir==-1) || (coccodrilli[i].x+distanze_coc[indice_flusso]>coccodrilli[j].x && coccodrilli[i].dir==1)) && coccodrilli[j].wait!=1){
			        riattivare=0;
			        break; 
			    }		
			}
			if (riattivare) {	  	  
			    distanze_coc[indice_flusso]=rand_funz(13,16);    //modifichiamo la distanza da rispettare così da aggiungere diversità						
			    	
			    coccodrilli[i].wait=-1;    //togliamo la wait e lo facciamo ripartire;
			    kill(array_pid[i],SIGCONT);	
			}
		    }
		}




}

//funzione di controllo del coccodrillo
void funzione_coccodrillo(Temp coccodrillo,Flusso flussi[8],int id_flusso_scelto, int pipe1[]){
    
    usleep(rand_funz(100000,300000));  
    coccodrillo.info=flussi[id_flusso_scelto].dir;
    
    
    while (true) {

	
	coccodrillo.x=coccodrillo.x+flussi[id_flusso_scelto].dir;
	write(pipe1[1], &coccodrillo, sizeof(Temp)); 
	usleep(flussi[id_flusso_scelto].speed);
	
	
	//se il coccodrillo esce fuori dalla mappa allora deve prepararsi a ricomciare il suo tragitto dall'inizio del flusso
	if ((coccodrillo.x>=POS_SPAWN_COC_DESTRA && flussi[id_flusso_scelto].dir==1) || (coccodrillo.x<=POS_SPAWN_COC_SINISTRA && flussi[id_flusso_scelto].dir==-1)) { 
	
	    if (flussi[id_flusso_scelto].dir==1) {
	        coccodrillo.x=POS_SPAWN_COC_SINISTRA-1;
	    } else {
	        coccodrillo.x=POS_SPAWN_COC_DESTRA+1;
	    }
	    usleep(rand_funz(500000,2000000));
	}
	    	
    }
}

//funzione gestione proiettile
void funz_proiettile(int id, Coccodrillo coccodrillo,int vel,int pipe1[]){   
    close(pipe1[0]);
    Temp proiettile={IDPROIETTILE+id,0,0,0};
	
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

//funzione che assegna una direzione ad ogni flusso
void def_dir_flussi(Flusso *flussi){
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


//funzione di gestione dei coccodrilli
void funzione_gestione_coccodrilli(Flusso *flussi,int pipe1[]){
    close(pipe1[0]);
    
    int numero_coc_flussi[8]={0};
    int flusso_random;
    Temp coccodrillo;
    Temp messaggio;   
    pid_t pid_coc;
    
    for(int i=0;i<MAX_CROCODILES;i++){
    
        do {
            flusso_random=rand_funz(0,7);
           
        }while(numero_coc_flussi[flusso_random]==3);  //cerchiamo un flusso random che abbia meno di 3 coccodrilli;
   
   
       	
	//imposto il coccodrillo
	coccodrillo.id=i;
	coccodrillo.info=0;
	coccodrillo.y=flussi[flusso_random].y;
	
	numero_coc_flussi[flusso_random]++;  //aumentiamo di uno il numero di coccodrilli nel flusso selezionato
	
	if (flussi[flusso_random].dir==1) {
	    coccodrillo.x=POS_SPAWN_COC_SINISTRA-1;   
	} else {
	    coccodrillo.x=POS_SPAWN_COC_DESTRA+1;
	}
	
	pid_coc=fork();
	if (pid_coc==-1) {
	    perror("Errorre nella fork coccodrillo: ");
	    exit(1);
	} else if (pid_coc==0) {
		funzione_coccodrillo(coccodrillo,flussi,flusso_random,pipe1);
	} else {
	    
	    messaggio.info=pid_coc;  //impostiamo il messsaggio fa inviare
	    messaggio.id=i;
	    messaggio.y=IDAGGIUNTAPID;  //serve per far capire che è un nuovo pid da aggiungere nella lista dei pid;      
	
	    write(pipe1[1], &messaggio,sizeof(Temp));                            
	    usleep(rand_funz(500000,800000));}
	    if (i== MAX_CROCODILES/2) {
		usleep(rand_funz(2000000,3000000));  //a metà della creazione dei coc fermiamo la loro generazione per qualche secondo, così da aumentare la randomicità;
	    }
    } 
}

//funzione che gestisce la creazione del processo proiettile
void sparaProiettile(int id,int identificatore_coc, Coccodrillo* coccodrilli,int velocità_proiettile, int pid_array[], int pipe1[]){
    pid_array[IDPROIETTILE+id]= fork(); // Crea un nuovo processo per la granata

    if (pid_array[IDPROIETTILE+id] < 0) {
        perror("Errore nella creazione del processo proiettile");
        exit(-1);
    }

    if (pid_array[IDPROIETTILE+id] == 0) {
   
        funz_proiettile(id,coccodrilli[identificatore_coc],velocità_proiettile,pipe1);  //chiamiamo la funzione proiettile all'interno del processo figlio
       
    }
}
