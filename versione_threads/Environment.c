#include "header.h"

//funzione che assegna una velocità a ogni flusso
void fluxInit(gameConfig *gameConfig){
	
    int altezza_base=37;  //posizione del primo flusso (quello più in basso)

    for (int i=0; i<NFLUSSI; i++) {

        Flusso* obj = &gameConfig->flussi[i];

        obj->y=altezza_base - i*DIM_FLUSSI;
	    obj->speed= rand_funz(gameConfig->velocità_coccodrilli - 10000,gameConfig->velocità_coccodrilli+10000);
    
        if (i>=1) {  //tutti i flussi dal secondo in poi hanno direzione opposta al flusso precedente
            obj->dir = -gameConfig->flussi[i-1].dir;
        } else {
            //impostiamo la direzione del primo flusso in modo randomico
            obj->dir= 2*rand()%2 -1;
        }
    }	
}
 
void* thread_tempo(void* arg) {

    gameConfig* gameCfg = (gameConfig*)arg;

    while (1) {

        pthread_mutex_lock(&buffer.mutex);

        Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            pthread_mutex_unlock(&buffer.mutex);
            break;
        }

        float* obj = &game_struct->tempo;

        if (*obj <= 0){
            game_struct->score -= 20;
            game_struct->vite--;
            newManche(game_struct, gameCfg);
        }
        else{
            (*obj)--;
            
        }
        sleep(1);  // wait one second
        pthread_mutex_unlock(&buffer.mutex);
    }
    pthread_exit(NULL);
}
