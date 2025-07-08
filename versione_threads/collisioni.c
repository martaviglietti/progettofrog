//funzione di controllo collisione rana-proiettile
#include "header.h"


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

bool CollRanaProiettile(const Frog* frog){

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




