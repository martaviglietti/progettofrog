#include "header.h"

//funzione che assegna una velocità a ogni flusso
void fluxInit(gameConfig *gameConfig){
	
    int altezza_base=37;  //posizione del primo flusso (quello più in basso)

    for (int i=0; i<NFLUSSI; i++) {

        Flusso* this = &gameConfig->flussi[i];

        this->y=altezza_base - i*DIM_FLUSSI;
	    this->speed= rand_funz(gameConfig->velocità_coccodrilli - 10000,gameConfig->velocità_coccodrilli+10000);
    
        if (i>=1) {  //tutti i flussi dal secondo in poi hanno direzione opposta al flusso precedente
            this->dir = -gameConfig->flussi[i-1].dir;
        } else {
            //impostiamo la direzione del primo flusso in modo randomico
            this->dir= 2*rand()%2 -1;
        }
    }	
}
 