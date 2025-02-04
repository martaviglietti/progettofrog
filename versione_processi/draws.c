#include "header.h"
#include <ncurses.h>
#include <stdlib.h> // per exit
#include <fcntl.h>  // per fcntl, F_GETFL

//funzione creazione colori e disegni oggetti


void creazione_colori(){
    if (has_colors()) {       //verifica il supporto ai colori da parte del terminale
        start_color();
        init_color(8,34,139,34); //verde foresta;
   	init_color(9,0,123,184); //blu scuro;		//in futuro li possiamo modificare ma l importante è non modificare l id;
  	init_color(10,95,95,95); //grigio scuro;
  	init_color(11,192,192,192); //grigio chiaro;
   	init_color(12,101,67,33); //marrone
   	init_color(13,152,118,84); //marrone pastello
   	init_color(14,50,205,50); //verde chiaro;
    
    
        
    	init_pair(1,8,14);                   //rana
   	init_pair(2, 9, COLOR_BLUE);	      // Fiume "~"
   	init_pair(3, COLOR_YELLOW, COLOR_YELLOW); // Tane zona intorno
   	init_pair(4, 10, 11);                     // Tane recinzione
    	init_pair(5, 13, 13); 		      //tana vera e propria
   	init_pair(6, COLOR_BLACK, COLOR_RED);     // Marciapiede
   	init_pair(7, COLOR_WHITE, COLOR_MAGENTA); // Punteggio e vite	
   	init_pair(8, 8, COLOR_GREEN);   	      //prato;
        init_pair(9,COLOR_BLACK,COLOR_WHITE);	   //selezione menù
        init_pair(10,COLOR_BLACK,8);               //coccodrillo
        init_pair(11,COLOR_BLACK,COLOR_BLUE);	   //proiettile;
                
    }else{
    	printw("Il terminale non permette la visualizzazione dei colori\n"); 
    	mvprintw(LINES-1/2,COLS/2,"Premi un tasto per uscire...");refresh();
    	getch();
    	_exit(0);
    }
	
	
}



void draw_proiettile(WINDOW* game, Proiettile proiettile){
	
	if(proiettile.alive){		//se il proiettile è vivo;
	        
		if(proiettile.y<15 && proiettile.y>=10){
			wattron(game,COLOR_PAIR(11));
			mvwaddch(game,proiettile.y,proiettile.x,'-');
			wattroff(game,COLOR_PAIR(11));			//impostiamo i colori come le zone della mappa;
		}
		if(proiettile.y<40 && proiettile.y>=15){
			wattron(game,COLOR_PAIR(11));
			mvwaddch(game,proiettile.y,proiettile.x,'-');
			wattroff(game,COLOR_PAIR(11));
		}
		if(proiettile.y<45 && proiettile.y>=40){
			wattron(game,COLOR_PAIR(11));
			mvwaddch(game,proiettile.y,proiettile.x,'-');
			wattroff(game,COLOR_PAIR(11));
		}}	
	
}

void draw_granate(WINDOW* game, Granata granate[2]){
	for(int i=0;i<2;i++){
		if(granate[i].alive){
					//se la granata è vivo;
			if(granate[i].y<15 && granate[i].y>=10){
				wattron(game,COLOR_PAIR(11));
				mvwaddch(game,granate[i].y,granate[i].x,'X');
				wattroff(game,COLOR_PAIR(11));			//impostiamo i colori come le zone della mappa;
				}
			if(granate[i].y<40 && granate[i].y>=15){
				wattron(game,COLOR_PAIR(11));
				mvwaddch(game,granate[i].y,granate[i].x,'X');
				wattroff(game,COLOR_PAIR(11));
				}	
			if(granate[i].y<45 && granate[i].y>=40){
				wattron(game,COLOR_PAIR(11));
				mvwaddch(game,granate[i].y,granate[i].x,'X');
				wattroff(game,COLOR_PAIR(11));
				}
}}
}

void draw_frog(WINDOW *game, Rana rana) {
	//attivare colore rana;
    
    wattron(game,COLOR_PAIR(1));
    for (int i = 0; i < ALTEZZARANA; i++) {
        for (int j = 0; j < LARGHEZZARANA; j++) {			
            mvwaddch(game, rana.y + i, rana.x -1 + j, frog_sprite[i][j]);
        }
    }
    wattroff(game,COLOR_PAIR(1));
}







void drawCoccodrilli(WINDOW *game, Coccodrillo *coccodrilli) {
    int h;
    wattron(game, COLOR_PAIR(10));
    for (int i = 0; i < MAX_CROCODILES; i++) {
        if (coccodrilli[i].alive && coccodrilli[i].dir==1) { 	   //direzione a destra
        
            
         
	    
            h=0; 
            
            
            
            for (int j = 0; j < LARGHEZZACOCCODRILLO-2; j++) {
       		   if(coccodrilli[i].x-3+j>0 && coccodrilli[i].x-3+j<79){			
       		     	mvwaddch(game, coccodrilli[i].y + h, coccodrilli[i].x -3 + j, coc_sprite[0][h][j]);   //prima riga , colonna h, al carattere n j;
     	    		}
   	    }
            h=1;
            
            for (int j = 0; j < LARGHEZZACOCCODRILLO; j++) {
       		   if(coccodrilli[i].x-4+j>0 && coccodrilli[i].x-4+j<79){			
       		     	mvwaddch(game, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coc_sprite[0][h][j]);
     	    	   }
   	    }
            
       
            
        }
        else if(coccodrilli[i].alive && coccodrilli[i].dir==-1){   //direzione verso sinistra
       	    
       	    h=0; 
            
            
            
            for (int j = 0; j < LARGHEZZACOCCODRILLO-2; j++) {
       		   if(coccodrilli[i].x-3+j>0 && coccodrilli[i].x-3+j<79){			
       		     	mvwaddch(game, coccodrilli[i].y + h, coccodrilli[i].x -3 + j, coc_sprite[1][h][j]);   //prima riga , colonna h, al carattere n j;
     	    		}
   	    }
            h=1;
            
            for (int j = 0; j < LARGHEZZACOCCODRILLO; j++) {
       		   if(coccodrilli[i].x-4+j>0 && coccodrilli[i].x-4+j<79){			
       		     	mvwaddch(game, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coc_sprite[1][h][j]);
     	    	   }
   	    }
        
    }}
    wattroff(game, COLOR_PAIR(10));
}


void sblocca_fd(int fd){

	int flags = fcntl(fd, F_GETFL);
	if (flags == -1) {
  	  perror("Errore nell'ottenere i flag del fd");
   	  exit(-1);
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
   	 perror("Errore nel settare O_NONBLOCK");
    	 exit(-1);
	}


}
