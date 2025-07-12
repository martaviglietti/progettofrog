#include "header.h"

Crocodile* CrocodileInit(Flusso *flussi, float time) {

    Crocodile* crocodiles = malloc(MAX_CROCODILES * sizeof(Crocodile));
    if (crocodiles == NULL) {
        fprintf(stderr, "malloc failed at Crocodile Initialization\n");
        exit(EXIT_FAILURE);
    }

    const int Ninit = rand_funz(3, 8);
    pthread_t t_coccodrilli[Ninit];

    bool used_flusso[NFLUSSI] = { false };

    for (int i = 0; i < MAX_CROCODILES; i++) {

        Crocodile* croc =  &crocodiles[i];
        croc->tempo_prec = time;
        croc->idx = i;

        if (i < Ninit){

            int id_flusso;
            do {
                id_flusso = rand_funz(0, NFLUSSI - 1);
            } while (used_flusso[id_flusso]);  // retry if already used
            used_flusso[id_flusso] = true;
            const Flusso* flux = &flussi[id_flusso];
            
            croc->alive=1;
            croc->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
            croc->y = flux->y;
            croc->dir = flux->dir;
            croc->speed = flux->speed;
            croc->wait = time - rand_funz(2, 10);

            pthread_create(&t_coccodrilli[i], NULL, thread_coccodrillo, (void*)&crocodiles[i]);
            pthread_detach(t_coccodrilli[i]);
        }
        else{
            croc->alive = 0;
            croc->wait = time - rand_funz(3, 10);
            croc->x = -1;
            croc->y = -1;
            croc->dir = -1;
            croc->speed = -1;
        }
        printf("Coccodrillo %d inizializzato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, wait=%d, tempo_prec=%f\n", i, crocodiles[i]->alive, crocodiles[i]->x, crocodiles[i]->y, crocodiles[i]->speed, crocodiles[i]->dir, crocodiles[i]->wait, crocodiles[i]->tempo_prec);
    }
    return crocodiles;
}

//questa funzione crea un messaggio m e lo invia nel buffer
void *thread_coccodrillo(void *arg) {

    Crocodile* croc = (Crocodile*)arg;

    while(atomic_load(&croc->alive)){
        

        const float time = game_struct->time;
        UNLOCK_GAME();

        LOCK_FROG();
        frogLocal = *(Frog*)buffer.buffer[IDX_RANA];
        UNLOCK_FROG();

        LOCK_CROCS();
        for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++){
            Crocodile* crocod = (Crocodile*)buffer.buffer[i];

            if (crocod->alive){

                //aggiorniamo la posizione del coccodrillo
                const int newX = crocod->x + crocod->dir * (int)(crocod->speed * (crocod->tempo_prec - time));

                if (newX != crocod->x){       //se la nuova posizione é diversa dalla precedente
                    if (newX >= POS_SPAWN_COC_SINISTRA && newX <= POS_SPAWN_COC_DESTRA){    // nuova posizione valida
                        crocod->x = newX;
                        crocod->tempo_prec = time;
                        if (time <= crocod->wait){       //coccodrillo spara il proiettile
                            sparaProiettile(time, gameCfg, i-IDX_COCCODRILLI);
                        }
                    }
                    else {
                        if ((newX < POS_SPAWN_COC_SINISTRA  && crocod->dir == -1) || (newX > POS_SPAWN_COC_DESTRA  && crocod->dir == 1)){                                               // coccodrillo fuori mappa, quindi muore
                            crocod->alive = 0;
                            crocod->wait = time - rand_funz(3, 10);
                            crocod->x = -1;
                            crocod->y = -1;
                            crocod->dir = -1;
                            crocod->speed = -1;
                            crocod->tempo_prec = time;
                        }
                    }
                }

                
            }
            else{     //coccodrillo é disattivo
                if (time <= crocod->wait){      // creiamo il coccodrillo

                    const int id_flusso = rand_funz(0, NFLUSSI-1);
                    const Flusso* flux = &gameCfg->flussi[id_flusso];
                    
                    crocod->alive=1;
                    crocod->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
                    crocod->y = flux->y;
                    crocod->dir = flux->dir;
                    crocod->speed = flux->speed;
                    crocod->wait = time - rand_funz(3, 10);
                    crocod->tempo_prec = time;
                }
            }
            printf("Coccodrillo %d aggiornato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, wait=%d, tempo_prec=%f, at time=%f\n", i-IDX_COCCODRILLI, crocod->alive, crocod->x, crocod->y, crocod->speed, crocod->dir, crocod->wait, crocod->tempo_prec, time);
        }
        UNLOCK_CROCS();
        usleep(100 * 1000);  // sleep 10 ms
    }
    pthread_exit(NULL); 
}

