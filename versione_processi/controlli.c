#include "header.h"
void controllo_sparo_proiettile(int* pipe1, int* array_pid, Coccodrillo* coccodrilli, Proiettile* proiettili,struct timeval* inizio, struct timeval* fine, float* numero_randomico, int velocità_proiettili){
    int coccodrillo_scelto;
    int indice_proiettile=0;
    gettimeofday(fine,NULL);				
    if ((fine->tv_sec - inizio->tv_sec) + (fine->tv_usec - inizio->tv_usec)/1e6>=*numero_randomico) {  //controlliamo se il tempo trascorso supera la soglia per lo sparo del proiettile
	
	    
        do {
            coccodrillo_scelto = numero_random(0, NUMERO_COCCODRILLI - 1);
    
	} while (coccodrilli[coccodrillo_scelto].vivo != 1);  //individuiamo un coccodrillo vivo da cui sparare	
	            
	while (array_pid[ID_PROIETTILE+indice_proiettile]!=0) {  //individuiamo uno slot libero da cui creare il proiettile
 	    indice_proiettile++;
	}
	
	spara_proiettile(indice_proiettile,coccodrillo_scelto,coccodrilli,velocità_proiettili,array_pid,pipe1); //spariamo il proiettile assocciandolo al coccodrillo scelto
	proiettili[indice_proiettile].vivo=1;  //impostiamo il proiettile sparato come vivo    
					
	gettimeofday(fine,NULL);		
	gettimeofday(inizio,NULL);
	*numero_randomico=numero_random_float(0.5,1.0);  //cambiamo l'intervallo di tempo per aggiungere casualità al gameplay;	
	
    }

}

void controllo_stato_coccodrillo(int id,Coccodrillo* coccodrilli){

    if ((coccodrilli[id].x>=SPAWN_DX_COCCODRILLO && coccodrilli[id].x==SPAWN_SX_COCCODRILLO && coccodrilli[id].dir==1) || (coccodrilli[id].x<=SPAWN_SX_COCCODRILLO&& coccodrilli[id].x==SPAWN_DX_COCCODRILLO && coccodrilli[id].dir==-1)) {  //verifichiamo se il coccodrillo è all'interno della mappa 
        coccodrilli[id].vivo=0;  //coccodrillo fuori dalla mappa = non attivo/morto
    } else {  
        coccodrilli[id].vivo=1;  //coccoddrillo dentro alla mappa = attivo/vivo
    }
}
void collisione_granate_proiettili(int pipe2, Granata* granate, Proiettile proiettili[], int array_pid[], Statistiche * statistiche_gioco){
        
    //controllo collione tra una qualsiasi granata e un qualsiasi proiettile
    for (int i=0; i<2; i++) {
	for (int j=0; j<NUMERO_PROIETTILI;j++) {
	    if (granate[i].x==proiettili[j].x && granate[i].y==proiettili[j].y && proiettili[j].vivo==1 && granate[i].vivo==1) {
	        
	        kill_processo(array_pid[ID_PROIETTILE+j]);  //killiamo il processo proiettile
	        array_pid[ID_PROIETTILE+j]=0;  //liberiamo la zona relativa al proiettile ucciso
		    
	        //impostiamo a morti il proiettile e la granata mettendoli anche nella posizione di default 
	        proiettili[j].vivo=0;
	        proiettili[j].x=-2;
	        granate[i].vivo=0;
	        granate[i].x=-1;
	        write(pipe2,&i, sizeof(int));  //avvertiamo il processo granata che una delle granate è morta
	        statistiche_gioco->punteggio+=5;
		    
 	    }
	}
    }
}
void attesa_coccodrilli(int id, Coccodrillo* coccodrilli, int distanze_coccodrilli[], int array_pid[]){
    int indice_flusso;
    
    if ((coccodrilli[id].x==SPAWN_DX_COCCODRILLO && coccodrilli[id].dir==-1) || (coccodrilli[id].x==SPAWN_SX_COCCODRILLO && coccodrilli[id].dir==1)) {  //controlliamo che il coccodrillo corrente si trovi a inizio tragitto (quindi appena prima di entrare in mappa)		
        for (int i=0;i<NUMERO_COCCODRILLI;i++) {
            indice_flusso=(coccodrilli[i].y-37)/-3;  //individuiamo l'indice del flusso del coccoddrillo corrente
			
	    //controlliamo se il coccodrillo attuale stia spawnando con coccodrilli troppo vicini nelle stesso flusso (ovviamente esclusi quelli in attesa)
	    if (i!=coccodrilli[id].id && coccodrilli[i].y==coccodrilli[id].y && ((coccodrilli[id].x-distanze_coccodrilli[indice_flusso]<coccodrilli[i].x && coccodrilli[id].dir==-1) || (coccodrilli[id].x+distanze_coccodrilli[indice_flusso]>coccodrilli[i].x && coccodrilli[id].dir==1)) && coccodrilli[i].attesa!=1){
	        coccodrilli[id].attesa=1;  //lo mettiamo in attesa perchè ci sono coccodrilli non in attesa vicino a dove spawna
	        kill(array_pid[id],SIGSTOP);  //lo stoppiamo
	    }
        }
    }
}

