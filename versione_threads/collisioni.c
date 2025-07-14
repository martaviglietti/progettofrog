//funzione di controllo collisione rana-proiettile
#include "header.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

void* thread_grafica(void* arg) {

    Game_struct* game_struct = (Game_struct*)arg;
    WINDOW *game = game_struct->game;

    //Inizializzazione tempo
    //Time* time = timeInit();
    struct timeval now, prev;
    gettimeofday(&prev, NULL);
    float time = 0.;
    //printf("Tempo inizializzato a %f\n", time->time);

    // Inizializzazione oggetti
    Frog* frog = frogInit();
    Crocodile* crocodiles = CrocodileInit(game_struct->gameCfg->flussi, time);
    Projectile* projectiles = NULL;
    Projectile* granates = NULL;

    const int waterYtop = TANA_POS + SPONDA_SUPERIORE;
    const int waterYlow = waterYtop + NFLUSSI * DIM_FLUSSI;

    while (1) {

        gettimeofday(&now, NULL);
        time += (now.tv_sec - prev.tv_sec) + (now.tv_usec - prev.tv_usec) / 1000000.0f;
        prev = now;

        //printf("new iteration of graphics...\n");
        int newX, newY, crocId;

        Message newMess = pop_event(&myBuffer);
        //printf("We are reading a message of type %d.\n", newMess.type);
        switch (newMess.type) {

            case TIME_STATUS:
                //printf("Updating time from %f to %f\n", time->time, *((float*)newMess.data));
                //time->time = *((float*)newMess.data);
                break;

            case FROG_STATUS:
                newX = ((int*)newMess.data)[0];
                newY = ((int*)newMess.data)[1];
                pthread_mutex_lock(&frog->mutex);
                if (newY < RANA_YINIT) frog->y = newY;
                if (newX < RANA_XMAX && newX > RANA_XMIN) frog->x = newX;
                pthread_mutex_unlock(&frog->mutex);
                break;

            case CROC_STATUS:
                crocId = ((int*)newMess.data)[0];
                newX = ((int*)newMess.data)[1];

                crocodiles[crocId].x = newX;
                break;

            case PROJ_STATUS:
                if (projectiles == NULL) break;
                for (int i = 0; i < MAX_CROCODILES; i++){
                    if (atomic_load(&projectiles[i].alive)){
                        projectiles[i].x = ((int*)newMess.data)[i];
                    }
                }
                break;

            case GRAN_STATUS:
            if (granates == NULL) break;
                for (int i = 0; i < 2; i++){
                    if (atomic_load(&granates[i].alive)){
                        granates[i].x = ((int*)newMess.data)[i];
                    }
                }
                break;
        }
        free(newMess.data);

        bool newManche = 0;

        // Check if there is still time
        //printf("Checking time limit...\n");
        if (time > game_struct->gameCfg->tempo) {
            newManche = true;
            game_struct->score -= 20;
            game_struct->vite--;
        }
        
        // Check if the frog didn't reach the region of tane
        //printf("Check if the frog didn't reach the region of tane...\n");
        if (!newManche && frog->y < TANA_POS){

            newManche = true; 
            bool isSucc = RanaSuTana(frog, game_struct);
            
            if(!isSucc){
                //printf("NewManche: rana haraggiungo l'estremo superiore della mappa ma mancando le tane...\n");
                game_struct->vite--;
                game_struct->score -= 10;
            }
            else{
                //printf("NewManche: rana ha raggiunto una delle tane\n");
                game_struct->score += 15 + (int)(15 * time / 100);

                if (game_struct->tane_count == NTANE){
                    game_struct->win = 1;
                    game_struct->score += 100;
                }
            }
        }
        
        //check if frog is on a crocodile or hit by a projectile
        //printf("Check if the frog frog is on a crocodile or hit by a projectile...\n");
        if (!newManche && frog->y < waterYlow  && frog->y > waterYtop){     //if frog in water region
            
            const int crocId = RanaSuCoccodrillo(frog, crocodiles);

            if(crocId == -1){              //rana fell in the water
                newManche = true;
                //printf("GestGraph: la rana é caduta in acqua...\n");
                game_struct->vite--;
                game_struct->score -= 10;
            }
            else{
                
                //printf("GestGraph: la rana é arrivata sul coccodrillo %d\n", crocId);
                //printf("GestGraph: Rana si muove sul coccodrillo %d, oldPos = %d, newPos = %d\n", crocId, frog->x, newX);
                pthread_mutex_lock(&frog->mutex);
                frog->crocIdx = crocId;
                if (crocodiles[crocId].x > RANA_XMIN && crocodiles[crocId].x < RANA_XMAX) frog->x = crocodiles[crocId].x;
                pthread_mutex_unlock(&frog->mutex);
                //}
            }

            if(CollRanaProiettile(frog, projectiles)){
                //printf("GestGraph: rana colpita da un proiettile...\n");
                newManche = true;
                game_struct->vite--;
                game_struct->score -= 15;
            }
        }
        
        //If the manche is not ended, check that every object is inside the window 
        if (!newManche){

            //coccodrilli
            for (int i = 0; i < MAX_CROCODILES; i++){
                Crocodile* crocod = &crocodiles[i]; 
                //printf("Checking crcodile %d...", i);

                if (atomic_load(&crocod->alive)){   // if the crocodile is alive, we check the position and eventually we turn it off
                    if ((crocod->x < POS_SPAWN_COC_SINISTRA  && crocod->dir == -1) || (crocod->x > POS_SPAWN_COC_DESTRA  && crocod->dir == 1)){     // coccodrillo fuori mappa, quindi muore
                        //printf(" is out of map\n");
                        atomic_store(&crocod->alive, false);
                        crocod->wait = time + rand_funz(CROC_MIN_WAIT, CROC_MAX_WAIT);
                        crocod->x = -1;
                        crocod->y = -1;
                        crocod->dir = -1;
                        crocod->speed = -1;
                        gettimeofday(&crocod->prev, NULL);
                        continue;
                    }

                    //controllo se Far sparare i proiettili
                    if (time >= crocod->wait){       //coccodrillo spara il proiettile
                        //printf("Crocodile %d is shooting a projectile\n", i);
                        if (projectiles == NULL){          // se é il primo proiettile sparato
                            projectiles = ProjectileInit(crocod,  time, game_struct->gameCfg);
                        }
                        else sparaProiettile(&projectiles[crocod->idx], crocod, time, game_struct->gameCfg);

                        crocod->wait = time + rand_funz(PROJ_MIN_WAIT, PROJ_MAX_WAIT);
                        usleep(5 * 1000);  // sleep 10 ms
                        continue;
                    }
                }
                else{                              // if the crocodile is NOT alive, we check if they can spawn or not
                    if (time >= crocod->wait){      // creiamo il coccodrillo
                        //printf(" is spawning\n");

                        const int id_flusso = rand_funz(0, NFLUSSI-1);
                        const Flusso* flux = &game_struct->gameCfg->flussi[id_flusso];
                        
                        atomic_store(&crocod->alive, true);
                        crocod->x = (flux->dir == 1) ? POS_SPAWN_COC_SINISTRA - 1 : POS_SPAWN_COC_DESTRA + 1;
                        crocod->y = flux->y;
                        crocod->dir = flux->dir;
                        crocod->speed = flux->speed;
                        crocod->wait = time + rand_funz(PROJ_MIN_WAIT, PROJ_MAX_WAIT);
                        gettimeofday(&crocod->prev, NULL);

                        pthread_t t_coccod;
                        pthread_create(&t_coccod, NULL, thread_coccodrillo, (void*)crocod);
                        pthread_detach(t_coccod);
                        continue;
                    }
                }
                //printf("\n");
            }
            
            //Granate
            if (granates != NULL){
                bool all_dead = true;
                for (int i = 0; i < 2; i++){
                    Projectile* gran = &granates[i];
                    //printf("Checking granade %d...", i);

                    if (!atomic_load(&gran->alive)) continue;

                    if (gran->x <= 0 || gran->x >= LARGHEZZA_GIOCO || CollGranataProiettile(gran, projectiles)){
                        atomic_store(&gran->alive, false);
                        gran->x = -1;
                        gran->y = -1;
                        gran->dir = -1;
                        gran->speed = -1;
                        gettimeofday(&gran->prev, NULL);
                        continue;
                    }
                    all_dead = false;
                }
                if (all_dead){
                    free(granates);
                    granates = NULL;
                }
            }
            
            // Proiettili
            if (projectiles != NULL){
                bool all_dead = true;
                for (int i = 0; i < MAX_CROCODILES; i++) {
                    Projectile* proj = &projectiles[i];
                    //printf("Checking projectile %d...", i);

                    if (!atomic_load(&proj->alive)) continue;

                    if ((proj->x <= 0  && proj->dir == -1) || (proj->x >= LARGHEZZA_GIOCO  && proj->dir == 1)){     // proiettile fuori mappa, quindi muore
                        atomic_store(&proj->alive, false);
                        proj->x = -1;
                        proj->y = -1;
                        proj->dir = -1;
                        proj->speed = -1;
                        gettimeofday(&proj->prev, NULL);
                        continue;
                    }
                    all_dead = false;
                }
                if (all_dead){
                    free(projectiles);
                    projectiles = NULL;
                }
            }
        }    
        
        werase(game);
        windowGeneration(game, COLS, LINES, game_struct);
    
        drawCoccodrilli(game, crocodiles);
        draw_granate(game, granates);
        draw_proiettile(game, projectiles);
        draw_frog(game, frog);

        // Punteggio
        wattron(game, COLOR_PAIR(15));
        mvwprintw(game, 2, 2, "Punteggio: %d ", game_struct->score);
        wattroff(game, COLOR_PAIR(15));

        // Vite
        wattron(game, COLOR_PAIR(15));
        mvwprintw(game, 2, 50, "Vite:");
        mvwhline(game, 2, 55, ' ', 21);
        for (int i = 0; i < game_struct->vite; i++) {
            mvwprintw(game, 2, 55 + i * 2, "❤️");
        }
        wattroff(game, COLOR_PAIR(15));
        // Tempo
        wattron(game, COLOR_PAIR(15));
        mvwhline(game, 46, 2, ' ', 10);
        mvwprintw(game, 46, 2, "Tempo: %d ", (int)time);
        wattroff(game, COLOR_PAIR(15));

        print_tempo(game,game_struct, (int)time);
        wrefresh(game);

        keypad(game, true);  // abilita frecce
        nodelay(game, TRUE); // aspetta input (puoi metterlo TRUE se vuoi non bloccare il loop)
        int key = wgetch(game);
        //int key = 258 +rand_funz(0,3);

        pthread_mutex_lock(&frog->mutex);
        if (key != ERR && frog->key == -1){
            //printf("Key pressed: %d\n", key);
            if(key == 's' && granates == NULL){
                granates = GranateInit(frog, time, game_struct->gameCfg);
            }
            else{
                frog->key = key;
            }
        }
        pthread_mutex_unlock(&frog->mutex);

        if(newManche){
            //printf("New manche...\n");

            //atomic_store(&time->alive, false);
            atomic_store(&frog->alive, false);
            for (int i = 0; i < MAX_CROCODILES; i++){
                atomic_store(&crocodiles[i].alive, false);
            }
            if (projectiles != NULL){
                for (int i = 0; i < MAX_CROCODILES; i++){
                    atomic_store(&projectiles[i].alive, false);
                }
            }
            if (granates != NULL){
                for (int i = 0; i < 2; i++){
                    atomic_store(&granates[i].alive, false);
                }
            }

            usleep(200 * 1000);  // sleep 10 ms
            free(crocodiles);
            free(frog);
            if (granates != NULL) {
                free(granates);
                granates =NULL;
            }
            if (projectiles != NULL) {
                free(projectiles);
                projectiles=NULL;
            }

            if (game_struct->win != 1 && game_struct->vite > 0){

                time = 0.;// timeInit();
                frog = frogInit();
                crocodiles = CrocodileInit(game_struct->gameCfg->flussi, time);
                projectiles = NULL;
                granates = NULL;
            }
            else break;
            fflush(stdout);
        }
    }
    pthread_exit(NULL);
}

