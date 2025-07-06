#include "header.h"

//sprite rana
const char *frog_sprite[2] = {
    "* *",
    "   "
};

const char *coc_sprite[2][2]={{"XXXXXXX","XXXXXX0X0"},{"XXXXXXX","0X0XXXXXX"}};

//funzione che crea i colori utilizzati durante il gioco
void creazione_colori(){
    if (has_colors()) { //verifica il supporto ai colori da parte del terminale
        start_color();
        init_color(8,0,300,0);       //verde foresta
   	    init_color(9,0,123,184);     //blu scuro		
  	    init_color(10,95,95,95);     //grigio scuro
  	    init_color(11,192,192,192);  //grigio chiaro
   	    init_color(12,101,67,33);    //marrone
   	    init_color(13,230,118,84);   //marrone pastello
   	    init_color(14,50,300,50);    //verde chiaro
        init_color(15,0,500,0);      //verde chiaro
        init_color(16, 150,200,0);   //verde scuro
        init_color(17,100,500,700);  //blu chiaro
        init_color(18,800,800,0);  //giallo chiaro
        init_color(19,350,350,350);  //grigio 
        
    	init_pair(1,COLOR_BLACK,14);               //rana
   	    init_pair(2, COLOR_BLUE, 17);	           //fiume
   	    init_pair(3, 19, COLOR_YELLOW);  //zona in torno alle tane
   	    init_pair(4, 10, 11);                      //recinzione delle tane
    	init_pair(5, 13, 13); 		           //tana vera e propria
   	    init_pair(6, COLOR_YELLOW, 16);            //marciapiede
   	    init_pair(7, COLOR_WHITE, COLOR_MAGENTA);  //punteggio e vite	
   	    init_pair(8, 8, 15);   	                   //prato;
        init_pair(9,COLOR_BLACK,COLOR_WHITE);	   //selezione menù
        init_pair(10,COLOR_BLACK,14);              //coccodrillo
        init_pair(11,COLOR_RED,17);	   //proiettile
        init_pair(12,COLOR_RED,15);    //granata nel prato
        init_pair(13,COLOR_RED,17);	   //granata fiume
        init_pair(14,COLOR_RED,COLOR_RED); 	   //zona esterna al gioco
        init_pair(15,COLOR_WHITE,14);		   //colore score-vite-tempo
        init_pair(16,COLOR_BLACK,18);      //occhi rana e coccodrilli    
        init_pair(17,COLOR_RED,16);     //granata nel marciapiede 
                
    } else {  //il terminale non supporta i colori
    	printw("Il terminale non permette la visualizzazione dei colori\n"); 
    	mvprintw(LINES-1/2,COLS/2,"Premi un tasto per uscire...");refresh();
    	getch();
    	_exit(0);
    }
	
}

//funzione per disegnare il proiettile nella mappa
void draw_proiettile(WINDOW* game){
	
    LOCK_PROJ();
    for (int i = IDX_PROIETTILI; i< IDX_PROIETTILI + MAX_CROCODILES; i++){
        const Projectile* proj = (Projectile*)buffer.buffer[i];

        if (proj->alive) {  //mostriamo il proiettile solo se vivo		
            wattron(game,COLOR_PAIR(11));
            mvwaddch(game,proj->y,proj->x,'*');		
            wattroff(game,COLOR_PAIR(11));
        }
    }	
    UNLOCK_PROJ();	
}




void draw_granate(WINDOW* game){
	
    LOCK_FROG();
    for (int i = IDX_GRANATE; i < IDX_GRANATE + 2; i++){
        const Projectile* gran = (Projectile*)buffer.buffer[i];

        if (gran->alive) { //mostriamo le granate solo se vive
                    
            if (gran->y<15 && gran->y>=10) {  //granata sul marciapiede
            wattron(game,COLOR_PAIR(6));
            mvwaddch(game,gran->y,gran->x,'O');
            wattroff(game,COLOR_PAIR(6));			
            }
            if (gran->y<40 && gran->y>=15) {  //granata sul fiume
            wattron(game,COLOR_PAIR(13));
            mvwaddch(game,gran->y,gran->x,'O');
            wattroff(game,COLOR_PAIR(13));
            }	
            if (gran->y<45 && gran->y>=40) {  //granata sul prato
            wattron(game,COLOR_PAIR(12));
            mvwaddch(game,gran->y,gran->x,'O');
            wattroff(game,COLOR_PAIR(12));
                }
        }                                                                                                                                                                                                                                                                  
    }
    UNLOCK_FROG();
}

