#include "header.h"
#include <ncurses.h>
#include <stdlib.h> // per exit
#include <fcntl.h>  // per fcntl, F_GETFL

void windowGeneration(WINDOW *game, int maxX, int maxY, Game_struct* game_struct){
    
    box(game, ACS_VLINE, ACS_HLINE);
    int offsetSumV=0;  
    int offsetSumH=1;  
    
    //Definiamo la mappa di gioco
    char tane[TANE][LARGHEZZA_GIOCO-2]={
        "     ..o..            o                   .          o                o        ",
    	"                 o               O                           .                 ",
    	"     +-------+      +-------+      +-------+      +-------+      +-------+  O  ",		          
    	"     |*******|      |*******|      |*******|      |*******|      |*******|     ",			
    	"  .  |*******|      |*******|   o  |*******|      |*******|      |*******|     ",
    	"     |*******|      |*******|      |*******|      |*******|      |*******|     "
    };	
    							 
    char fiume[FIUME][LARGHEZZA_GIOCO-2]={
 	"                                                                               ",
    	"              ~                                                                ",
    	"                         ~                ~          ~                 ~       ",
    	"        ~                     ~                                                ",
    	"                                                        ~          ~           ",
    	"                                    ~                                          ",
    	"                                               ~                               ",
    	"                ~            ~                                                 ",
    	"                                                        ~                      ",
    	"                                             ~                                 ",	
    	"         ~                      ~                                              ",
    	"                                                                   ~           ",
    	"                                                                               ",
    	"                     ~                    ~          ~                         ",
    	"                                                                               ",
    	"                                                          ~                    ",
    	"             ~            ~          ~            ~                     ~      ",
    	"                                                                               ",
    	"                                                                ~              ",
    	"                                ~           ~                                  ",
    	"         ~                                                                     ",
    	"                                                                               ",
    	"              ~            ~                           ~                       ",
    	"                                      ~                       ~     ~          ",
    	"                                                                               "
    };     
    
    char prato[PRATO][LARGHEZZA_GIOCO-2]={   
    
    	"      .                                                                        ",
    	"                   /          |                      /          .     \\        ",
    	"         \\                                       .                             ",
    	"   .             .                 .                       \\          .        ",
    	"                          /                     /                              "
    };
    
    offsetSumV += 4; 
    
    //Sezione Tane
    
    //Printiamo zona intorno alle tane
    wattron(game,COLOR_PAIR(3));
    for (int i=0; i<TANE;i++) {
    	for (int j=0; j<maxX-2;j++) {				
    	    if (tane[i][j]==' ' || tane[i][j]=='o' || tane[i][j]=='O' || tane[i][j]=='.') {  		
    	        mvwaddch(game,i+offsetSumV,j+offsetSumH,tane[i][j]);
            } 		
    	      
        }
    }
    wattroff(game,COLOR_PAIR(3));
    
    //Printiamo le tane
    
    for(int i=0; i<5;i++){
	if(game_struct->tane[i]==0){
            wattron(game,COLOR_PAIR(4));
	    for (int v=2; v<TANE;v++) {
	    	for (int h=5+(8+7)*i; h<5+(8+7)*i+9 ;h++) {							
	    	    if (tane[v][h]=='|' || tane[v][h]=='+' || tane[v][h]=='-') {				
	    	     	 mvwaddch(game,v+offsetSumV,h+offsetSumH,tane[v][h]);
	            }
	       	}
 	    }
	    wattroff(game,COLOR_PAIR(4));
	    wattron(game,COLOR_PAIR(5));
	    for (int v=2; v<TANE;v++) {
	    	for (int h=5+(8+7)*i; h<5+(8+7)*i+9 ;h++) {
	    	    if (tane[v][h]=='*') {									
	    	        mvwaddch(game,v+offsetSumV,h+offsetSumH,tane[v][h]);
	   	    }
	   	}
	    }
	    wattroff(game,COLOR_PAIR(5));
    
	} else {
	    
	    wattron(game,COLOR_PAIR(3));
	    for (int v=2; v<TANE;v++) {
		for (int h=5+(8+7)*i; h<5+(8+7)*i+9 ;h++) {								
	     	    mvwaddch(game,v+offsetSumV,h+offsetSumH,' ');
		}
	    }
	    wattroff(game,COLOR_PAIR(3));    
	}
    }
    
    offsetSumV += TANE;  
    
    //Sezione sponda superiore
    
    wattron(game, COLOR_PAIR(6)); 
    for (int i = 0; i < SPONDA_SUPERIORE; i++) {
        mvwhline(game, offsetSumV + i, offsetSumH, ' ', maxX - 2);
    }
    wattroff(game, COLOR_PAIR(6));
   
    offsetSumV += SPONDA_SUPERIORE;  


    //Sezione Fiume
    
    wattron(game,COLOR_PAIR(2));
    for (int i=0; i<FIUME;i++) {
        for (int j=0; j<maxX-2;j++) {										
    	    mvwaddch(game,i+offsetSumV,j+offsetSumH,fiume[i][j]);
        }
    }
    wattroff(game,COLOR_PAIR(2));
    
    offsetSumV += FIUME;  
    
    
    //Sezione Prato
    
    wattron(game,COLOR_PAIR(8));
    for (int i=0; i<PRATO;i++) {
    	for (int j=0; j<maxX-2;j++) {											
    	    mvwaddch(game,i+offsetSumV,j+offsetSumH,prato[i][j]);
        }
    }
    wattroff(game,COLOR_PAIR(8));
    
    //print zona esterna 
    wattron(game,COLOR_PAIR(14));
    mvwhline(game, 1,1, ' ', maxX - 2);
    mvwhline(game, 3,1, ' ', maxX - 2);
    mvwhline(game, 45,1, ' ', maxX - 2);  
    mvwhline(game, 47,1, ' ', maxX - 2);  
    mvwhline(game, 2,15, ' ', 35);
    mvwhline(game, 46,77, ' ', 3);
    mvwhline(game, 46,12, ' ', 3);
    mvwhline(game, 2,76, ' ', 4);
   
    mvwaddch(game,46,78,' ');
    mvwaddch(game,46,77,' ');
    mvwaddch(game,46,78,' ');
    mvwaddch(game,46,1,' ');
    mvwaddch(game,2,1,' ');
    
    wattroff(game,COLOR_PAIR(14));
    
    //print score
    wattron(game, COLOR_PAIR(15));
    mvwprintw(game, 2, 2, "Punteggio: %d ",game_struct->score);
    wattroff(game, COLOR_PAIR(15));
    
    //print vite
    wattron(game, COLOR_PAIR(15));
    mvwprintw(game, 2, 50, "Vite:");
    mvwhline(game, 2,55, ' ', 21);
    for(int i=0; i<game_struct->vite;i++){
    	mvwprintw(game, 2, 55+i*2, "❤️  ");
       
    } 
	    	
    wattroff(game, COLOR_PAIR(15));
    
    
    //print tempo
    wattron(game, COLOR_PAIR(15));
    mvwhline(game, 46,2, ' ', 10);
    mvwprintw(game, 46, 2, "Tempo: %d ",game_struct->tempo);
    wattroff(game, COLOR_PAIR(15));


    
};

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
        init_color(20,0,400,0);    //verde chiaro
        
    	init_pair(1,COLOR_BLACK,20);               //rana
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
void draw_proiettile(WINDOW* game, Proiettile proiettili[]){
    
    for(int i=0; i<15;i++){
	    if (proiettili[i].alive) {  //mostriamo il proiettile solo se vivo		
		wattron(game,COLOR_PAIR(11));
		
		    mvwaddch(game,proiettili[i].y,proiettili[i].x,'*');		
		
		wattroff(game,COLOR_PAIR(11));
		}
    }		
}




