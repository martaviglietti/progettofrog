#include "header.h"

//funzione che assegna una velocità a ogni flusso
void fluxInit(gameConfig *gameConfig){
	
    int altezza_base=37;  //posizione del primo flusso (quello più in basso)

    for (int i=0; i<NFLUSSI; i++) {

        Flusso* flux = &gameConfig->flussi[i];

        flux->y=altezza_base - i*DIM_FLUSSI;
	    flux->speed= rand_funz(gameConfig->velocità_coccodrilli - 2,gameConfig->velocità_coccodrilli+2);
    
        if (i>=1) {    //tutti i flussi dal secondo in poi hanno direzione opposta al flusso precedente
            flux->dir = -gameConfig->flussi[i-1].dir;
        } 
        else {         //impostiamo la direzione del primo flusso in modo randomico
            flux->dir= 2*rand()%2 -1;
        }
        //printf("Flusso %d inizializzato con y=%d, speed=%d, dir=%d\n", i, flux->y, flux->speed, flux->dir);
    }	
}