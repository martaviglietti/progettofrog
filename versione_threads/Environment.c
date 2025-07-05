#include "header.h"

//funzione che assegna una velocità a ogni flusso
void fluxInit(gameConfig *gameConfig){
	
    int altezza_base=37;  //posizione del primo flusso (quello più in basso)

    for (int i=0; i<NFLUSSI; i++) {

        Flusso* flux = &gameConfig->flussi[i];

        flux->y=altezza_base - i*DIM_FLUSSI;
	    flux->speed= rand_funz(gameConfig->velocità_coccodrilli - 10000,gameConfig->velocità_coccodrilli+10000);
    
        if (i>=1) {  //tutti i flussi dal secondo in poi hanno direzione opposta al flusso precedente
            flux->dir = -gameConfig->flussi[i-1].dir;
        } else {
            //impostiamo la direzione del primo flusso in modo randomico
            flux->dir= 2*rand()%2 -1;
        }
        //printf("Flusso %d inizializzato con y=%d, speed=%d, dir=%d\n", i, flux->y, flux->speed, flux->dir);
    }	
}
 
void* thread_tempo(void* arg) {

    gameConfig* gameCfg = (gameConfig*)arg;

    struct timeval prev, now;
    gettimeofday(&prev, NULL);

    while (1) {
        usleep(10 * 1000);  // sleep 10 ms

        gettimeofday(&now, NULL);
        float elapsed = (now.tv_sec - prev.tv_sec) + (now.tv_usec - prev.tv_usec) / 1000000.0f;
        prev = now;

        pthread_mutex_lock(&buffer.mutex);

        Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            pthread_mutex_unlock(&buffer.mutex);
            break;
        }

        game_struct->tempo -= elapsed;
        //printf("New time: %f\n",game_struct->tempo);

        if (game_struct->tempo <= 0){
            Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
            frog->alive = false;
            game_struct->score -= 20;
            game_struct->vite--;
            newManche(game_struct, gameCfg);
        }

        pthread_mutex_unlock(&buffer.mutex);
    }
    pthread_exit(NULL);
}