void draw_granate(WINDOW* game, Granata granate[2]){
	
	
    for (int i=0;i<2;i++) {
	if (granate[i].alive && granate[i].x!=-1) { //mostriamo le granate solo se vive
				
	    if (granate[i].y<15 && granate[i].y>=10) {  //granata sul marciapiede
		wattron(game,COLOR_PAIR(6));
		mvwaddch(game,granate[i].y,granate[i].x,'O');
		wattroff(game,COLOR_PAIR(6));			
	    }
	    if (granate[i].y<40 && granate[i].y>=15) {  //granata sul fiume
		wattron(game,COLOR_PAIR(13));
		mvwaddch(game,granate[i].y,granate[i].x,'O');
		wattroff(game,COLOR_PAIR(13));
	    }	
	    if (granate[i].y<45 && granate[i].y>=40) {  //granata sul prato
		wattron(game,COLOR_PAIR(12));
		mvwaddch(game,granate[i].y,granate[i].x,'O');
		wattroff(game,COLOR_PAIR(12));
            }
	}
    }
}

//funzione che disegna la rana
void draw_frog(WINDOW *game, Rana* rana){
	
    
    wattron(game,COLOR_PAIR(1));
    
    for (int i = 0; i < ALTEZZARANA; i++) {
        for (int j = 0; j < LARGHEZZARANA; j++) {			
            mvwaddch(game, rana->y + i, rana->x -1 + j, frog_sprite[i][j]);
        }
    }
    wattron(game,COLOR_PAIR(16));
    mvwaddch(game, rana->y, rana->x -1, frog_sprite[0][0]);
    mvwaddch(game, rana->y, rana->x + 1, frog_sprite[0][2]);
    wattroff(game,COLOR_PAIR(16));
}






