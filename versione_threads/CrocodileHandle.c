#include "header.h"

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




void crea_thread_coccodrilli(Flusso flussi[], Game_struct *game_struct) {
    for (int i = 0; i < MAX_CROCODILES; i++) {
        int id_flusso = rand_funz(0, 7);
        Flusso f = flussi[id_flusso];

        game_struct->Coccodrillo[i].id = i;
        game_struct->Coccodrillo[i].x = (f.dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
        game_struct->Coccodrillo[i].y = f.y;
        game_struct->Coccodrillo[i].info = f.dir;
        game_struct->Coccodrillo[i].speed = f.speed;

        pthread_create(&game_struct->coccodrilli[i].tid, NULL, produttore_coccodrillo, &game_struct->coccodrilli_args[i]);
    }
}



//GESTIONE PROIETTILI------------------------------------------------------------------

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

