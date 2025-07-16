#include<CrocHandle.h>


void controllo_sparo_proiettile(Thread_id thread_id[] ,Coccodrillo* coccodrilli, Rana* rana,  Proiettile* proiettili,struct timeval* inizio, struct timeval* fine, float* numero_randomico, int velocità_proiettili){
    int coccodrillo_scelto;
    int indice_proiettile=0;
    gettimeofday(fine,NULL);				
    if ((fine->tv_sec - inizio->tv_sec) + (fine->tv_usec - inizio->tv_usec)/1e6>=*numero_randomico) {  //controlliamo se il tempo trascorso supera la soglia per lo sparo del proiettile
	
	    
        do {
            coccodrillo_scelto = numero_random(0, NUMERO_COCCODRILLI - 1);
    
	} while (coccodrilli[coccodrillo_scelto].vivo != 1 || rana_su_coccodrillo(rana, coccodrilli)==coccodrillo_scelto);  //individuiamo un coccodrillo vivo da cui sparare	
	            
	while (thread_id[ID_PROIETTILE + indice_proiettile].valido==1) {  //individuiamo uno slot libero da cui creare il proiettile
 	    indice_proiettile++;
	}
	
	spara_proiettile(indice_proiettile,coccodrillo_scelto,coccodrilli,velocità_proiettili,thread_id); //spariamo il proiettile assocciandolo al coccodrillo scelto
	
	proiettili[indice_proiettile].vivo=1;  //impostiamo il proiettile sparato come vivo    
	gettimeofday(fine,NULL);		
	gettimeofday(inizio,NULL);
	*numero_randomico=numero_random_float(0.5,1.0);  //cambiamo l'intervallo di tempo per aggiungere casualità al gameplay;	
	
    }

}

//funzione che gestisce la creazione del processo proiettile
void spara_proiettile(int id,int identificatore_coccodrillo, Coccodrillo* coccodrilli,int velocità_proiettili,Thread_id thread_id[]){
   
    Parametri_proiettile* parametri_proiettile = malloc(sizeof(Parametri_proiettile));
    parametri_proiettile->id=id;
    parametri_proiettile->coccodrillo=coccodrilli[identificatore_coccodrillo];
    parametri_proiettile->velocità_proiettili=velocità_proiettili;
   
    pthread_create(&thread_id[ID_PROIETTILE+id].id, NULL, &funzione_proiettile, (void*)parametri_proiettile);
    pthread_detach(thread_id[ID_PROIETTILE+id].id);
    thread_id[ID_PROIETTILE+id].valido=1;
}


void collisione_proiettili_confine(Thread_id thread_id[] , Proiettile proiettili[]){

    //per ogni proiettile controlliamo se ha colpito il bordo della mappa
    for (int i=0; i<NUMERO_PROIETTILI; i++) {
	if ((proiettili[i].x<1 || proiettili[i].x>LARGHEZZA_GIOCO-2) && proiettili[i].vivo==1 && proiettili[i].x!=-2) {	
	    
	    pthread_cancel(thread_id[ID_PROIETTILE + i].id);
	    thread_id[ID_PROIETTILE + i].valido=0;
	    //lo impostiamo a morto e in posizione di default
	    proiettili[i].vivo=0;
	    proiettili[i].x=-2;
	    
	}
    }
}


//funzione che gestisce il proiettile
void* funzione_proiettile(void* parametri_thread){   
    
    Parametri_proiettile* param= (Parametri_proiettile*) parametri_thread;
    int velocità= param->velocità_proiettili;
    
    
    Temp proiettile={ID_PROIETTILE+param->id,0,0,0};
    
    //impostiamo il proiettile in base al coccodrillo che lo ha sparato
    if (param-> coccodrillo.dir==1) proiettile.x=param-> coccodrillo.x+5;
    else proiettile.x= param->coccodrillo.x-5;	
    proiettile.y=param->coccodrillo.y;	

    while (true) {

	    
        scrittura_buffer(proiettile);
        		
        proiettile.x+=param->coccodrillo.dir;
        usleep(velocità);
	
    }
}



void controllo_stato_coccodrillo(int id,Coccodrillo* coccodrilli){

    if ((coccodrilli[id].x>=SPAWN_DX_COCCODRILLO && coccodrilli[id].x==SPAWN_SX_COCCODRILLO && coccodrilli[id].dir==1) || (coccodrilli[id].x<=SPAWN_SX_COCCODRILLO&& coccodrilli[id].x==SPAWN_DX_COCCODRILLO && coccodrilli[id].dir==-1)) {  //verifichiamo se il coccodrillo è all'interno della mappa 
        coccodrilli[id].vivo=0;  //coccodrillo fuori dalla mappa = non attivo/morto
    } else {  
        coccodrilli[id].vivo=1;  //coccoddrillo dentro alla mappa = attivo/vivo
    }
}




//funzione di gestione dei coccodrilli
void* funzione_gestione_coccodrilli(Flusso *flussi,Parametri* parametri_gioco, Thread_id thread_id[], Parametri_coccodrillo parametri_coccodrillo[]){
    
    
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
	
	numero_coccodrilli_flussi[flusso_scelto]++;  //funzione_coccodrillo aumentiamo di uno il numero di coccodrilli nel flusso selezionato
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
        
        pthread_create(&thread_id[i].id, NULL, &funzione_coccodrillo, (void*) &parametri_coccodrillo[i]);
        pthread_detach(thread_id[i].id);
        thread_id[i].valido=1;
		
     
    } 
}



//funzione di controllo del coccodrillo
void* funzione_coccodrillo(void* parametri_thread){

    
    Parametri_coccodrillo* parametri_coccodrillo= (Parametri_coccodrillo *) parametri_thread;
       
    Temp coccodrillo= parametri_coccodrillo->coccodrillo;
    
    
  
    
    
    int id_flusso_scelto= parametri_coccodrillo->id_flusso;
    
    
    coccodrillo.info=parametri_coccodrillo->flussi[id_flusso_scelto].dir;
    
    
    while (true) {

        
	coccodrillo.x+=coccodrillo.info;
	
        sem_wait(&semafori_coccodrilli[coccodrillo.id]);
        scrittura_buffer(coccodrillo);
        sem_post(&semafori_coccodrilli[coccodrillo.id]);
        
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

void scrittura_buffer(Temp messaggio){
    
    
    
    sem_wait(&spazi_liberi);  // aspetta spazio libero
    pthread_mutex_lock(&semaforo_buffer);
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, &semaforo_buffer);

    buffer[indice_scrittura] = messaggio;
    indice_scrittura = (indice_scrittura + 1) % DIM_BUFFER;
    

    pthread_cleanup_pop(1); // sblocca il mutex qui
    sem_post(&spazi_occupati);
    
}

Temp lettura_buffer(){
    
    Temp messaggio={-1,0,0,0};
    
    if (sem_trywait(&spazi_occupati) == 0){
	    pthread_mutex_lock(&semaforo_buffer);
            pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, &semaforo_buffer);

            messaggio = buffer[indice_lettura];
            indice_lettura = (indice_lettura + 1) % DIM_BUFFER;
            

            pthread_cleanup_pop(1);  // sblocca il mutex qui
            sem_post(&spazi_liberi);
    }
    return messaggio;
    
}


