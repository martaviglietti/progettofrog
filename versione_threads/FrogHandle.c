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
    frog->alive = 1;
    pthread_mutex_init(&frog->mutex, NULL);
    //printf("Rana inizializzata con alive=%d, x=%d, y=%d, tempo_prec=%f\n", frog->alive, frog->x, frog->y, frog->tempo_prec);

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
        pthread_mutex_unlock(&frog->mutex);


        if (key == -1) continue;

        int oldX = newX;
        int oldY = newY;
        switch (key) {
            case KEY_UP:
                newY  -=  ALTEZZARANA;
                break;

            case KEY_DOWN:
                newY += ALTEZZARANA;
                break;

            case KEY_LEFT:
                newX -= LARGHEZZARANA;
                break;

            case KEY_RIGHT:
                newX += LARGHEZZARANA;
                break;
        }    
        
        printf("frog moved from %d,%d to %d,%d\n", oldX, oldY, newX, newY);

        int* msgFrog = malloc(2 * sizeof(int));
        msgFrog[0] = newX;
        msgFrog[1] = newY;
        Message newMess;
        newMess.type = FROG_STATUS;
        newMess.data = msgFrog;

        push_event(&myBuffer, &newMess);

        usleep(30 * 1000);  // sleep 10 ms
    }
    pthread_exit(NULL);
}

void sparaGranata(const Frog* frog, const float time, const gameConfig* gameConfig) {

    bool allowed = true;

    for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){

        const Projectile* gran = (Projectile*)buffer.buffer[i];
        if(gran->alive) allowed = false;
    }

    if (allowed){
        GranateInit();
        pthread_create(&t_granate, NULL, thread_granata, (void *)gameConfig);
        for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){
            
            Projectile* gran = (Projectile*)buffer.buffer[i];
            
            gran->x = frog->x;
            gran->y = frog->y;
            gran->alive = 1;
            gran->tempo_prec = time;
            gran->dir = 2*(i%2) -1;
            gran->speed = gameConfig->velocità_proiettili;
        }
    }

    
}

//GESTIONE GRANATE-----------------------------------------
Projectile* GranateInit(){

    Projectile* granates = malloc(MAX_CROCODILES * sizeof(Projectile));
    if (granates == NULL) {
        fprintf(stderr, "malloc failed at Projectile Initialization\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 2; i++){

        Projectile* gran = (Projectile*)&granates[i];
        gran->x = -1;
        gran->y = -1;
        gran->alive = 0;
        gran->tempo_prec = -1;
        gran->dir = -1;
        gran->speed =-1;

        printf("Granata %d inizializzato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, tempo_prec=%f\n", i, gran->alive, gran->x, gran->y, gran->speed, gran->dir, gran->tempo_prec);
    }
    return granates;
}

void* thread_granata(void* arg) {

    while(1){

        LOCK_READ_GAME();
        const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            UNLOCK_GAME();
            break;
        }
        const float time = game_struct->time;
        UNLOCK_GAME();

        LOCK_FROG();
        for (int i = IDX_GRANATE; i < IDX_GRANATE + 2; i++){

            Projectile* gran = (Projectile*)buffer.buffer[i];

            if (!gran->alive || CollGranataProiettile(gran)){
                continue;
            }

            const int newX = gran->x + gran->dir * (int)(gran->speed * (gran->tempo_prec - time));
            if (newX == gran->x) continue;
            if (newX > 0 && newX < LARGHEZZA_GIOCO && !CollGranataProiettile(gran)){
                gran->x = newX;
                gran->tempo_prec = time;
            }
            else gran->alive=0;
        }
        UNLOCK_FROG();
        usleep(100 * 1000);  // sleep 10 ms
    }
    pthread_exit(NULL);
}

//___________________________________________________________________________________________________
//funzione che rinizializza la rana
void restartFrog(){

    Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
    frog->x = RANA_XINIT;
    frog->y = RANA_YINIT;
    frog->crocIdx = -1;
    frog->key = -1;
}