bool RanaSuTana(const Frog* frog, Game_struct* game_struct){
    
    for (int i = 0; i < NTANE; i++) {
        if (frog->x >= 8 + (15) * i && frog->x < 8 + (15) * i + 5 && game_struct->tane[i] == 0) { 
            game_struct->tane[i] = 1;
            game_struct->tane_count++;
            return true;
        }
    }
    return false;
}

//funzione che verifica se la rana è su un coccodrillo
int RanaSuCoccodrillo(const Frog *frog, const Crocodile* croc){

    const int frog_x = frog->x;
    const int frog_y = frog->y;

    for (int i = 0; i < MAX_CROCODILES; i++) {
        if (atomic_load(&croc[i].alive)) {
            const int croc_x = croc[i].x;
            const int croc_y = croc[i].y;
            if(frog_y <= croc_y + DIM_FLUSSI/2 && frog_y >= croc_y - DIM_FLUSSI/2){
                if ((frog_x >= croc_x-3) && (frog_x <= croc_x+3)){
                    return i;
                }
            }
        }
    }
    return -1; 
}

bool CollRanaProiettile(const Frog* frog, Projectile* projectiles){

    if (projectiles == NULL) return 0;

    const int frog_x = frog->x;
    const int frog_y = frog->y;

    for (int i = 0; i < MAX_CROCODILES; i++) {
        Projectile* proj = &projectiles[i];

        if (atomic_load(&proj->alive)){
            const int proj_x = proj->x;
            const int proj_y = proj->y;
            if(proj_y <= frog_y + ALTEZZARANA/2 && proj_y >= frog_y - ALTEZZARANA/2){
                if (proj_x <= frog_x + LARGHEZZARANA/2 && proj_x >= frog_x - LARGHEZZARANA/2){
                    proj->x = -1;
                    proj->y = -1;
                    atomic_store(&proj->alive, false);
                    proj->dir = -1;
                    proj->speed =-1;
                    gettimeofday(&proj->prev, NULL);
                    return 1;
                }
            }
        }
    }
    return 0;
}

bool CollGranataProiettile(const Projectile* gran, Projectile* projectiles){

    if (projectiles == NULL) return 0;

    const int gran_x = gran->x;
    const int gran_y = gran->y;

    for (int i = 0; i < MAX_CROCODILES; i++) {
        Projectile* proj = (Projectile*)&projectiles[i];

        if (atomic_load(&proj->alive)){

            const int proj_x = proj->x;
            const int proj_y = proj->y;

            if(proj_y == gran_y  && proj_x <= gran_x + 1 && proj_x >= gran_x - 1){
                proj->x = -1;
                proj->y = -1;
                atomic_store(&proj->alive, false);
                proj->dir = -1;
                proj->speed = -1;
                gettimeofday(&proj->prev, NULL);

                return 1;
            }
        }
    }
    return 0;
}




