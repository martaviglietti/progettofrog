#include "header.h"

void CrocodileInit(Flusso *flussi, Game_struct* game_struct) {
    const int Ninit = rand_funz(6, 12);

    for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++) {

        messaggio* this = &buffer.buffer[i];

        if ((i-IDX_COCCODRILLI) <= Ninit){

            const int id_flusso = rand_funz(0, NFLUSSI-1);
            const Flusso* flux = &flussi[id_flusso];
            
            this->alive=1;
            this->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
            this->y = flux->y;
            this->dir = flux->dir;
            this->speed = flux->speed;
            this->wait = -1;
            this->tempo_prec = game_struct->tempo;
            this->wait = rand_funz(2, 10);
        }
        else{
            this->alive = 0;
            this->wait = rand_funz(3, 15);
            this->x = -1;
            this->y = -1;
            this->dir = -1;
            this->speed = -1;
            this->tempo_prec = game_struct->tempo;
        }
    }
}

//questa funzione crea un messaggio m e lo invia nel buffer
void *thread_coccodrillo(void *arg) {

    ThreadArgs* args = (ThreadArgs*)arg;
    Game_struct* game_struct = args->Game_struct;
    gameConfig* gameConfig = args->gameConfig;

    if ((game_struct->win) ||  (game_struct->vite == 0)){
        pthread_exit(NULL);
    }

    pthread_mutex_lock(&buffer.mutex);

    for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++){
        messaggio* this = &buffer.buffer[i];

        if (this->alive){
            const int newX = this->x + this->dir * this->speed * (this->tempo_prec - game_struct->tempo);
            if (newX > POS_SPAWN_COC_SINISTRA && newX < POS_SPAWN_COC_DESTRA){    // nuova posizione valida
                this->x = newX;
                this->tempo_prec = game_struct->tempo;
                if (game_struct->tempo <= (this->tempo_prec - this->wait)){       //coccodrillo spara il proiettile
                    sparaProiettile(game_struct, gameConfig, i-IDX_COCCODRILLI);
                }
            }
            else {                                                         // coccodrillo fuori mappa, quindi muore
                this->alive = 0;
                this->wait = rand_funz(6, 15);
                this->x = -1;
                this->y = -1;
                this->dir = -1;
                this->speed = -1;
                this->tempo_prec = game_struct->tempo;
            }
        }
        else{
            if (game_struct->tempo <= (this->tempo_prec - this->wait)){      // creiamo il coccodrillo

                const int id_flusso = rand_funz(0, NFLUSSI-1);
                const Flusso* flux = &gameConfig->flussi[id_flusso];
                
                this->alive=1;
                this->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
                this->y = flux->y;
                this->dir = flux->dir;
                this->speed = flux->speed;
                this->wait = -1;
                this->tempo_prec = game_struct->tempo;
            }
            else continue;
        }
    }

    pthread_mutex_unlock(&buffer.mutex); 
}

//GESTIONE PROIETTILI------------------------------------------------------------------
void ProjectileInit(){
    for (int i = IDX_PROIETTILI; i< IDX_PROIETTILI + MAX_CROCODILES; i++){

        if (i >= BUFFER_SIZE){
            printf("you need a bigger buffer... we are accessing random areas of the memory!");
        }
        messaggio* this = &buffer.buffer[i];
        this->x = 50;
        this->y = -1;
        this->alive = 0;
    }
}

void* thread_proiettile(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    Game_struct* game_struct = args->Game_struct;

    if ((game_struct->win) ||  (game_struct->vite == 0)){
        pthread_exit(NULL);
    }

    pthread_mutex_lock(&buffer.mutex);

    for (int i = IDX_PROIETTILI; i < IDX_PROIETTILI + MAX_CROCODILES; i++){
        messaggio* this = &buffer.buffer[i];

        if (!this->alive) continue;

        int newX = this->x + this->dir * this->speed * (this->tempo_prec - game_struct->tempo);
        if (newX > 0 && newX < LARGHEZZA_GIOCO){    // nuova posizione valida
            this->x = newX;
            this->tempo_prec = game_struct->tempo;
        }
        else {                                                         // coccodrillo fuori mappa, quindi muore
            this->alive = 0;
        }
        
    }
    pthread_mutex_unlock(&buffer.mutex); 
}

void sparaProiettile(Game_struct* game_struct, gameConfig* gameConfig, int idx) {

    messaggio* this = &buffer.buffer[IDX_PROIETTILI + idx];
    const messaggio* cocco = &buffer.buffer[IDX_COCCODRILLI + idx];

    if (this->alive) return;
    
    this->alive = 1;
    this->y = cocco->y;
    this->tempo_prec = game_struct->tempo;
    this->dir = cocco->dir;
    this->x = (cocco->dir == 1) ? cocco->x + 5 : cocco->x - 5;
    this->speed = gameConfig->velocità_proiettili;
}

//---------------------------------------------------------------------------