//funzione che disegna i coccodrilli
void drawCoccodrilli(WINDOW *game, Coccodrillo *coccodrilli){
    int h;  //variabile utilizzata per tenere conto di quale parte del coccodrillo stiamo printando (inferiore o superiore)
    wattron(game, COLOR_PAIR(10));
    
    for (int i = 0; i < MAX_CROCODILES; i++) {
        if (coccodrilli[i].alive && coccodrilli[i].dir==1 && coccodrilli[i].wait!=1) { //coccodrillo vivo, non in wait e direzionato verso destra
          
            h=0; 
	    
	    //printiamo la parte superiore del coccodrillo
            for (int j = 0; j < LARGHEZZACOCCODRILLO-2; j++) {
	        if (coccodrilli[i].x-3+j>0 && coccodrilli[i].x-3+j<LARGHEZZA_GIOCO-1) {  //condizioni necessarie per printare solo le parti del coccodrillo all'interno della mappa	
	            
	                mvwaddch(game, coccodrilli[i].y, coccodrilli[i].x -3 + j, coc_sprite[0][h][j]);
	        }
	       	      
    	    }
   	    
            h=1;
            
            //printiamo la parte inferiore del coccodrillo
            for (int j = 0; j < LARGHEZZACOCCODRILLO; j++) {
       	        if (coccodrilli[i].x-4+j>0 && coccodrilli[i].x-4+j<LARGHEZZA_GIOCO-1) {		
       	            if(coc_sprite[0][h][j]=='0'){
	                wattron(game, COLOR_PAIR(16));
	                mvwaddch(game, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coc_sprite[0][h][j]);
	                wattroff(game, COLOR_PAIR(16));
	                
	                wattron(game, COLOR_PAIR(10));
	            
	            } else {	
       		     	mvwaddch(game, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coc_sprite[0][h][j]);
     	    	    }
   	        }
           
            }
        } else if (coccodrilli[i].alive && coccodrilli[i].dir==-1 &&  coccodrilli[i].wait!=1) {   //coccodrillo vivo, non in wait e direzionato verso sinistra
       	    
       	    h=0; 
            
            
            //printiamo la parte superiore del coccodrillo
            for (int j = 0; j < LARGHEZZACOCCODRILLO-2; j++) {
	        if (coccodrilli[i].x-3+j>0 && coccodrilli[i].x-3+j<LARGHEZZA_GIOCO-1) {			
	     	    mvwaddch(game, coccodrilli[i].y + h, coccodrilli[i].x -3 + j, coc_sprite[1][h][j]);   //prima riga , colonna h, al carattere n j;
    		}
   	    }
            h=1;
            
            //printiamo la parte inferiore del coccodrillo
            for (int j = 0; j < LARGHEZZACOCCODRILLO; j++) {
       		   if (coccodrilli[i].x-4+j>0 && coccodrilli[i].x-4+j<LARGHEZZA_GIOCO-1) {			
       		     	
       		     	if(coc_sprite[1][h][j]=='0'){
			        wattron(game, COLOR_PAIR(16));
			        mvwaddch(game, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coc_sprite[1][h][j]);
			        wattroff(game, COLOR_PAIR(16));
			        
			        wattron(game, COLOR_PAIR(10));
	            
	                } else {
       		     	    mvwaddch(game, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coc_sprite[1][h][j]);
     	    	        }
     	    	   }
     	          	    
   	    }
   	    
   	    
        }
    }
    wattroff(game, COLOR_PAIR(10));
    
}





