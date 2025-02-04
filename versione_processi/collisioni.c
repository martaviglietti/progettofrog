#include "header.h"
int CollisioneRanaProiettile(Rana rana,Proiettile proiettile){

	if((rana.x+1==proiettile.x || rana.x-1==proiettile.x) && rana.y-1==proiettile.y) return 1;
	else return 0;   
}

int RanaSuTana(Rana rana, Game_struct* game_struct){
	 
	 for(int i=0;i<5;i++){
	 	if(rana.x>=10+8*i+6*i && rana.x<=11+8*i+6*i){
	 		if(game_struct->tane[i]==0){
	 		 	 game_struct->tane[i]=1;
	 			 return 1;}
	 }}
	 return 0;
}
	 	
	 	
	 		 

int RanaSuCoccodrillo(Rana *rana, Coccodrillo *coccodrilli){
    for (int i = 0; i < MAX_CROCODILES; i++) {
        if (coccodrilli[i].alive) {

            if ((rana->y == coccodrilli[i].y) &&  (rana->x-1>= coccodrilli[i].x-4) && (rana->x+1 <= coccodrilli[i].x+4)) {
                return coccodrilli[i].id; // La rana è sopra un coccodrillo
            }
        }
    }

    return -1; 
}