void riattivazione_coccodrilli(Coccodrillo* coccodrilli, int distanze_coccodrilli[], int array_pid[]){

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
		kill(array_pid[i],SIGCONT);	
	    }
	}
    }
}

void collisione_proiettili_confine(int array_pid[], Proiettile proiettili[]){

    //per ogni proiettile controlliamo se ha colpito il bordo della mappa
    for (int i=0; i<NUMERO_PROIETTILI; i++) {
	if ((proiettili[i].x<1 || proiettili[i].x>LARGHEZZA_GIOCO-2) && proiettili[i].vivo==1 && proiettili[i].x!=-2) {	
	    kill_processo(array_pid[ID_PROIETTILE+i]);  //lo uccidiamo
	    array_pid[ID_PROIETTILE+i]=0;
	    //lo impostiamo a morto e in posizione di default
	    proiettili[i].vivo=0;
	    proiettili[i].x=-2;
	    
	}
    }
}

void collisione_granate_confine(int pipe2, Granata* granate){
    //per ogni granata controlliamo se ha colpito il bordo della mappa
    for (int i=0; i<2;i++) {
	if ((granate[i].x<1 || granate[i].x>LARGHEZZA_GIOCO-2) && granate[i].vivo==1 && granate[i].x!=-1) {
	    //la impostiamo a morta e in posizione i default
	    granate[i].vivo=0;
	    granate[i].x=-1;  
	    write(pipe2,&i, sizeof(int));  //scriviamo al processo granate quale delle due granate è morta    	
	}
    }
}



//funzione di controllo collisione rana-proiettile
int collisione_rana_proiettili(Rana*rana,Proiettile proiettili[], Statistiche * statistiche_gioco, int giocare){
    
    //per ogni proiettile controlla se collide con la rana
    for (int i=0;i<NUMERO_PROIETTILI;i++) {
 
        if ((rana->x+1==proiettili[i].x || rana->x-1==proiettili[i].x) && rana->y==proiettili[i].y && proiettili[i].vivo) {
	    statistiche_gioco->vite--;
	    statistiche_gioco->punteggio-=15;
	    return 0;
        }
    }
    
    return giocare;    
}

//funzione di controllo se la rana è su una tana
int rana_su_tana(Rana* rana, Statistiche * statistiche_gioco){
	  
    //verifica se la rana si trova su una tana aperta, altrimenti restituisce 0
    for (int i=0;i<5;i++) {
        if (rana->x>=8+(15)*i && rana->x<8+(15)*i+5 && statistiche_gioco->tane[i]==0) {
   	    statistiche_gioco->tane[i]=1;
 	    return 1;
 	}
    }
    return 0;
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

//funzione che gestisce il movimento della rana su un coccodrillo
int movimento_rana_su_coccodrillo(int id, int coccodrillo_scelto, Coccodrillo* coccodrilli, Rana* rana, Statistiche * statistiche_gioco, int giocare ) {

    if (coccodrillo_scelto==coccodrilli[id].id) {  //controlliamo che il coccodrillo attuale sia anche quello su cui la rana era posata prima dello spostamento
        if ((rana->x<=2 && coccodrilli[id].dir==-1) || (rana->x>=LARGHEZZA_GIOCO-3 && coccodrilli[id].dir==1)) {  //controlliamo se la rana si trova agli estremi della mappa			
	    if (rana_su_coccodrillo(rana,coccodrilli)!=coccodrillo_scelto) {  //se la rana non viene considerata più sopra l'attuale coccodrillo allora è caduta in acqua sul bordo della mappa 
	        statistiche_gioco->vite--;
		statistiche_gioco->punteggio-=10;	    
		return 0;
	    }	
	} else {  //altrimenti la rana si muove con il coccodrillo (ovvero non si trovava agli estremi della mappa)					
	rana->x+=coccodrilli[id].dir;
	}               			
    }
    return giocare;
}
//funzione che controlla se la rana si trova in mappa
int rana_in_finestra(Rana* rana, Temp* temp){

    if (rana->x+temp->x>78) return 0;
    if (rana->x+temp->x<2) return 0;
    if (rana->y+temp->y>43) return 0;
    if (rana->y+temp->y<7) return 0;
    return 1;
}


void uccidi_granate(Granata* granate, int array_pid[]){
    
    if (granate[0].vivo==0 && granate[1].vivo==0 && array_pid[ID_GRANATE]!=0) {  //se entrambe sono morte killiamo il loro processo 
               
	kill_processo(array_pid[ID_GRANATE]);
    	
	array_pid[ID_GRANATE]=0;  //liberiamo la zona dell'array relativa al processo granate 
    }
}
