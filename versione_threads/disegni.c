#include "header.h"


//funzione che crea i colori utilizzati durante il finestra_gioco
void creazione_colori(){
    if (has_colors()) { //verifica il supporto ai colori da parte del terminale
        pthread_mutex_lock(&semaforo_disegno);
        start_color();
        init_color(8,0,300,0);       //verde foresta
   	init_color(9,0,123,184);     //blu scuro		
  	init_color(10,95,95,95);     //grigio scuro
  	init_color(11,192,192,192);  //grigio chiaro
   	init_color(12,101,67,33);    //marrone
   	init_color(13,230,118,84);   //marrone pastello
   	init_color(14,50,300,50);    //1o verde chiaro
        init_color(15,0,500,0);      //2o verde chiaro
        init_color(16, 150,200,0);   //verde scuro
        init_color(17,100,500,700);  //blu chiaro
        init_color(18,800,800,0);    //giallo chiaro
        init_color(19,350,350,350);  //grigio 
        init_color(20,0,400,0);      //3o verde chiaro
        
    	init_pair(1,COLOR_BLACK,20);               //rana
   	init_pair(2, COLOR_BLUE, 17);	           //fiume
   	init_pair(3, 19, COLOR_YELLOW);            //zona in torno alle tane
   	init_pair(4, 10, 11);                      //recinzione delle tane
    	init_pair(5, 13, 13); 		           //tana vera e propria
   	init_pair(6, COLOR_YELLOW, 16);            //marciapiede
   	init_pair(7, COLOR_WHITE, COLOR_MAGENTA);  //barra del tempo	
   	init_pair(8, 8, 15);   	                   //prato;
        init_pair(9,COLOR_BLACK,COLOR_WHITE);	   //selezione menù
        init_pair(10,COLOR_BLACK,14);              //coccodrillo
        init_pair(11,COLOR_RED,17);	           //proiettile
        init_pair(12,COLOR_RED,15);                //granata nel prato
        init_pair(13,COLOR_RED,17);	           //granata fiume
        init_pair(14,COLOR_RED,COLOR_RED); 	   //zona esterna al finestra_gioco
        init_pair(15,COLOR_WHITE,14);		   //colore punteggio-vite-tempo
        init_pair(16,COLOR_BLACK,18);              //occhi rana e coccodrilli    
        init_pair(17,COLOR_RED,16);                //granata nel marciapiede 
        pthread_mutex_unlock(&semaforo_disegno);
             
    } else {  //il terminale non supporta i colori
    	printw("Il terminale non permette la visualizzazione dei colori\n"); 
    	mvprintw(LINES-1/2,COLS/2,"Premi un tasto per uscire...");refresh();
    	getch();
    	exit(1);
    }
	
	
}




//funzione per disegnare il proiettile nella mappa
void disegna_proiettile(WINDOW* finestra_gioco, Proiettile proiettili[]){
    
    for (int i=0; i<NUMERO_PROIETTILI;i++) {
        if (proiettili[i].vivo && proiettili[i].x!=-2) {  //mostriamo il proiettile solo se vivo e non in posizione di default		
	    wattron(finestra_gioco,COLOR_PAIR(11));
		
   	    mvwaddch(finestra_gioco,proiettili[i].y,proiettili[i].x,'*');		
		
	    wattroff(finestra_gioco,COLOR_PAIR(11));
	}
    }
  		
}




void disegna_granate(WINDOW* finestra_gioco, Granata granate[2]){
	
   
    for (int i=0;i<2;i++) {
	if (granate[i].vivo && granate[i].x!=-1) { //mostriamo le granate solo se vive e non in posizione di default
				
	    if (granate[i].y<15 && granate[i].y>=10) {  //granata sul marciapiede
		wattron(finestra_gioco,COLOR_PAIR(6));
		mvwaddch(finestra_gioco,granate[i].y,granate[i].x,'0');
		wattroff(finestra_gioco,COLOR_PAIR(6));			
	    }
	    if (granate[i].y<40 && granate[i].y>=15) {  //granata sul fiume
		wattron(finestra_gioco,COLOR_PAIR(13));
		mvwaddch(finestra_gioco,granate[i].y,granate[i].x,'0');
		wattroff(finestra_gioco,COLOR_PAIR(13));
	    }	
	    if (granate[i].y<45 && granate[i].y>=40) {  //granata sul prato
		wattron(finestra_gioco,COLOR_PAIR(12));
		mvwaddch(finestra_gioco,granate[i].y,granate[i].x,'0');
		wattroff(finestra_gioco,COLOR_PAIR(12));
            }
	}
    }
    
}

