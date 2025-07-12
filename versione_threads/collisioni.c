//funzione di controllo collisione rana-proiettile
#include "header.h"

void* thread_grafica(void* arg) {
    Game_struct* game_struct = (Game_struct*)arg;
    WINDOW *game = game_struct->game;

    //Inizializzazione tempo
    Time* time = timeInit(game_struct->gameCfg);

    // Inizializzazione oggetti
    Frog* frog = frogInit();
    Crocodile* crocodiles = CrocodileInit(game_struct->gameCfg->flussi, time->time);
    Projectile* projectiles = ProjectileInit();
    Projectile* granates = GranateInit();

    while (1) {

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            break;
        }

        Message newMess = pop_event(&myBuffer);
        switch (newMess.type) {

            case TIME_STATUS:
                time->time = *((float*)newMess.data);
                break;

            case FROG_STATUS:
                int newX = ((int*)newMess.data)[0];
                int newY = ((int*)newMess.data)[1];
                if (newY < RANA_YINIT) frog->y = newY;
                if (newX < RANA_XMAX && newX > RANA_XMIN) frog->x = newX;
                break;

            case CROC_STATUS:
                break;

            case PROJ_STATUS:
                break;

            case GRAN_STATUS:
                break;
        
        }
        free(newMess.data);

        bool newManche = 0;

        if (time <= 0) {
            newManche = true;
            game_struct->score -= 20;
            game_struct->vite--;
        }

        if (!newManche && frog->y < TANA_POS){

            RanaSuTana(frog, gameCfg);
            newManche = true; 

        }
        //altri controlli

        //check if frog is on a crocodile or hit by a projectile
        if (frogLocal.y < waterYlow  && frogLocal.y > waterYtop){     //if frog in water region
            
            const int crocId = RanaSuCoccodrillo(&frogLocal, gameCfg);

            if(crocId == -1){              //rana fell in the water
                newManche = true;
                printf("GestGraph: la rana é caduta in acqua...\n");
            }
            else{
                frogLocal.crocIdx = crocId;
                printf("GestGraph: la rana é sul coccodrillo %d\n", crocId);
            }

            if(CollRanaProiettile(&frogLocal, gameCfg)){
                printf("GestGraph: rana colpita da un proiettile...\n");
                newManche = true;
            }
        }


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

        //Check objects are inside h window
        

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
        if (key != ERR && frogLocal.key == -1){
            LOCK_FROG();
            Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
            frog->key = key;
            UNLOCK_FROG();
            case 's':  // spara granate
                sparaGranata(&frogLocal, gameLocal.time, gameCfg);
                break;
        }
        //int key = 258 +rand_funz(0,3);

        wclear(game);
        wrefresh(game);

    }



}

void RanaSuTana(const Frog* frog, const gameConfig* gameConfig){

    bool isSucc = false;

    LOCK_WRITE_GAME();
    Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];
    
    for (int i = 0; i < NTANE; i++) {
        if (frog->x >= 8 + (15) * i && frog->x < 8 + (15) * i + 5 && game_struct->tane[i] == 0) { 
            game_struct->tane[i] = 1;
            game_struct->tane_count++;
            isSucc = true;
        }
    }
    
    if(!isSucc){
        printf("NewManche: rana haraggiungo l'estremo superiore della mappa ma mancando le tane...\n");
        game_struct->vite--;
        game_struct->score -= 10;
    }
    else{
        printf("NewManche: rana ha raggiunto una delle tane\n");
        game_struct->score += 15 + (int)(15 * game_struct->time / 100);

        if (game_struct->tane_count == NTANE){
            game_struct->win = 1;
            game_struct->score += 100;
        }
    }
    game_struct->time = gameConfig->tempo;
    UNLOCK_GAME();
}

//funzione che verifica se la rana è su un coccodrillo
int RanaSuCoccodrillo(const Frog *frog, const gameConfig* gameConfig){

    const int frog_x = frog->x;
    const int frog_y = frog->y;

    LOCK_CROCS();
    for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++) {
        const Crocodile* croc = (Crocodile*)buffer.buffer[i];

        if (croc->alive) {
            const int croc_x = croc->x;
            const int croc_y = croc->y;
            if(frog_y <= croc_y + DIM_FLUSSI/2 && frog_y >= croc_y - DIM_FLUSSI/2){
                if ((frog_x >= croc_x-3) && (frog_x <= croc_x+3)){
                    UNLOCK_CROCS();
                    return i;
                }
            }
        }
    }
    UNLOCK_CROCS();

    LOCK_WRITE_GAME();
    Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];
    game_struct->vite--;
    game_struct->score -= 10;
    game_struct->time = gameConfig->tempo;
    UNLOCK_GAME();

    return -1; 
}

bool CollRanaProiettile(const Frog* frog, const gameConfig* gameConfig){

    const int frog_x = frog->x;
    const int frog_y = frog->y;

    LOCK_PROJ();

    for (int i = IDX_PROIETTILI; i < IDX_PROIETTILI + MAX_CROCODILES; i++) {
        Projectile* proj = (Projectile*)buffer.buffer[i];

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
                    UNLOCK_PROJ();

                    LOCK_WRITE_GAME();
                    Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];
                    game_struct->vite--;
                    game_struct->score -= 15;
                    game_struct->time = gameConfig->tempo;
                    UNLOCK_GAME();

                    return 1;
                }
            }
        }
    }
    UNLOCK_PROJ();
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




