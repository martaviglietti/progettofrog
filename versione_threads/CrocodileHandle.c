#include "header.h"

void CrocodileInit(Flusso *flussi) {
    Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

    const int Ninit = rand_funz(2, 8);
    bool used_flusso[NFLUSSI] = { false };

    for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++) {

        buffer.buffer[i] = malloc(sizeof(Crocodile));
            if (buffer.buffer[i]== NULL) {
            fprintf(stderr, "malloc failed at Crocodile Initialization at index %d\n", i);
            exit(EXIT_FAILURE);
        }

        Crocodile* crocod = (Crocodile*)buffer.buffer[i];

        if ((i-IDX_COCCODRILLI) <= Ninit){

            int id_flusso;
            do {
                id_flusso = rand_funz(0, NFLUSSI - 1);
            } while (used_flusso[id_flusso]);  // retry if already used
            used_flusso[id_flusso] = true;
            const Flusso* flux = &flussi[id_flusso];
            
            crocod->alive=1;
            crocod->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
            crocod->y = flux->y;
            crocod->dir = flux->dir;
            crocod->speed = flux->speed;
            crocod->tempo_prec = game_struct->time;
            crocod->wait = game_struct->time - rand_funz(2, 10);
        }
        else{
            crocod->alive = 0;
            crocod->wait = game_struct->time - rand_funz(3, 10);
            crocod->x = -1;
            crocod->y = -1;
            crocod->dir = -1;
            crocod->speed = -1;
            crocod->tempo_prec = game_struct->time;
        }
        printf("Coccodrillo %d inizializzato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, wait=%d, tempo_prec=%f\n", i-IDX_COCCODRILLI, crocod->alive, crocod->x, crocod->y, crocod->speed, crocod->dir, crocod->wait, crocod->tempo_prec);
    }
}

//questa funzione crea un messaggio m e lo invia nel buffer
void *thread_coccodrillo(void *arg) {

    gameConfig* gameCfg = (gameConfig*)arg;
    Frog frogLocal;

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

                if (i==frogLocal.crocIdx && newX > RANA_XMIN && newX < RANA_XMAX){
                    LOCK_FROG();
                    Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
                    frog->x = newX;
                    UNLOCK_FROG();
                    printf("GestGraph: Rana é su coccodrillo %d, oldPos = %d, newPos = %d\n", i, frogLocal.x, newX);
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
        usleep(10 * 1000);  // sleep 10 ms
    }
    pthread_exit(NULL); 
}

//GESTIONE PROIETTILI------------------------------------------------------------------
void ProjectileInit(){
    for (int i = IDX_PROIETTILI; i< IDX_PROIETTILI + MAX_CROCODILES; i++){

        if (i >= BUFFER_SIZE){
            printf("you need a bigger buffer... we are accessing random areas of the memory!");
            exit(EXIT_FAILURE);
        }

        buffer.buffer[i] = malloc(sizeof(Projectile));
        if (buffer.buffer[i]== NULL) {
            fprintf(stderr, "malloc failed at Projectile Initialization at index %d\n", i);
            exit(EXIT_FAILURE);
        }
        Projectile* proj = (Projectile*)buffer.buffer[i];
        proj->x = -1;
        proj->y = -1;
        proj->alive = 0;
        proj->dir = -1;
        proj->speed =-1;
        proj->tempo_prec = -1;
        //printf("Proiettile %d inizializzato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, tempo_prec=%f\n", i, proj->alive, proj->x, proj->y, proj->speed, proj->dir, proj->tempo_prec);
    }
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
        usleep(10 * 1000);  // sleep 10 ms
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
}

//---------------------------------------------------------------------------

