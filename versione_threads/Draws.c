#include "Draws.h"
#include "main.h"

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




void gestore_grafica(WINDOW* finestra_gioco,int velocità_proiettili, Statistiche * statistiche_gioco, Thread_id thread_id[]){			
	
    //Variabili per gestione proiettile
    struct timeval inizio, fine; 
    gettimeofday(&fine,NULL);		
    gettimeofday(&inizio,NULL);
    float numero_randomico=5;
	 
    Temp temp={-1,0,0,0};  //utilizzato per leggere dalla pipe1
    
    //Inizializzazione degli elementi di gioco
    Coccodrillo coccodrilli[NUMERO_COCCODRILLI];
    for (int i=0; i<NUMERO_COCCODRILLI;i++) {
        coccodrilli[i].id=i;        //id coccodrillo
	coccodrilli[i].x=-1;	    //posizione x coccodrillo
	coccodrilli[i].y=-1;        //posizione y coccodrillo
	coccodrilli[i].dir=0;       //direzione coccodrillo
	coccodrilli[i].vivo=0;      //stato del coccoddrillo
	coccodrilli[i].attesa=-1;}  //variabile per stato di attesa

    Granata granate[2];
    for (int i=0; i<2;i++) {
	granate[i].id=i;
	granate[i].x=-1;
	granate[i].y=-1;
	granate[i].vivo=0;}
		
    Rana rana;	
    rana.id=ID_RANA;
    rana.x=40; 
    rana.y=43;

    Proiettile proiettili[15];
    for (int i=0;i<NUMERO_PROIETTILI;i++) {
        proiettili[i].id=ID_PROIETTILE+i;
        proiettili[i].x=-2;
        proiettili[i].y=-1;
        proiettili[i].vivo=0;
    }
    
   
    int tempo=statistiche_gioco->tempo;  
    int coccodrillo_scelto;  //variabile utilizzata per indicare su quale coccodrillo posa la rana
    int distanze_coccodrilli[8];  //array utilizzato per tenere conto della distanza da mantenere tra i coccodrilli per ogni flusso (evita che si sovrappongano quando spawnano)
    
    for (int i=0; i<8;i++) {
	distanze_coccodrilli[i]=numero_random(13,16);  //impostiamo le distanze iniziali da mantenere tra i coccodrilli, per ogni flusso del fiume
    }
   
    int giocare=1;  //variabile utilizzata per terminare la manche quando si sono verificate le giuste condizioni
    
    int key;
    int sval;
    while (giocare) {
    	
	giocare=1;  //se durante il ciclo viene impostata a 0 allora si esce dal gioco
	
	
	//funzioni di gestione grafica del gioco
	pthread_mutex_lock(&semaforo_disegno);
	werase(finestra_gioco);
	
	generatore_finestra(finestra_gioco,COLS,LINES,statistiche_gioco);
        disegna_coccodrilli(finestra_gioco,coccodrilli);
        disegna_granate(finestra_gioco,granate);
        disegna_proiettile(finestra_gioco,proiettili);
        disegna_rana(finestra_gioco,&rana);
        barra_tempo(finestra_gioco,statistiche_gioco,tempo); //mostra la barra dinamica del tempo 
     
        
     
     
        wrefresh(finestra_gioco);
        pthread_mutex_unlock(&semaforo_disegno);
        
	
        
	temp=lettura_buffer();
	
	
	
	//se l'ID è della rana						
	if (temp.id==ID_RANA) {
	    if (rana_in_finestra(&rana,&temp)) {  //controllo per evitare che la rana fuoriesca dalla finestra di gioco
	        rana.x+=temp.x;
	        rana.y+=temp.y;	
	
		if (rana.y<10) {  //controllo se la rana si trova nella zona delle tane			
		    if (rana_su_tana(&rana,statistiche_gioco)) {  //controllo se la rana si trova su una delle tane        
		        statistiche_gioco->punteggio+=15;
			statistiche_gioco->punteggio+=(int)(15*(float)statistiche_gioco->tempo/100);
			giocare=0;						
		    } else {  //altrimenti la rana si trova nella zona circostante alle tane ( e muore)
		        statistiche_gioco->vite--;
			statistiche_gioco->punteggio-=10; 
		        giocare=0;
		    }
		}
		
		if(rana.y>=15 && rana.y<40){
		 
		    if(rana_su_coccodrillo(&rana,coccodrilli)==-1){ 
			statistiche_gioco->vite--;
		        statistiche_gioco->punteggio-=10;
			return;
					
		    }
		}
	    }  
        }
	
	//se l'id è del processo tempo
	if (temp.id==ID_TIME) {	
	    statistiche_gioco->tempo-=1;
	    if (statistiche_gioco->tempo==0) {  //controllo per verificare se il tempo è terminato
	        statistiche_gioco->vite--;
		statistiche_gioco->punteggio-=20;
		giocare=0;
	    }
	}
	
	
	//se l'ID è di un coccodrillo
	if (temp.id<NUMERO_COCCODRILLI && temp.id>=0) {
	        
		coccodrillo_scelto=rana_su_coccodrillo(&rana,coccodrilli);  //controllo di verifica che la rana sia già su un coccodrillo (qualsiasi) prima dello spostamento
		coccodrilli[temp.id].x=temp.x;
		coccodrilli[temp.id].y=temp.y;
	        coccodrilli[temp.id].dir=temp.info;  

                //controlli per lo spostamento della rana
		giocare=movimento_rana_su_coccodrillo(temp.id, coccodrillo_scelto,coccodrilli, &rana,statistiche_gioco, giocare);
                //controllo sullo stato del coccodrillo
		controllo_stato_coccodrillo(temp.id,coccodrilli);
		
		
		//gestione dello stato di attesa dei coccodrilli
		attesa_coccodrilli(temp.id, coccodrilli,distanze_coccodrilli);
		riattivazione_coccodrilli(coccodrilli, distanze_coccodrilli);
		
	}
        
        
        //se l'id è di un proiettile
	if (temp.id>=ID_PROIETTILE && temp.id<ID_PROIETTILE+NUMERO_PROIETTILI) {	
	    proiettili[temp.id-ID_PROIETTILE].x=temp.x;
	    proiettili[temp.id-ID_PROIETTILE].y=temp.y;
	    
	}
	
	//se l'id è delle granate
	if (temp.id>=ID_GRANATE && temp.id<=ID_GRANATE+2) {
	    if (temp.id==ID_GRANATE+2) {  //id relativo alla richiesta di sparo delle granate
	        if (granate[0].vivo==0 && granate[1].vivo==0) {  //devono essere entrambe morte;		
	            spara_granata(rana.x,rana.y,velocità_proiettili,thread_id);
	           
	            granate[0].vivo=1;
                    granate[1].vivo=1;
          
                    
	            
	        }
	    } else {  
	        granate[temp.id-ID_GRANATE].x=temp.x;	
		granate[temp.id-ID_GRANATE].y=temp.y;
			   
	    }	  
	}  

    }
}



//funzione che gestisce la creazione del processo granata
void spara_granata(int inizioX, int inizioY,int velocità_proiettile , Thread_id thread_id[]){

    Parametri_granata* parametri_granata = malloc(sizeof(Parametri_granata));
   
    parametri_granata->x=inizioX;
    parametri_granata->y=inizioY;
    parametri_granata->velocità_granata=velocità_proiettile;
   
    pthread_create(&thread_id[ID_GRANATE].id, NULL, &funzione_granata, (void*)parametri_granata);
    pthread_detach(thread_id[ID_GRANATE].id);
    thread_id[ID_GRANATE].valido=1;
}


//funzione che mostra la barra del tempo rimanente
void barra_tempo(WINDOW* finestra_gioco,Statistiche * statistiche_gioco, int tempo){
    
    wattron(finestra_gioco, COLOR_PAIR(7));
    mvwhline(finestra_gioco,46,15, ' ', (int)(62*((float)statistiche_gioco->tempo/tempo)));
    wattroff(finestra_gioco, COLOR_PAIR(7));
   
}






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
