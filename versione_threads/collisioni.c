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


int CollisioneRanaProiettile(Rana rana,Proiettile proiettile){

    if((rana.x+1==proiettile.x || rana.x-1==proiettile.x) && rana.y==proiettile.y && proiettile.alive) return 1;
    else return 0;   
}