//funzione che disegna la rana
void disegna_rana(WINDOW *finestra_gioco, Rana* rana){
    
    wattron(finestra_gioco,COLOR_PAIR(1));
    
    //mostriamo la rana
    for (int i = 0; i < ALTEZZA_RANA; i++) {
        for (int j = 0; j < LARGHEZZARANA; j++) {			
            mvwaddch(finestra_gioco, rana->y + i, rana->x -1 + j, rana_sprite[i][j]);
        }
    }
    //coloriamo gli occhi della rana
    wattron(finestra_gioco,COLOR_PAIR(16));
    mvwaddch(finestra_gioco, rana->y, rana->x -1, rana_sprite[0][0]);
    mvwaddch(finestra_gioco, rana->y, rana->x + 1, rana_sprite[0][2]);
    wattroff(finestra_gioco,COLOR_PAIR(16));
   
}




//funzione che disegna i coccodrilli
void disegna_coccodrilli(WINDOW *finestra_gioco, Coccodrillo *coccodrilli){


    
    int h;  //variabile utilizzata per tenere conto di quale parte del coccodrillo stiamo mostrando (inferiore o superiore)
    wattron(finestra_gioco, COLOR_PAIR(10));
    
    for (int i = 0; i < NUMERO_COCCODRILLI; i++) {
        if (coccodrilli[i].vivo && coccodrilli[i].dir==1 && coccodrilli[i].attesa!=1) { //coccodrillo vivo, non in attesa e direzionato verso destra
          
            h=0; 
	    
	    //mostriamo la parte superiore del coccodrillo
            for (int j = 0; j < LARGHEZZA_COCCODRILLO-2; j++) {
	        if (coccodrilli[i].x-3+j>0 && coccodrilli[i].x-3+j<LARGHEZZA_GIOCO-1) {  //condizioni necessarie per printare solo le parti del coccodrillo all'interno della mappa	
	            
	                mvwaddch(finestra_gioco, coccodrilli[i].y, coccodrilli[i].x -3 + j, coccodrillo_sprite[0][h][j]);
	        }
	       	      
    	    }
   	    
            h=1;
            
            //printiamo la parte inferiore del coccodrillo
            for (int j = 0; j < LARGHEZZA_COCCODRILLO; j++) {
       	        if (coccodrilli[i].x-4+j>0 && coccodrilli[i].x-4+j<LARGHEZZA_GIOCO-1) {		
       	            if (coccodrillo_sprite[0][h][j]=='0') {  //condizione per printare gli occhi del coccodrillo diversamente dal resto
	                wattron(finestra_gioco, COLOR_PAIR(16));
	                mvwaddch(finestra_gioco, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coccodrillo_sprite[0][h][j]);
	                wattroff(finestra_gioco, COLOR_PAIR(16));
	                wattron(finestra_gioco, COLOR_PAIR(10));
	            
	            } else {	
       		     	mvwaddch(finestra_gioco, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coccodrillo_sprite[0][h][j]);
     	    	    }
   	        }
           
            }
        } else if (coccodrilli[i].vivo && coccodrilli[i].dir==-1 && coccodrilli[i].attesa!=1) {   //coccodrillo vivo, non in attesa e direzionato verso sinistra
       	    
       	    h=0; 
            
            
            //printiamo la parte superiore del coccodrillo
            for (int j = 0; j < LARGHEZZA_COCCODRILLO-2; j++) {
	        if (coccodrilli[i].x-3+j>0 && coccodrilli[i].x-3+j<LARGHEZZA_GIOCO-1) {			
	     	    mvwaddch(finestra_gioco, coccodrilli[i].y + h, coccodrilli[i].x -3 + j, coccodrillo_sprite[1][h][j]);   //prima riga , colonna h, al carattere n j;
    		}
   	    }
            h=1;
            
            //printiamo la parte inferiore del coccodrillo
            for (int j = 0; j < LARGHEZZA_COCCODRILLO; j++) {
       	        if (coccodrilli[i].x-4+j>0 && coccodrilli[i].x-4+j<LARGHEZZA_GIOCO-1) {			
       		     	
       		    if (coccodrillo_sprite[1][h][j]=='0') {
		        wattron(finestra_gioco, COLOR_PAIR(16));
			mvwaddch(finestra_gioco, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coccodrillo_sprite[1][h][j]);
			wattroff(finestra_gioco, COLOR_PAIR(16));  
			wattron(finestra_gioco, COLOR_PAIR(10));
	            
	            } else {
       		        mvwaddch(finestra_gioco, coccodrilli[i].y + h, coccodrilli[i].x -4 + j, coccodrillo_sprite[1][h][j]);
     	    	    }
     	        }     	    
   	    } 
        }
    }
    wattroff(finestra_gioco, COLOR_PAIR(10));
    
}


