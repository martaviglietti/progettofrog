#include "header.h"

void CrocodileInit(Flusso *flussi) {
    const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];
    const int Ninit = rand_funz(6, 12);

    for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++) {

        buffer.buffer[i] = malloc(sizeof(Crocodile));
            if (buffer.buffer[i]== NULL) {
            fprintf(stderr, "malloc failed at Crocodile Initialization at index %d\n", i);
            exit(EXIT_FAILURE);
        }

        Crocodile* crocod = (Crocodile*)buffer.buffer[i];

        if ((i-IDX_COCCODRILLI) <= Ninit){

            const int id_flusso = rand_funz(0, NFLUSSI-1);
            const Flusso* flux = &flussi[id_flusso];
            
            crocod->alive=1;
            crocod->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
            crocod->y = flux->y;
            crocod->dir = flux->dir;
            crocod->speed = flux->speed;
            crocod->tempo_prec = game_struct->tempo;
            crocod->wait = rand_funz(2, 10);
        }
        else{
            crocod->alive = 0;
            crocod->wait = rand_funz(3, 15);
            crocod->x = -1;
            crocod->y = -1;
            crocod->dir = -1;
            crocod->speed = -1;
            crocod->tempo_prec = game_struct->tempo;
        }
    }
}

//questa funzione crea un messaggio m e lo invia nel buffer
void *thread_coccodrillo(void *arg) {

    gameConfig* gameCfg = (gameConfig*)arg;

    while(1){
        
        pthread_mutex_lock(&buffer.mutex);

        const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            pthread_mutex_unlock(&buffer.mutex);
            break;
        }

        for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++){
            Crocodile* crocod = (Crocodile*)buffer.buffer[i];

            if (crocod->alive){
                const int newX = crocod->x + crocod->dir * crocod->speed * (crocod->tempo_prec - game_struct->tempo);
                if (newX > POS_SPAWN_COC_SINISTRA && newX < POS_SPAWN_COC_DESTRA){    // nuova posizione valida
                    crocod->x = newX;
                    crocod->tempo_prec = game_struct->tempo;
                    if (game_struct->tempo <= (crocod->tempo_prec - crocod->wait)){       //coccodrillo spara il proiettile
                        sparaProiettile(game_struct->tempo, gameCfg, i-IDX_COCCODRILLI);
                    }
                }
                else {                                                         // coccodrillo fuori mappa, quindi muore
                    crocod->alive = 0;
                    crocod->wait = rand_funz(6, 15);
                    crocod->x = -1;
                    crocod->y = -1;
                    crocod->dir = -1;
                    crocod->speed = -1;
                    crocod->tempo_prec = game_struct->tempo;
                }
            }
            else{
                if (game_struct->tempo <= (crocod->tempo_prec - crocod->wait)){      // creiamo il coccodrillo

                    const int id_flusso = rand_funz(0, NFLUSSI-1);
                    const Flusso* flux = &gameCfg->flussi[id_flusso];
                    
                    crocod->alive=1;
                    crocod->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
                    crocod->y = flux->y;
                    crocod->dir = flux->dir;
                    crocod->speed = flux->speed;
                    crocod->wait = -1;
                    crocod->tempo_prec = game_struct->tempo;
                }
                else continue;
            }
        }

        pthread_mutex_unlock(&buffer.mutex);
    }
    pthread_exit(NULL); 
}

//GESTIONE PROIETTILI------------------------------------------------------------------
void ProjectileInit(){
    for (int i = IDX_PROIETTILI; i< IDX_PROIETTILI + MAX_CROCODILES; i++){

        if (i >= BUFFER_SIZE){
            printf("you need a bigger buffer... we are accessing random areas of the memory!");
        }

        buffer.buffer[i] = malloc(sizeof(Projectile));
        if (buffer.buffer[i]== NULL) {
            fprintf(stderr, "malloc failed at Projectile Initialization at index %d\n", i);
            exit(EXIT_FAILURE);
        }
        Projectile* proj = (Projectile*)buffer.buffer[i];
        proj->x = 50;
        proj->y = -1;
        proj->alive = 0;
        proj->dir = -1;
        proj->speed =-1;
        proj->tempo_prec = -1;
    }
}

void* thread_proiettile(void* arg) {

    while(1){
        pthread_mutex_lock(&buffer.mutex);

        const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            pthread_mutex_unlock(&buffer.mutex);
            break;
        }

        for (int i = IDX_PROIETTILI; i < IDX_PROIETTILI + MAX_CROCODILES; i++){
            Projectile* proj = (Projectile*)buffer.buffer[i];

            if (!proj->alive) continue;

            int newX = proj->x + proj->dir * proj->speed * (proj->tempo_prec - game_struct->tempo);
            if (newX > 0 && newX < LARGHEZZA_GIOCO){    // nuova posizione valida
                proj->x = newX;
                proj->tempo_prec = game_struct->tempo;
            }
            else {                                                         // coccodrillo fuori mappa, quindi muore
                proj->alive = 0;
            }
            
        }
        pthread_mutex_unlock(&buffer.mutex); 
    }
    pthread_exit(NULL);
}

void sparaProiettile(const float time, const gameConfig* gameConfig, const int idx) {

    Projectile* proj = (Projectile*)buffer.buffer[IDX_PROIETTILI + idx];
    const Crocodile* crocodile = (Crocodile*)buffer.buffer[IDX_COCCODRILLI + idx];

    if (proj->alive) return;
    
    proj->alive = 1;
    proj->y = crocodile->y;
    proj->tempo_prec = time;
    proj->dir = crocodile->dir;
    proj->x = (crocodile->dir == 1) ? crocodile->x + 5 : crocodile->x - 5;
    proj->speed = gameConfig->velocità_proiettili;
}

//---------------------------------------------------------------------------

