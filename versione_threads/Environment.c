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
 
void* thread_tempo(void* arg) {

    gameConfig* gameCfg = (gameConfig*)arg;

    struct timeval prev, now;
    gettimeofday(&prev, NULL);

    while (1) {
        usleep(100 * 1000);  // sleep 10 ms

        gettimeofday(&now, NULL);
        float elapsed = (now.tv_sec - prev.tv_sec) + (now.tv_usec - prev.tv_usec) / 1000000.0f;
        prev = now;

        LOCK_WRITE_GAME();
        Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            UNLOCK_GAME();
            break;
        }

        game_struct->time -= elapsed;
        printf("New time: %f\n",game_struct->time);
        
        if (game_struct->time <= 0){
            printf("NewManche: time out. \n");

            game_struct->score -= 20;
            game_struct->vite--;
            game_struct->time = gameCfg->tempo;

            UNLOCK_GAME();

            LOCK_FROG();
            restartFrog();
            UNLOCK_FROG();

            continue;
        }
        UNLOCK_GAME();
    }
    pthread_exit(NULL);
}
