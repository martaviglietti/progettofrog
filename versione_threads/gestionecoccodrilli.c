#include "header.h"


const char *coccodrillo_sprite[2][2]={{"XXXXXXX","XXXXXX0X0"},{"XXXXXXX","0X0XXXXXX"}};

//funzione di gestione dei coccodrilli
void* funzione_gestione_coccodrilli(Flusso *flussi,int* array_pid,Parametri* parametri_gioco, pthread_t thread_coccodrilli[], Parametri_coccodrillo parametri_coccodrillo[], pthread_mutex_t semafori_coccodrilli[]){
    
    
    int numero_coccodrilli_flussi[8]={0};  //variabile per tenere conto del numero di coccodrilli per ogni flusso
    int flusso_scelto;  //flusso scelto per lo spawn del coccodrillo
    Temp coccodrillo; 
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
	
	    

	parametri_coccodrillo[i].coccodrillo=coccodrillo;
	parametri_coccodrillo[i].flussi=flussi;
	parametri_coccodrillo[i].id_flusso=flusso_scelto;
        parametri_coccodrillo[i].parametri_gioco=parametri_gioco;
        parametri_coccodrillo[i].semaforo_coccodrillo= &semafori_coccodrilli[i];
        pthread_create(&thread_coccodrilli[i], NULL, &funzione_coccodrillo, (void*) &parametri_coccodrillo[i]);
	
		
     
    } 
}



//funzione di controllo del coccodrillo
void* funzione_coccodrillo(void* parametri_thread){
    Parametri_coccodrillo* parametri_coccodrillo= (Parametri_coccodrillo *) parametri_thread;
    
    pthread_mutex_t* semaforo_coccodrillo= parametri_coccodrillo->semaforo_coccodrillo;
    
    Temp coccodrillo= parametri_coccodrillo->coccodrillo;
    
    usleep(numero_random((int)(500000*((float)parametri_coccodrillo->parametri_gioco->velocità_coccodrilli/120000)),(int)(800000*((float)parametri_coccodrillo->parametri_gioco->velocità_coccodrilli/120000)))*coccodrillo.id); 
    
  
    
    
    int id_flusso_scelto= parametri_coccodrillo->id_flusso;
    
    
    coccodrillo.info=parametri_coccodrillo->flussi[id_flusso_scelto].dir;
    
    
    while (true) {

        
	coccodrillo.x+=coccodrillo.info;
	pthread_mutex_lock(semaforo_coccodrillo);  //semafori utilizzati per bloccare il coccodrillo se esso è stato messo in pausa
	scrittura_buffer(coccodrillo); 
	pthread_mutex_unlock(semaforo_coccodrillo);
	usleep(parametri_coccodrillo->flussi[id_flusso_scelto].velocità);
	
	
	//se il coccodrillo esce fuori dalla mappa allora deve prepararsi a ricominciare il suo tragitto dall'inizio del flusso
	if ((coccodrillo.x>=SPAWN_DX_COCCODRILLO && coccodrillo.info==1) || (coccodrillo.x<=SPAWN_SX_COCCODRILLO && coccodrillo.info==-1)) {  
	  
	
	    if (coccodrillo.info==1) {
	    
	        coccodrillo.x=SPAWN_SX_COCCODRILLO-1;
	        
	    } else {
	    
	        coccodrillo.x=SPAWN_DX_COCCODRILLO+1;
	    }
	    
	    usleep(numero_random((int)(300000*(1-parametri_coccodrillo->parametri_gioco->livello_difficoltà*0.20)),(int)(3000000*(1-parametri_coccodrillo->parametri_gioco->livello_difficoltà*0.20))));  //sleep basata sul livello di difficoltà scelto
	}
	    	
    }
}

void controllo_stato_coccodrillo(int id,Coccodrillo* coccodrilli){

    if ((coccodrilli[id].x>=SPAWN_DX_COCCODRILLO && coccodrilli[id].x==SPAWN_SX_COCCODRILLO && coccodrilli[id].dir==1) || (coccodrilli[id].x<=SPAWN_SX_COCCODRILLO&& coccodrilli[id].x==SPAWN_DX_COCCODRILLO && coccodrilli[id].dir==-1)) {  //verifichiamo se il coccodrillo è all'interno della mappa 
        coccodrilli[id].vivo=0;  //coccodrillo fuori dalla mappa = non attivo/morto
    } else {  
        coccodrilli[id].vivo=1;  //coccoddrillo dentro alla mappa = attivo/vivo
    }
}

