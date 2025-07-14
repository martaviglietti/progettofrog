#include"header.h"
#include <ncurses.h>
#include <pthread.h>

//GESTIONE RANA-------------------------------------------------------------------------------------------------------------
Frog* frogInit(){

    Frog* frog = malloc(sizeof(Frog));
    if (frog == NULL) {
        fprintf(stderr, "malloc failed at Frog Initialization\n");
        exit(EXIT_FAILURE);
    }
    
    frog->x = RANA_XINIT;
    frog->y = RANA_YINIT;
    frog->crocIdx = -1;
    frog->key = -1;
    atomic_store(&frog->alive, true);
    pthread_mutex_init(&frog->mutex, NULL);
    //printf("Rana inizializzata con alive=%d, x=%d, y=%d, key=%d\n", frog->alive, frog->x, frog->y, frog->key);

    pthread_t t_rana;
    pthread_create(&t_rana, NULL, thread_rana, (void *)frog);
    pthread_detach(t_rana);

    return frog;
}

void* thread_rana(void* arg) {
    
    Frog* frog = (Frog*)arg;
    int newX = frog->x;
    int newY = frog->y;
    int key = -1;

    while(atomic_load(&frog->alive)){

        pthread_mutex_lock(&frog->mutex);
        key = frog->key;
        frog->key = -1;
        newX = frog->x;
        pthread_mutex_unlock(&frog->mutex);


        if (key == -1) continue;

        int oldX = newX;
        int oldY = newY;
        switch (key) {
            case KEY_UP:
                newY  -=  ALTEZZARANA+1;
                break;

            case KEY_DOWN:
                newY += ALTEZZARANA+1;
                break;

            case KEY_LEFT:
                newX -= LARGHEZZARANA-1;
                break;

            case KEY_RIGHT:
                newX += LARGHEZZARANA-1;
                break;
        }    
        
        //printf("frog moved from %d,%d to %d,%d\n", oldX, oldY, newX, newY);

        int* msgFrog = malloc(2 * sizeof(int));
        msgFrog[0] = newX;
        msgFrog[1] = newY;
        Message newMess;
        newMess.type = FROG_STATUS;
        newMess.data = msgFrog;

        push_event(&myBuffer, &newMess);

        usleep(20 * 1000);  // sleep 10 ms
    }
    pthread_exit(NULL);
}

//GESTIONE GRANATE-----------------------------------------
Projectile* GranateInit(const Frog* frog, const float time, const gameConfig* gameConfig){

    Projectile* granates = malloc(2 * sizeof(Projectile));
    if (granates == NULL) {
        fprintf(stderr, "malloc failed at Granate Initialization\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 2; i++){

        Projectile* gran = (Projectile*)&granates[i];
        gran->x = frog->x;
        gran->y = frog->y;
        atomic_store(&gran->alive, true);
        gettimeofday(&gran->prev, NULL);
        gran->dir = 2*(i%2) -1;
        gran->speed = gameConfig->velocità_proiettili;

        //printf("Granata %d inizializzato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, tempo_prec=%f\n", i, gran->alive, gran->x, gran->y, gran->speed, gran->dir, time);
    }
    pthread_t t_granate;
    pthread_create(&t_granate, NULL, thread_granata, (void *)granates);
    pthread_detach(t_granate);
    return granates;
}

void* thread_granata(void* arg) {

    Projectile* granates = (Projectile*)arg;
    Projectile localGran[2];

    for (int i = 0; i < 2; i++){
        localGran[i] = granates[i];
    }

    struct timeval now;

    while(localGran[0].alive || localGran[1].alive){  
        if (granates == NULL) break;

        for (int i = 0; i < 2; i++){

            if(!atomic_load(&granates[i].alive)){
                localGran[i].alive = false;
                continue;
            }

            gettimeofday(&now, NULL);
            const float dt = (now.tv_sec - localGran[i].prev.tv_sec) + (now.tv_usec - localGran[i].prev.tv_usec) / 1000000.0f;
            const int dx = localGran[i].dir * (int)(localGran[i].speed * dt);

            if (dx == 0) continue;
            
            //printf("granade %d moved from %d to %d\n", i, localGran[i].x, localGran[i].x + dx);

            localGran[i].x += dx;
            localGran[i].prev = now;  
        }
        int* msgGran = malloc(2 * sizeof(int));
        msgGran[0] = localGran[0].x;
        msgGran[1] = localGran[1].x;
        Message newMess;
        newMess.type = GRAN_STATUS;
        newMess.data = msgGran;

        push_event(&myBuffer, &newMess);
    }
    pthread_exit(NULL);
}
