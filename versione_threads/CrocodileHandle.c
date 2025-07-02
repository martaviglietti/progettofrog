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

        Crocodile* obj = (Crocodile*)buffer.buffer[i];

        if ((i-IDX_COCCODRILLI) <= Ninit){

            const int id_flusso = rand_funz(0, NFLUSSI-1);
            const Flusso* flux = &flussi[id_flusso];
            
            obj->alive=1;
            obj->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
            obj->y = flux->y;
            obj->dir = flux->dir;
            obj->speed = flux->speed;
            obj->tempo_prec = game_struct->tempo;
            obj->wait = rand_funz(2, 10);
        }
        else{
            obj->alive = 0;
            obj->wait = rand_funz(3, 15);
            obj->x = -1;
            obj->y = -1;
            obj->dir = -1;
            obj->speed = -1;
            obj->tempo_prec = game_struct->tempo;
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
            break;
        }

        for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++){
            Crocodile* obj = (Crocodile*)buffer.buffer[i];

            if (obj->alive){
                const int newX = obj->x + obj->dir * obj->speed * (obj->tempo_prec - game_struct->tempo);
                if (newX > POS_SPAWN_COC_SINISTRA && newX < POS_SPAWN_COC_DESTRA){    // nuova posizione valida
                    obj->x = newX;
                    obj->tempo_prec = game_struct->tempo;
                    if (game_struct->tempo <= (obj->tempo_prec - obj->wait)){       //coccodrillo spara il proiettile
                        sparaProiettile(game_struct->tempo, gameCfg, i-IDX_COCCODRILLI);
                    }
                }
                else {                                                         // coccodrillo fuori mappa, quindi muore
                    obj->alive = 0;
                    obj->wait = rand_funz(6, 15);
                    obj->x = -1;
                    obj->y = -1;
                    obj->dir = -1;
                    obj->speed = -1;
                    obj->tempo_prec = game_struct->tempo;
                }
            }
            else{
                if (game_struct->tempo <= (obj->tempo_prec - obj->wait)){      // creiamo il coccodrillo

                    const int id_flusso = rand_funz(0, NFLUSSI-1);
                    const Flusso* flux = &gameCfg->flussi[id_flusso];
                    
                    obj->alive=1;
                    obj->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
                    obj->y = flux->y;
                    obj->dir = flux->dir;
                    obj->speed = flux->speed;
                    obj->wait = -1;
                    obj->tempo_prec = game_struct->tempo;
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
        Projectile* obj = (Projectile*)buffer.buffer[i];
        obj->x = 50;
        obj->y = -1;
        obj->alive = 0;
        obj->dir = -1;
        obj->speed =-1;
        obj->tempo_prec = -1;
    }
}

void* thread_proiettile(void* arg) {

    while(1){
        pthread_mutex_lock(&buffer.mutex);

        const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            break;
        }

        for (int i = IDX_PROIETTILI; i < IDX_PROIETTILI + MAX_CROCODILES; i++){
            Projectile* obj = (Projectile*)buffer.buffer[i];

            if (!obj->alive) continue;

            int newX = obj->x + obj->dir * obj->speed * (obj->tempo_prec - game_struct->tempo);
            if (newX > 0 && newX < LARGHEZZA_GIOCO){    // nuova posizione valida
                obj->x = newX;
                obj->tempo_prec = game_struct->tempo;
            }
            else {                                                         // coccodrillo fuori mappa, quindi muore
                obj->alive = 0;
            }
            
        }
        pthread_mutex_unlock(&buffer.mutex); 
    }
    pthread_exit(NULL);
}

void sparaProiettile(const float time, const gameConfig* gameConfig, const int idx) {

    Projectile* obj = (Projectile*)buffer.buffer[IDX_PROIETTILI + idx];
    const Crocodile* crocodile = (Crocodile*)buffer.buffer[IDX_COCCODRILLI + idx];

    if (obj->alive) return;
    
    obj->alive = 1;
    obj->y = crocodile->y;
    obj->tempo_prec = time;
    obj->dir = crocodile->dir;
    obj->x = (crocodile->dir == 1) ? crocodile->x + 5 : crocodile->x - 5;
    obj->speed = gameConfig->velocità_proiettili;
}

//---------------------------------------------------------------------------