//funzione che verifica se la rana è su un coccodrillo
int rana_su_coccodrillo(Rana* rana, Coccodrillo *coccodrilli){
    for (int i = 0; i < NUMERO_COCCODRILLI; i++) {
        if (coccodrilli[i].vivo && rana->y == coccodrilli[i].y &&  rana->x-1>= coccodrilli[i].x-4 && rana->x+1 <= coccodrilli[i].x+4) {  //la rana si trova su un coccodrillo se la sua intera figura è al suo interno
       
            return coccodrilli[i].id; 
            
        }
    }
    return -1; 
}












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

void attesa_coccodrilli(int id, Coccodrillo* coccodrilli, int distanze_coccodrilli[], int array_pid[],pthread_mutex_t semafori_coccodrilli[] ){
    int indice_flusso;
    
    if ((coccodrilli[id].x==SPAWN_DX_COCCODRILLO && coccodrilli[id].dir==-1) || (coccodrilli[id].x==SPAWN_SX_COCCODRILLO && coccodrilli[id].dir==1)) {  //controlliamo che il coccodrillo corrente si trovi a inizio tragitto (quindi appena prima di entrare in mappa)		
        for (int i=0;i<NUMERO_COCCODRILLI;i++) {
            indice_flusso=(coccodrilli[i].y-37)/-3;  //individuiamo l'indice del flusso del coccoddrillo corrente
			
	    //controlliamo se il coccodrillo attuale stia spawnando con coccodrilli troppo vicini nelle stesso flusso (ovviamente esclusi quelli in attesa)
	    if (i!=coccodrilli[id].id && coccodrilli[i].y==coccodrilli[id].y && ((coccodrilli[id].x-distanze_coccodrilli[indice_flusso]<coccodrilli[i].x && coccodrilli[id].dir==-1) || (coccodrilli[id].x+distanze_coccodrilli[indice_flusso]>coccodrilli[i].x && coccodrilli[id].dir==1)) && coccodrilli[i].attesa!=1){
	        coccodrilli[id].attesa=1;  //lo mettiamo in attesa perchè ci sono coccodrilli non in attesa vicino a dove spawna
	        pthread_mutex_lock(&semafori_coccodrilli[id]);
	    }
        }
    }
}

void riattivazione_coccodrilli(Coccodrillo* coccodrilli, int distanze_coccodrilli[], int array_pid[],pthread_mutex_t semafori_coccodrilli[]){

    int indice_flusso;
    int riattivare;
    
    for (int i=0;i<NUMERO_COCCODRILLI;i++) {  //controlliamo se dobbiamo sbloccare qualche coccodrillo
        if (coccodrilli[i].attesa==1) {
	    indice_flusso=(coccodrilli[i].y-37)/-3;  //individuiamo l'indice del flusso del coccoddrillo corrente	
	    riattivare=1;
	    for (int j=0;j<NUMERO_COCCODRILLI;j++) {  //per ogni coccodrillo in attesa controlliamo se ci sono ancora coccodrilli nelle vicinanze (quelli in attesa non contano)
	        if(i!=j && coccodrilli[j].y==coccodrilli[i].y && ((coccodrilli[i].x-distanze_coccodrilli[indice_flusso]<coccodrilli[j].x && coccodrilli[i].dir==-1) || (coccodrilli[i].x+distanze_coccodrilli[indice_flusso]>coccodrilli[j].x && coccodrilli[i].dir==1)) && coccodrilli[j].attesa!=1){
		    riattivare=0;
		    break; 
		}		
	    }
	    if (riattivare) {  //se riattivare è rimasto uguale a 1 significa che non aveva coccodrilli nelle vicinanze	  	  
	        distanze_coccodrilli[indice_flusso]=numero_random(13,16);   //modifichiamo la distanza da rispettare così da aggiungere casualità							    	
		coccodrilli[i].attesa=-1;  //togliamo la attesa e lo facciamo ripartire;
		pthread_mutex_unlock(&semafori_coccodrilli[i]);	
	    }
	}
    }
}
