#include "header.h"
void windowGeneration(WINDOW *game, int maxX, int maxY, Game_struct* game_struct){
    
    box(game, ACS_VLINE, ACS_HLINE);
    
    

    int offsetSumV=2;  //posizione di stampa del tempo;
    int offsetSumH=1;  //per ignorare il bordo laterale;
	
    char tane[TANE][78]={
    
    				   "                                                                              ",
    				   "                                                                              ",
    				   "        +----+        +----+        +----+        +----+        +----+        ",		          
    				   "        |....|        |....|        |....|        |....|        |....|        ",			
    				   "        |....|        |....|        |....|        |....|        |....|        ",
    				   "        |....|        |....|        |....|        |....|        |....|        "};						
    				 
    




    
    
    char fiume[FIUME][78]=
 				       {"                                                                              ",
    				        "                                                                              ",
    				        "                                                                              ",
    				        "                                                                              ",
    				        "                                                                              ",
    				        "                                                                              ",
    				        "                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",	
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              "};     //25
    
    
    
    
    char prato[PRATO][78]={   
    
    				        "                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              ",
    					"                                                                              "};
    
    
    
    
    
    
    
    
    
    
    offsetSumV += 2;                                               //ci spostiamo da posizione tempo a inizio zona tane;
    
    
    // Sezione Tane
    wattron(game,COLOR_PAIR(3));
    for(int i=0; i<TANE;i++){
    	for(int j=0; j<maxX-2;j++){				//printa zona intorno
    	     if(tane[i][j]==' ' ){				
    	     	 mvwaddch(game,i+offsetSumV,j+offsetSumH,tane[i][j]);
   
        }
    }}
    wattroff(game,COLOR_PAIR(3));
    
    for(int i=0;i<5;i++){			//questo for decide come stampare la zona delle tane;
    	if(game_struct->tane[i]==0){
    		 wattron(game,COLOR_PAIR(4));
   		 for(int v=2; v<TANE;v++){
    			for(int h=8*(i+1)+i*6; h<8*(i+1)+i*6+6;h++){							//printa recinzione
    	  		   if(tane[v][h]=='|' || tane[v][h]=='+' || tane[v][h]=='-'){				
    	     			 mvwaddch(game,v+offsetSumV,h+offsetSumH,tane[v][h]);
   
       		 }}}
  		 wattroff(game,COLOR_PAIR(4));
    
   		 wattron(game,COLOR_PAIR(5));
   		 for(int v=2; v<TANE;v++){
    			for(int h=8*(i+1)+i*6; h<8*(i+1)+i*6+6;h++){							//printa tana vera e propria
    	  		   	if(tane[v][h]=='.'){				
    	     			 	mvwaddch(game,v+offsetSumV,h+offsetSumH,tane[v][h]);
   		
   		 }}
 		 wattroff(game,COLOR_PAIR(5));}}
    
    	else{
    		
    		wattron(game,COLOR_PAIR(3));
  		for(int v=2; v<TANE;v++){
    			for(int h=8*(i+1)+i*6; h<8*(i+1)+i*6+6;h++){							//ricopri zona tana col colore dell area circostante;	
    	     			mvwaddch(game,v+offsetSumV,h+offsetSumH,tane[v][h]);
   		
   		}}
   		wattroff(game,COLOR_PAIR(3));}
    		
   	
    }	
    
   
    
    
    offsetSumV += TANE;  //siamo a 10
    
    // Sezione Sponda superiore
    wattron(game, COLOR_PAIR(6));
    for (int i = 0; i < SPONDA_SUPERIORE; i++) {
        
        mvwhline(game, offsetSumV + i, offsetSumH, ' ', maxX - 2);
        
    }
    wattroff(game, COLOR_PAIR(6));
    
    
    
    
    offsetSumV += SPONDA_SUPERIORE;  //siamo a 15



    // Sezione Fiume
    wattron(game,COLOR_PAIR(2));
    for(int i=0; i<FIUME;i++){
    	for(int j=0; j<maxX-2;j++){	
    											
    	     	 mvwaddch(game,i+offsetSumV,j+offsetSumH,fiume[i][j]);
   
        }
    }
    wattroff(game,COLOR_PAIR(2));
    
    offsetSumV += FIUME;  //siamo a 40
    
    
    // Sezione Prato
    wattron(game,COLOR_PAIR(8));
    for(int i=0; i<PRATO;i++){
    	for(int j=0; j<maxX-2;j++){											
    	     	 mvwaddch(game,i+offsetSumV,j+offsetSumH,prato[i][j]);
   
        }
    }
    wattroff(game,COLOR_PAIR(8));
    
    offsetSumV += PRATO;  //siamo a 45
    
    
    
    
    
};
