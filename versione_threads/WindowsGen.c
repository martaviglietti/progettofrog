#include "header.h"

void windowGeneration(WINDOW *game, int maxX, int maxY, const Game_struct* game_struct){
    
    box(game, ACS_VLINE, ACS_HLINE);
    int offsetSumV=0;  //variabile che tiene conto della posizione di stampa
    int offsetSumH=1;  //variabile per ignorare il bordo laterale
    
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
    
};
