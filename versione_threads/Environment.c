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

    Time* time = (Time*)arg;
    float newTime = time->time;

    struct timeval prev, now;
    gettimeofday(&prev, NULL);

    while (atomic_load(&time->alive)) {
        usleep(50 * 1000);  // sleep 10 ms

        gettimeofday(&now, NULL);
        float elapsed = (now.tv_sec - prev.tv_sec) + (now.tv_usec - prev.tv_usec) / 1000000.0f;
        prev = now;

        newTime += elapsed;
        printf("New time: %f\n",newTime);

        float* msgTime = malloc(sizeof(float));
        *msgTime = newTime;
        Message newMess;
        newMess.type = TIME_STATUS;
        newMess.data = msgTime;

        push_event(&myBuffer, &newMess);
    }
    pthread_exit(NULL);
}

Time* timeInit(){

    Time* time = malloc(sizeof(Time));
    time->time =  0;
    time->alive = 1;

    pthread_t t_tempo;
    pthread_create(&t_tempo, NULL, thread_tempo, (void *)time);
    pthread_detach(t_tempo);

    return time;
}
