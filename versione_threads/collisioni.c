#include "main.h"
#include "FrogHandle.h"
#include "CrocHandle.h"
#include<collisioni.h>


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
	 	
	  		