//GESTIONE PROIETTILI------------------------------------------------------------------
Projectile* ProjectileInit(){

    Projectile* projectiles = malloc(MAX_CROCODILES * sizeof(Projectile));
    if (projectiles == NULL) {
        fprintf(stderr, "malloc failed at Projectile Initialization\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CROCODILES; i++){

        Projectile* proj = &projectiles[i];
        proj->x = -1;
        proj->y = -1;
        proj->alive = 0;
        proj->dir = -1;
        proj->speed =-1;
        proj->tempo_prec = -1;
        //printf("Proiettile %d inizializzato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, tempo_prec=%f\n", i, proj->alive, proj->x, proj->y, proj->speed, proj->dir, proj->tempo_prec);
    }

    return projectiles;
}

void* thread_proiettile(void* arg) {

    while(1){

        LOCK_READ_GAME();
        const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            UNLOCK_GAME();
            break;
        }
        const float time = game_struct->time;
        UNLOCK_GAME();

        LOCK_PROJ();
        for (int i = IDX_PROIETTILI; i < IDX_PROIETTILI + MAX_CROCODILES; i++){
            Projectile* proj = (Projectile*)buffer.buffer[i];

            if (!proj->alive){
                continue;
            }

            const int newX = proj->x + proj->dir * (int)(proj->speed * (proj->tempo_prec - time));
            if (newX != proj->x){
            
                if (newX > 0 && newX < LARGHEZZA_GIOCO){    // nuova posizione valida
                    proj->x = newX;
                    proj->tempo_prec = time;
                }
                else {                                                         // coccodrillo fuori mappa, quindi muore
                    if ((newX <= 0  && proj->dir == -1) || (newX >= LARGHEZZA_GIOCO  && proj->dir == 1)){
                        proj->alive = 0;
                    }
                } 
            }
            printf("Proiettile %d aggiornato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, tempo_prec=%f, at time=%f\n", i-IDX_PROIETTILI, proj->alive, proj->x, proj->y, proj->speed, proj->dir, proj->tempo_prec, time);         
        }
        UNLOCK_PROJ();
        usleep(100 * 1000);  // sleep 10 ms
    }
    pthread_exit(NULL);
}

void sparaProiettile(const float time, const gameConfig* gameConfig, const int idx) {

    const Crocodile* crocodile = (Crocodile*)buffer.buffer[IDX_COCCODRILLI + idx];

    LOCK_PROJ();
    Projectile* proj = (Projectile*)buffer.buffer[IDX_PROIETTILI + idx];
    
    if (proj->alive){
        UNLOCK_PROJ();
        return;
    }
    printf("Stiamo sparando un proiettile da coccodrillo %d\n", idx);
    proj->alive = 1;
    proj->y = crocodile->y;
    proj->tempo_prec = time;
    proj->dir = crocodile->dir;
    proj->x = (crocodile->dir == 1) ? crocodile->x + 5 : crocodile->x - 5;
    proj->speed = gameConfig->velocità_proiettili;

    UNLOCK_PROJ();

    pthread_create(&t_proiettili, NULL, thread_proiettile, (void *)gameConfig);
}

//---------------------------------------------------------------------------

