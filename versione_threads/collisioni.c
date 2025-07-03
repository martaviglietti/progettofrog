//funzione di controllo collisione rana-proiettile
#include "header.h"


bool RanaSuTana(const Frog* frog, const Game_struct* game_struct){

    ///// aggiungere chiusura tana

    for (int i = 0; i < 5; i++) {
        if (rana.x >= 8 + (15) * i && rana.x < 8 + (15) * i + 5 && game_struct->tane[i] == 0) {        //?????///////////
            game_struct->tane[i] = 1;
            return 1;
        }
    }

    
}








int CollisioneRanaProiettile(Rana rana,Proiettile proiettile){

    if((rana.x+1==proiettile.x || rana.x-1==proiettile.x) && rana.y==proiettile.y && proiettile.alive) return 1;
    else return 0;   
}





//funzione che verifica se la rana è su un coccodrillo
int RanaSuCoccodrillo(Rana *rana, Coccodrillo *coccodrilli){
    for (int i = 0; i < MAX_CROCODILES; i++) {
        if (coccodrilli[i].alive) {
            if ((rana->y == coccodrilli[i].y) &&  (rana->x-1>= coccodrilli[i].x-4) && (rana->x+1 <= coccodrilli[i].x+4)) {
                return coccodrilli[i].id; 
            }
        }
    }
    return -1; 
}