void generatore_finestra(WINDOW *finestra_gioco, int maxX, int maxY, Statistiche * statistiche_gioco){
    
   
    box(finestra_gioco, ACS_VLINE, ACS_HLINE);
    int spostamento_verticale=0;  
    int spostamento_orizzontale=1;  
    
    //DEFINIAMO LA MAPPA DI GIOCO
    
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
    
    spostamento_verticale += 4; 
    
    //SEZIONE TANE
    
    //Mostriamo zona intorno alle tane
    wattron(finestra_gioco,COLOR_PAIR(3));
    for (int i=0; i<TANE;i++) {
    	for (int j=0; j<maxX-2;j++) {				
    	    if (tane[i][j]==' ' || tane[i][j]=='o' || tane[i][j]=='O' || tane[i][j]=='.') {  		
    	        mvwaddch(finestra_gioco,i+spostamento_verticale,j+spostamento_orizzontale,tane[i][j]);
            } 		
    	      
        }
    }
    wattroff(finestra_gioco,COLOR_PAIR(3));
    
    //Mostriamo le tane vere e proprie
    
    for(int i=0; i<5;i++){
	if(statistiche_gioco->tane[i]==0){  //per ogni tana verifichiamo se è aperta altrimenti la mostriamo chiusa 
            wattron(finestra_gioco,COLOR_PAIR(4));
	    for (int v=2; v<TANE;v++) {
	    	for (int h=5+(8+7)*i; h<5+15*i+9 ;h++) {  //la formula all'interno è ricavata per non riscrivere lo stesso codice per mostrare ogni tana							
	    	    if (tane[v][h]=='|' || tane[v][h]=='+' || tane[v][h]=='-') {				
	    	        mvwaddch(finestra_gioco,v+spostamento_verticale,h+spostamento_orizzontale,tane[v][h]);
	            }
	       	}
 	    }
	    wattroff(finestra_gioco,COLOR_PAIR(4));
	    wattron(finestra_gioco,COLOR_PAIR(5));
	    for (int v=2; v<TANE;v++) {
	    	for (int h=5+15*i; h<5+15*i+9 ;h++) {
	    	    if (tane[v][h]=='*') {									
	    	        mvwaddch(finestra_gioco,v+spostamento_verticale,h+spostamento_orizzontale,tane[v][h]);
	   	    }
	   	}
	    }
	    wattroff(finestra_gioco,COLOR_PAIR(5));
    
	} else {  
	    
	    wattron(finestra_gioco,COLOR_PAIR(3));
	    for (int v=2; v<TANE;v++) {
		for (int h=5+15*i; h<5+15*i+9 ;h++) {								
	     	    mvwaddch(finestra_gioco,v+spostamento_verticale,h+spostamento_orizzontale,' ');
		}
	    }
	    wattroff(finestra_gioco,COLOR_PAIR(3));    
	}
    }
    
    spostamento_verticale += TANE;  
    
    //SEZIONE SPONDA SUPERIORE
    
    wattron(finestra_gioco, COLOR_PAIR(6)); 
    for (int i = 0; i < SPONDA_SUPERIORE; i++) {
        mvwhline(finestra_gioco, spostamento_verticale + i, spostamento_orizzontale, ' ', maxX - 2);
    }
    wattroff(finestra_gioco, COLOR_PAIR(6));
   
    spostamento_verticale += SPONDA_SUPERIORE;  


    //SEZIONE FIUME
    
    wattron(finestra_gioco,COLOR_PAIR(2));
    for (int i=0; i<FIUME;i++) {
        for (int j=0; j<maxX-2;j++) {										
    	    mvwaddch(finestra_gioco,i+spostamento_verticale,j+spostamento_orizzontale,fiume[i][j]);
        }
    }
    wattroff(finestra_gioco,COLOR_PAIR(2));
    
    spostamento_verticale += FIUME;  
    
    
    //SEZIONE PRATO
    
    wattron(finestra_gioco,COLOR_PAIR(8));
    for (int i=0; i<PRATO;i++) {
    	for (int j=0; j<maxX-2;j++) {											
    	    mvwaddch(finestra_gioco,i+spostamento_verticale,j+spostamento_orizzontale,prato[i][j]);
        }
    }
    wattroff(finestra_gioco,COLOR_PAIR(8));
    
    //ZONA ESTERNA ALLA MAPPA DI GIOCO (vera e propria)
    
    //mostriamo zona di riempimento della schermata
    wattron(finestra_gioco,COLOR_PAIR(14));
    mvwhline(finestra_gioco, 1,1, ' ', maxX - 2);
    mvwhline(finestra_gioco, 3,1, ' ', maxX - 2);
    mvwhline(finestra_gioco, 45,1, ' ', maxX - 2);  
    mvwhline(finestra_gioco, 47,1, ' ', maxX - 2);  
    mvwhline(finestra_gioco, 2,15, ' ', 35);
    mvwhline(finestra_gioco, 46,77, ' ', 3);
    mvwhline(finestra_gioco, 46,12, ' ', 3);
    mvwhline(finestra_gioco, 2,76, ' ', 4);
   
    mvwaddch(finestra_gioco,46,78,' ');
    mvwaddch(finestra_gioco,46,77,' ');
    mvwaddch(finestra_gioco,46,78,' ');
    mvwaddch(finestra_gioco,46,1,' ');
    mvwaddch(finestra_gioco,2,1,' ');
    
    wattroff(finestra_gioco,COLOR_PAIR(14));
    
    //mostriamo il punteggio
    wattron(finestra_gioco, COLOR_PAIR(15));
    mvwprintw(finestra_gioco, 2, 2, "Punteggio: %d ",statistiche_gioco->punteggio);
    wattroff(finestra_gioco, COLOR_PAIR(15));
    
    //mostriamo le vite
    wattron(finestra_gioco, COLOR_PAIR(15));
    mvwprintw(finestra_gioco, 2, 50, "Vite:");
    mvwhline(finestra_gioco, 2,55, ' ', 21);
    
    for (int i=0; i<statistiche_gioco->vite;i++) {  //per ogni vita rimanente mostriamo un cuore
    	mvwprintw(finestra_gioco, 2, 55+i*2, "❤️  ");
       
    } 
	    	
    wattroff(finestra_gioco, COLOR_PAIR(15));
    
    
    //mostriamo il tempo
    wattron(finestra_gioco, COLOR_PAIR(15));
    mvwhline(finestra_gioco, 46,2, ' ', 10);
    mvwprintw(finestra_gioco, 46, 2, "Tempo: %d ",statistiche_gioco->tempo);
    wattroff(finestra_gioco, COLOR_PAIR(15));
 
     
};