//funzione che disegna la rana
void draw_frog(WINDOW *game, Frog* frog){
	
    
    wattron(game,COLOR_PAIR(1));
    
    for (int i = 0; i < ALTEZZARANA; i++) {
        for (int j = 0; j < LARGHEZZARANA; j++) {			
            mvwaddch(game, frog->y + i, frog->x -1 + j, frog_sprite[i][j]);
        }
    }
    wattron(game,COLOR_PAIR(16));
    mvwaddch(game, frog->y, frog->x -1, frog_sprite[0][0]);
    mvwaddch(game, frog->y, frog->x + 1, frog_sprite[0][2]);
    wattroff(game,COLOR_PAIR(16));
}

//funzione che disegna i coccodrilli
void drawCoccodrilli(WINDOW *game){

    int h;  //variabile utilizzata per tenere conto di quale parte del coccodrillo stiamo printando (inferiore o superiore)
    wattron(game, COLOR_PAIR(10));
    
    for (int i = IDX_COCCODRILLI; i < IDX_COCCODRILLI + MAX_CROCODILES; i++){
        pthread_mutex_lock(&buffer.mutex[i]); 
        const Crocodile* crocod = (Crocodile*)buffer.buffer[i];

        if (crocod->alive && crocod->dir==1) { //coccodrillo vivo e direzionato verso destra
          
            h=0; 
	        //printiamo la parte superiore del coccodrillo
            for (int j = 0; j < LARGHEZZACOCCODRILLO-2; j++) {
                if (crocod->x-3+j>0 && crocod->x-3+j<LARGHEZZA_GIOCO-1) {  //condizioni necessarie per printare solo le parti del coccodrillo all'interno della mappa	
                    
                        mvwaddch(game, crocod->y, crocod->x -3 + j, coc_sprite[0][h][j]);
                }
	       	      
    	    }
   	    
            h=1;
            //printiamo la parte inferiore del coccodrillo
            for (int j = 0; j < LARGHEZZACOCCODRILLO; j++) {
       	        if (crocod->x-4+j>0 && crocod->x-4+j<LARGHEZZA_GIOCO-1) {		
       	            if(coc_sprite[0][h][j]=='0'){
                        wattron(game, COLOR_PAIR(16));
                        mvwaddch(game, crocod->y + h, crocod->x -4 + j, coc_sprite[0][h][j]);
                        wattroff(game, COLOR_PAIR(16));
                        wattron(game, COLOR_PAIR(10));
                    
                    } else {	
                        mvwaddch(game, crocod->y + h, crocod->x -4 + j, coc_sprite[0][h][j]);
                    }
   	            }
            }

        } else if (crocod->alive && crocod->dir==-1) {   //coccodrillo vivo e direzionato verso sinistra
       	    
       	    h=0; 
            //printiamo la parte superiore del coccodrillo
            for (int j = 0; j < LARGHEZZACOCCODRILLO-2; j++) {
                if (crocod->x-3+j>0 && crocod->x-3+j<LARGHEZZA_GIOCO-1) {			
                    mvwaddch(game, crocod->y + h, crocod->x -3 + j, coc_sprite[1][h][j]);   //prima riga , colonna h, al carattere n j;
                }
            }

            h=1;           
            //printiamo la parte inferiore del coccodrillo
            for (int j = 0; j < LARGHEZZACOCCODRILLO; j++) {
       		    if (crocod->x-4+j>0 && crocod->x-4+j<LARGHEZZA_GIOCO-1) {			
       		     	
       		     	if(coc_sprite[1][h][j]=='0'){
                        wattron(game, COLOR_PAIR(16));
                        mvwaddch(game, crocod->y + h, crocod->x -4 + j, coc_sprite[1][h][j]);
                        wattroff(game, COLOR_PAIR(16));
                        wattron(game, COLOR_PAIR(10));
	            
	                } else {
       		     	    mvwaddch(game, crocod->y + h, crocod->x -4 + j, coc_sprite[1][h][j]);
     	    	    }
     	    	}  	    
   	        }
        }
        pthread_mutex_unlock(&buffer.mutex[i]); 
    }
    wattroff(game, COLOR_PAIR(10));
}

