#include "header.h"

Crocodile* CrocodileInit(Flusso *flussi, const float time) {

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
        gettimeofday(&croc->prev, NULL);
        croc->idx = i;

        if (i < Ninit){

            int id_flusso;
            do {
                id_flusso = rand_funz(0, NFLUSSI - 1);
            } while (used_flusso[id_flusso]);  // retry if already used
            used_flusso[id_flusso] = true;
            const Flusso* flux = &flussi[id_flusso];
            
            atomic_store(&croc->alive, true);
            croc->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
            croc->y = flux->y;
            croc->dir = flux->dir;
            croc->speed = flux->speed;
            croc->wait = time + rand_funz(2, 10);

            pthread_create(&t_coccodrilli[i], NULL, thread_coccodrillo, (void*)&crocodiles[i]);
            pthread_detach(t_coccodrilli[i]);
        }
        else{
            atomic_store(&croc->alive, false);
            croc->wait = time + rand_funz(3, 10);
            croc->x = -1;
            croc->y = -1;
            croc->dir = -1;
            croc->speed = -1;
        }
        printf("Coccodrillo %d inizializzato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, wait=%d, tempo_prec=%f\n", i, atomic_load(&croc->alive), croc->x, croc->y, croc->speed, croc->dir, croc->wait,time);
    }
    return crocodiles;
}

//questa funzione crea un messaggio m e lo invia nel buffer
void *thread_coccodrillo(void *arg) {

    Crocodile* croc = (Crocodile*)arg;
    Crocodile localCroc = *croc;

    struct timeval now;

    while(atomic_load(&croc->alive)){
        usleep(30 * 1000);  // sleep 100 ms

        gettimeofday(&now, NULL);

        const float dt = (now.tv_sec - localCroc.prev.tv_sec) + (now.tv_usec - localCroc.prev.tv_usec) / 1000000.0f;
        const int dx = localCroc.dir * (int)(localCroc.speed * dt);

        if (dx == 0) continue;

        printf("Crocodile %d moved from %d to %d\n", localCroc.idx, localCroc.x, localCroc.x + dx);
        localCroc.x += dx;
        localCroc.prev = now;
        
            
        int* msgCroc = malloc(2 * sizeof(int));
        msgCroc[0] = localCroc.idx;
        msgCroc[1] = localCroc.x;
        Message newMess;
        newMess.type = CROC_STATUS;
        newMess.data = msgCroc;

        push_event(&myBuffer, &newMess);
    }
    pthread_exit(NULL); 
}

//GESTIONE PROIETTILI------------------------------------------------------------------
Projectile* ProjectileInit(const Crocodile* croc, const float time, const gameConfig* gameCfg){

    Projectile* projectiles = malloc(MAX_CROCODILES * sizeof(Projectile));
    if (projectiles == NULL) {
        fprintf(stderr, "malloc failed at Projectile Initialization\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CROCODILES; i++){

        Projectile* proj = &projectiles[i];
        
        if(i == croc->idx){
            proj->x = (croc->dir == 1) ? croc->x + 5 : croc->x - 5;;
            proj->y = croc->y;
            atomic_store(&proj->alive, true);
            proj->dir = croc->dir;
            gettimeofday(&proj->prev, NULL);
            proj->speed = gameCfg->velocità_proiettili;
        }
        else{
            proj->x = -1;
            proj->y = -1;
            atomic_store(&proj->alive, false);
            proj->dir = -1;
            proj->speed =-1;
            gettimeofday(&proj->prev, NULL);
        }
        
        printf("Proiettile %d inizializzato con alive=%d, x=%d, y=%d, speed=%d, dir=%d, tempo_prec=%f\n", i, proj->alive, proj->x, proj->y, proj->speed, proj->dir, time);
    }

    pthread_t t_proiettili;
    pthread_create(&t_proiettili, NULL, thread_coccodrillo, (void*)projectiles);
    pthread_detach(t_proiettili);

    return projectiles;
}

void* thread_proiettile(void* arg) {

    Projectile* projectiles = (Projectile*)arg;
    Projectile localProj[MAX_CROCODILES];
    int active = 0, updated = 0;

    for (int i = 0; i < MAX_CROCODILES; i++){
        localProj[i] = projectiles[i];
        active++;
    }
    struct timeval now;

    while(active>0){  
        usleep(50 * 1000);  // sleep 100 ms
        active = MAX_CROCODILES;
        updated = 0;

        for (int i = 0; i < MAX_CROCODILES; i++){

            if(!atomic_load(&projectiles[i].alive)){
                active--;
                continue;
            }

            gettimeofday(&now, NULL);
            const float dt = (now.tv_sec - localProj[i].prev.tv_sec) + (now.tv_usec - localProj[i].prev.tv_usec) / 1000000.0f;
            const int dx = localProj[i].dir * (int)(localProj[i].speed * dt);

            if (dx == 0) continue;
            
            printf("Projectile %d moved from %d to %d\n", i, localProj[i].x, localProj[i].x + dx);

            localProj[i].x += dx;
            localProj[i].prev = now;   
            updated ++;
        }
        if (updated > 0 ){
            int* msgProj = malloc(MAX_CROCODILES * sizeof(int));
            for (int i = 0; i < MAX_CROCODILES; i++){
                msgProj[i] = localProj[i].x;
            }
            Message newMess;
            newMess.type = PROJ_STATUS;
            newMess.data = msgProj;

            push_event(&myBuffer, &newMess);
        }
    }
    pthread_exit(NULL);
}

void sparaProiettile(Projectile* proj, const Crocodile* croc, const float time, const gameConfig* gameCfg) {
        
    if (atomic_load(&proj->alive)) return;

    printf("Stiamo sparando un proiettile da coccodrillo %d\n", croc->idx);

    proj->x = (croc->dir == 1) ? croc->x + 5 : croc->x - 5;;
    proj->y = croc->y;
    atomic_store(&proj->alive, true);
    proj->dir = croc->dir;
    gettimeofday(&proj->prev, NULL);
    proj->speed = gameCfg->velocità_proiettili;
}

//---------------------------------------------------------------------------

