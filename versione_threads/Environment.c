#include "header.h"

//funzione che assegna una velocità a ogni flusso
void def_vel_flussi(gameConfig gameConfig){
	
    int altezza_base=37;  //posizione del primo flusso (quello più in basso)
    int dimensione_flussi=3;  //larghezza dei flussi
    for (int i=0;i<NFLUSSI;i++) {
        gameConfig.flussi[i].y=altezza_base - i*dimensione_flussi;
	    gameConfig.flussi[i].speed= rand_funz(gameConfig.velocità_coccodrilli - 10000,gameConfig.velocità_coccodrilli+10000);
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
 