//funzione di controllo collisione rana-proiettile
#include "header.h"

extern pthread_mutex_t mutex_tane;

int CollisioneRanaProiettile(Rana rana,Proiettile proiettile){

    if((rana.x+1==proiettile.x || rana.x-1==proiettile.x) && rana.y==proiettile.y && proiettile.alive) return 1;
    else return 0;   
}


int RanaSuTana(Rana rana, Game_struct* game_struct){
    pthread_mutex_lock(&mutex_tane);
    for (int i = 0; i < 5; i++) {
        if (rana.x >= 8 + (15) * i && rana.x < 8 + (15) * i + 5 && game_struct->tane[i] == 0) {
            game_struct->tane[i] = 1;
            pthread_mutex_unlock(&mutex_tane);
            return 1;
        }
    }
    pthread_mutex_unlock(&mutex_tane);
    return 0;
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


//funzione che controlla se la rana si trova in mappa
int RanaInFinestra(Rana rana, messaggio temp){
    if(rana.x+temp.x>78) return 0;
    if(rana.x+temp.x<2) return 0;
    if(rana.y+temp.y>43) return 0;
    if(rana.y+temp.y<7) return 0;
    return 1;

}