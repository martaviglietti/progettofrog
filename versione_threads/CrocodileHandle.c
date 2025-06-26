#include "header.h"
#include <cstdlib>

void CrocodileInit(Flusso *flussi) {
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
        }
        else{
            this->alive = 0;
            this->wait = rand_funz(3, 15);

            this->x = -1;
            this->y = -1;
            this->dir = -1;
            this->speed = -1;
        }
    }
}





//questa funzione crea un messaggio m e lo invia nel buffer

void *produttore_coccodrillo(void *arg) {
    messaggio *cocco = (messaggio *)arg;
    Flusso* flussi = NULL;
    Flusso flusso_scelto = flussi[cocco->info]; // Assicurati che l'id del flusso sia stato settato prima

    while (1) {
        cocco->x += flusso_scelto.dir;

        produttore(*cocco);  // Usa la funzione generica

        usleep(flusso_scelto.speed);

        // Se esce fuori dallo schermo, resettalo all’inizio
        if ((cocco->x >= POS_SPAWN_COC_DESTRA && flusso_scelto.dir == 1) ||
            (cocco->x <= POS_SPAWN_COC_SINISTRA && flusso_scelto.dir == -1)) {
            
            cocco->x = (flusso_scelto.dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
            usleep(rand_funz(500000, 2000000));
        }
    }

    return NULL;
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
    messaggio m = *(messaggio*)arg;
    free(arg);  // liberi subito la memoria

    while (m.x > 0 && m.x < LARGHEZZA_GIOCO - 1) {
        produttore(m);  // invia il messaggio nel buffer condiviso
        m.x += m.info;  // direzione: +1 o -1
        usleep(m.speed);
    }

    pthread_exit(NULL);
}

void sparaProiettile_daCoccodrillo(Coccodrillo coccodrillo, int vel_proiettile) {
    messaggio* arg = malloc(sizeof(messaggio));
    if (!arg) {
        perror("malloc fallita");
        return;
    }

    arg->id = IDPROIETTILE;
    arg->x = (coccodrillo.dir == 1) ? coccodrillo.x + 5 : coccodrillo.x - 5;
    arg->y = coccodrillo.y;
    arg->info = coccodrillo.dir;
    arg->speed = vel_proiettile;

    pthread_t t;
    if (pthread_create(&t, NULL, thread_proiettile, (void*)arg) != 0) {
        perror("errore creazione thread proiettile");
        free(arg);
    } else {
        pthread_detach(t);  // non serve fare pthread_join
    }
}

//---------------------------------------------------------------------------

