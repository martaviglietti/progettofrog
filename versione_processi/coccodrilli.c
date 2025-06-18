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



