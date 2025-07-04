//funzione di controllo collisione rana-proiettile
#include "header.h"


bool RanaSuTana(const Frog* frog, Game_struct* game_struct){

    for (int i = 0; i < NTANE; i++) {
        if (frog->x >= 8 + (15) * i && frog->x < 8 + (15) * i + 5 && game_struct->tane[i] == 0) { 
            game_struct->tane[i] = 1;
            return 1;
        }
    }

    return 0;
}

//funzione che verifica se la rana è su un coccodrillo
int RanaSuCoccodrillo(const Frog *frog){
    for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++) {
        const Crocodile* croc = (Crocodile*)buffer.buffer[i];
        
        const int frog_x = frog->x;
        const int frog_y = frog->y;

        if (croc->alive) {
            const int croc_x = croc->x;
            const int croc_y = croc->y;
            if(frog_y <= croc_y + DIM_FLUSSI/2 && frog_y >= croc_y - DIM_FLUSSI/2){
                if ((frog_x-1 >= croc_x-4) && (frog_x+1 <= croc_x+4)){
                    return i;
                }
            }
        }
    }
    return -1; 
}

bool CollRanaProiettile(const Frog* frog){

    const int frog_x = frog->x;
    const int frog_y = frog->y;

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
                return 1;
            }
        }
    }
    return 0;
}




