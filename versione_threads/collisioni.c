//funzione di controllo collisione rana-proiettile
#include "header.h"
#include <string.h>

void* thread_grafica(void* arg) {
    Game_struct* game_struct = (Game_struct*)arg;
    WINDOW *game = game_struct->game;

    //Inizializzazione tempo
    Time* time = timeInit(game_struct->gameCfg);

    // Inizializzazione oggetti
    Frog* frog = frogInit();
    Crocodile* crocodiles = CrocodileInit(game_struct->gameCfg->flussi, time->time);
    Projectile* projectiles = ProjectileInit();
    Projectile* granates = NULL;

    const int waterYtop = TANA_POS + SPONDA_SUPERIORE;
    const int waterYlow = waterYtop + NFLUSSI * DIM_FLUSSI;

    while (1) {

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            break;
        }
        int newX, newY, crocId;

        Message newMess = pop_event(&myBuffer);
        switch (newMess.type) {

            case TIME_STATUS:
                time->time = *((float*)newMess.data);
                break;

            case FROG_STATUS:
                newX = ((int*)newMess.data)[0];
                newY = ((int*)newMess.data)[1];

                if (newY < RANA_YINIT) frog->y = newY;
                if (newX < RANA_XMAX && newX > RANA_XMIN) frog->x = newX;
                break;

            case CROC_STATUS:
                crocId = ((int*)newMess.data)[0];
                newX = ((int*)newMess.data)[1];

                crocodiles[crocId].x = newX;
                
                if (crocId == frog->crocIdx && newX > RANA_XMIN && newX < RANA_XMAX){
                    printf("GestGraph: Rana si muove sul coccodrillo %d, oldPos = %d, newPos = %d\n", crocId, frog->x, newX);
                    frog->x = newX;
                }
                break;

            case PROJ_STATUS:
                break;

            case GRAN_STATUS:
                break;
        
        }
        free(newMess.data);

        bool newManche = 0;

        // Check if there is still time
        if (time <= 0) {
            newManche = true;
            game_struct->score -= 20;
            game_struct->vite--;
        }

        // Check if the frog didn't reach the region of tane
        if (!newManche && frog->y < TANA_POS){

            newManche = true; 
            bool isSucc = RanaSuTana(frog, game_struct);
            
            if(!isSucc){
                printf("NewManche: rana haraggiungo l'estremo superiore della mappa ma mancando le tane...\n");
                game_struct->vite--;
                game_struct->score -= 10;
            }
            else{
                printf("NewManche: rana ha raggiunto una delle tane\n");
                game_struct->score += 15 + (int)(15 * time->time / 100);

                if (game_struct->tane_count == NTANE){
                    game_struct->win = 1;
                    game_struct->score += 100;
                }
            }
        }

        //check if frog is on a crocodile or hit by a projectile
        if (!newManche && frog->y < waterYlow  && frog->y > waterYtop){     //if frog in water region
            
            const int crocId = RanaSuCoccodrillo(frog, crocodiles);

            if(crocId == -1){              //rana fell in the water
                newManche = true;
                printf("GestGraph: la rana é caduta in acqua...\n");
                game_struct->vite--;
                game_struct->score -= 10;
            }
            else{
                frog->crocIdx = crocId;
                printf("GestGraph: la rana é arrivata sul coccodrillo %d\n", crocId);
            }

            if(CollRanaProiettile(frog, projectiles)){
                printf("GestGraph: rana colpita da un proiettile...\n");
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

                if (atomic_load(&crocod->alive)){   // if the crocodile is alive, we check the position and eventually we turn it off
                    if ((crocod->x < POS_SPAWN_COC_SINISTRA  && crocod->dir == -1) || (crocod->x > POS_SPAWN_COC_DESTRA  && crocod->dir == 1)){
                        atomic_store(&crocod->alive, false);
                        crocod->wait = time->time - rand_funz(3, 10);
                        crocod->x = -1;
                        crocod->y = -1;
                        crocod->dir = -1;
                        crocod->speed = -1;
                        crocod->tempo_prec = time->time;
                    }
                }
                else{                              // if the crocodile is NOT alive, we check if they can spawn or not

                }
                
            }



        }




        
        if (!gran->alive || CollGranataProiettile(gran)){
                continue;
            }
        

        //Check objects are inside h window
        for granates like: if (newX > 0 && newX < LARGHEZZA_GIOCO){   else gran->alive=0;
        
            fro croc: if (newX >= POS_SPAWN_COC_SINISTRA && newX <= POS_SPAWN_COC_DESTRA){    // nuova posizione valida

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

                    


        // far spownare nuovi coccodrrilli
        for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++){
            Crocodile* crocod = (Crocodile*)buffer.buffer[i];

            if (crocod->alive){
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
        // Far sparare i proiettili
        if (time <= crocod->wait){       //coccodrillo spara il proiettile
                            sparaProiettile(time, gameCfg, i-IDX_COCCODRILLI);
                        }


        werase(game);
        windowGeneration(game, COLS, LINES, game_struct);
    
        drawCoccodrilli(game);
        draw_granate(game);
        draw_proiettile(game);
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
        mvwprintw(game, 46, 2, "Tempo: %d ", (int)time->time);
        wattroff(game, COLOR_PAIR(15));

        print_tempo(game,game_struct, (int)time->time);
        wrefresh(game);

        keypad(game, true);  // abilita frecce
        nodelay(game, TRUE); // aspetta input (puoi metterlo TRUE se vuoi non bloccare il loop)
        int key = wgetch(game);

        pthread_mutex_lock(&frog->mutex);
        if (key != ERR && frog->key == -1){

            if(key == 's' && granates == NULL){
                granates = GranateInit();
                GranateInit(frog, time->time, game_struct->gameCfg);
            }
            else{
                frog->key = key;
            }
        }
        pthread_mutex_unlock(&frog->mutex);
        //int key = 258 +rand_funz(0,3);

        wclear(game);
        wrefresh(game);









        if(newManche){

            time->alive = false;
            frog->alive = false;
            
            usleep(100 * 1000);  // sleep 10 ms

            free(time);

            if (game_struct->win != 1 && game_struct->vite > 0){

                time = timeInit(game_struct->gameCfg);


                restartFrog();

            }
        }
    }



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
        if (croc[i].alive) {
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

    const int frog_x = frog->x;
    const int frog_y = frog->y;

    for (int i = 0; i < MAX_CROCODILES; i++) {
        Projectile* proj = &projectiles[i];

        if (proj->alive){
            const int proj_x = proj->x;
            const int proj_y = proj->y;
            if(proj_y <= frog_y + ALTEZZARANA/2 && proj_y >= frog_y - ALTEZZARANA/2){
                if (proj_x <= frog_x + LARGHEZZARANA/2 && proj_x >= frog_x - LARGHEZZARANA/2){
                    proj->x = -1;
                    proj->y = -1;
                    proj->alive = 0;
                    proj->dir = -1;
                    proj->speed =-1;
                    proj->tempo_prec = -1;
                    return 1;
                }
            }
        }
    }
    return 0;
}

bool CollGranataProiettile(Projectile* gran){

    const int gran_x = gran->x;
    const int gran_y = gran->y;

    LOCK_PROJ();

    for (int i = IDX_PROIETTILI; i < IDX_PROIETTILI + MAX_CROCODILES; i++) {
        Projectile* proj = (Projectile*)buffer.buffer[i];

        if (proj->alive){
            const int proj_x = proj->x;
            const int proj_y = proj->y;
            if(proj_y == gran_y  && proj_x <= gran_x + 1 && proj_x >= gran_x - 1){
                proj->x = -1;
                proj->y = -1;
                proj->alive = 0;
                proj->dir = -1;
                proj->speed =-1;
                proj->tempo_prec = -1;

                gran->x = -1;
                gran->y = -1;
                gran->alive = 0;
                gran->tempo_prec = -1;
                gran->dir = -1;
                gran->speed =-1;
                UNLOCK_PROJ();
                return 1;
            }
        }
    }
    UNLOCK_PROJ();
    return 0;
}




