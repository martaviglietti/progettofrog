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

    ThreadArgs* args = (ThreadArgs*)arg;
    Game_struct* game_struct = args->Game_struct;
    gameConfig* gameConfig = args->gameConfig;

    while (1) {

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            break;
        }

        pthread_mutex_lock(&buffer.mutex);

        float* obj = (float*)buffer.buffer[IDX_TEMPO];

        if (*obj <= 0){
            game_struct->vite--;
            game_struct->score -= 20;
            /// riniziaizza
        }

        *obj =

    }
    pthread_exit(NULL);



    pthread_mutex_lock(&buffer.mutex);
    float* obj = (float*)buffer.buffer[IDX_TEMPO];

    if ((game_struct->win) || (game_struct->vite == 0)) {
        pthread_mutex_unlock(&buffer.mutex);
        break;
    }

    if (*obj <= 0) {
        game_struct->vite--;
        game_struct->score -= 20;
        *obj = gameConfig->tempo; // reset timer
    } else {
        (*obj)--;
    }

    pthread_mutex_unlock(&buffer.mutex);
    sleep(1);  // wait one second
}
pthread_exit(NULL);













    int tempo_rimanente = tempo_totale;

    while (tempo_rimanente >= 0) {
        messaggio m;
        m.id = IDTIME;
        m.x = tempo_rimanente;  // usiamo `x` per comunicare il tempo rimasto
        m.y = 0;
        m.dir = 0;
        m.speed = 0;
        m.alive = true;  // non important
        produttore(m);  // usa il buffer già condiviso
        sleep(1);
        tempo_rimanente--;
    }

    pthread_mutex_unlock(&buffer.mutex); 
}
