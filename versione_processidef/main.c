
int main(){
    printf("\e[8;%d;%dt", 49, 81);  //ridimensioniamo il terminale
    fflush(stdout);
    setlocale(LC_ALL, "");  //abilita UTF-8	
    sleep(1);  //pausa scelta da noi

    //Inizializziamo ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    resizeterm(49, 81);  //avvisiamo ncurses del cambio di dimensioni del terminale
    creazione_colori();  
      
    srand(time(NULL));
    
    int scelta;	        //variabile contenente la scelta dal menù principale
    int difficoltà;     //variabile contenente la difficoltà scelta dal menù delle difficoltà
    int ricomincia=1;  
    
    Parametri parametri_gioco={0,0,0,0,0};
    Statistiche  risultato;  //conterrà i risultati della partita
    WINDOW *finestra_gioco = newwin(LINES, COLS, 0, 0);  //finestra dell'area di gioco
    

    while (true){
    	if (ricomincia) {  //se 'ricomincia' è 0 non torniamo al menu;
            scelta= menu(finestra_gioco,"Menu Principale", opzioni, 3);  
        }	
	
        if (scelta == 0) { 
         
   	    if (ricomincia) {  //se 'ricomincia' è 0 manteniamo la stessa difficoltà
                difficoltà=scelta_difficoltà(finestra_gioco);  //richiesta di quale difficoltà affrontare
            }
        
            impostazioni_gioco(&parametri_gioco, difficoltà);  //impostiamo i parametri di gioco in base alla difficoltà scelta
            
            risultato=Partita(finestra_gioco, parametri_gioco);  //facciamo partire il gioco
            
            if (risultato.gioco==1) {  //Vittoria
                scelta=game_vinto(finestra_gioco,risultato.punteggio); 
            	
            	if (scelta) {	
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con la stessa difficoltà
            	}	
            } else {  //Sconfitta
            	scelta=game_perso(finestra_gioco,risultato.punteggio);
            	if (scelta) {					
            	    ricomincia=1;  //torna al menù principale
            	} else {
            	    ricomincia=0;  //ricomincia il game con la stessa difficoltà
            	} 	
            }
        
        } else if (scelta==1) {  //schermata dei crediti
            crediti(finestra_gioco);
        	
        } else {  //richiesta di uscita dal programma
            wclear(finestra_gioco);
            mvwprintw(finestra_gioco, LINES / 2, COLS / 2 - 5, "Uscita...");
            wrefresh(finestra_gioco);
            sleep(1);  //pausa scelta da noi
            break; 
        }
    }

    delwin(finestra_gioco);
    endwin();
    return 0;
}

void impostazioni_gioco(Parametri* parametri_gioco, int difficoltà ){
    switch (difficoltà) {
        case 0:  //Facile
            parametri_gioco->livello_difficoltà=1;         //livello di difficoltà
            parametri_gioco->vite=10;			   //numero vite
            parametri_gioco->tempo=100;		           //tempo di gioco
            parametri_gioco->velocità_proiettili=40000;    //velocità proiettili
            parametri_gioco->velocità_coccodrilli=120000;  //velocità coccodrilli        
            break;
            
        case 1:  //Medio
            parametri_gioco->livello_difficoltà=2;
            parametri_gioco->vite=5;
            parametri_gioco->tempo=50;
            parametri_gioco->velocità_proiettili=30000;
            parametri_gioco->velocità_coccodrilli=80000;
            
            break;
        case 2:  //Difficile
            parametri_gioco->livello_difficoltà=3;
            parametri_gioco->vite=3;
            parametri_gioco->tempo=30;
            parametri_gioco->velocità_proiettili=20000;
            parametri_gioco->velocità_coccodrilli=60000;
            
            break;      
    }
}

int game_vinto(WINDOW *finestra_gioco, int punteggio){  //schermata in caso di vittoria
    wclear(finestra_gioco);
    box(finestra_gioco, ACS_VLINE, ACS_HLINE);
    mvwprintw(finestra_gioco, 10, 10, "HAI VINTO! Punteggio: %d. Vuoi giocare ancora? (s/n)", punteggio);
    wrefresh(finestra_gioco);
    char decisione = wgetch(finestra_gioco);
    while (decisione!= 's' && decisione!='n') {
        decisione = wgetch(finestra_gioco);
    }
    if (decisione == 's') {  //se rispondiamo di si ripetiamo il gioco (con la stessa difficoltà) da capo
        return 0;
    } else {  //altrimenti torniamo al menù
        return 1;
    }
}


int game_perso(WINDOW *finestra_gioco, int punteggio){  //schermata in caso di perdita
    wclear(finestra_gioco);
    box(finestra_gioco, ACS_VLINE, ACS_HLINE);
    mvwprintw(finestra_gioco, 10, 10, "GAME OVER! Punteggio: %d. Vuoi giocare ancora? (s/n)", punteggio);
    wrefresh(finestra_gioco);
    char decisione = wgetch(finestra_gioco);
    while (decisione!= 's' && decisione!='n') {
    	 decisione = wgetch(finestra_gioco);
    }
    if (decisione == 's') {  //se rispondiamo di si ripetiamo il gioco (con la stessa difficoltà) da capo
        return 0;
    } else {  //altrimenti torniamo al menù
        return 1;
    }
}



int scelta_difficoltà(WINDOW *finestra_gioco) {  //schermata per scelta della difficoltà
    const char *difficolta[] = {"Facile", "Media", "Difficile"};
    return menu(finestra_gioco, "Scegli la Difficoltà", difficolta, 3);
}


void generatore_finestra(WINDOW *finestra_gioco, Statistiche * statistiche_gioco){
    
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
    	for (int j=0; j<COLS-2;j++) {				
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
	    	for (int h=5+(8+7)*i; h<5+15*i+9 ;h++) {  //la formula all'interno è ricavata per non riscrivere più volte lo stesso codice (quello che printa ogni tana)							
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
        mvwhline(finestra_gioco, spostamento_verticale + i, spostamento_orizzontale, ' ', COLS - 2);
    }
    wattroff(finestra_gioco, COLOR_PAIR(6));
   
    spostamento_verticale += SPONDA_SUPERIORE;  


    //SEZIONE FIUME
    
    wattron(finestra_gioco,COLOR_PAIR(2));
    for (int i=0; i<FIUME;i++) {
        for (int j=0; j<COLS-2;j++) {										
    	    mvwaddch(finestra_gioco,i+spostamento_verticale,j+spostamento_orizzontale,fiume[i][j]);
        }
    }
    wattroff(finestra_gioco,COLOR_PAIR(2));
    
    spostamento_verticale += FIUME;  
    
    
    //SEZIONE PRATO
    
    wattron(finestra_gioco,COLOR_PAIR(8));
    for (int i=0; i<PRATO;i++) {
    	for (int j=0; j<COLS-2;j++) {											
    	    mvwaddch(finestra_gioco,i+spostamento_verticale,j+spostamento_orizzontale,prato[i][j]);
        }
    }
    wattroff(finestra_gioco,COLOR_PAIR(8));
    
    //ZONA ESTERNA ALLA MAPPA DI GIOCO 
    
    //mostriamo zona di riempimento della schermata
    wattron(finestra_gioco,COLOR_PAIR(14));
    mvwhline(finestra_gioco, 1,1, ' ', COLS - 2);
    mvwhline(finestra_gioco, 3,1, ' ', COLS - 2);
    mvwhline(finestra_gioco, 45,1, ' ', COLS - 2);  
    mvwhline(finestra_gioco, 47,1, ' ', COLS - 2);  
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


Statistiche  Partita(WINDOW *finestra_gioco,Parametri parametri_gioco){			
    
    //Creazione pipe;
    int pipe1[2];  //per comunicare da produttori a consumatore;
    int pipe2[2];  //per comunicare alla granata se è morta;
    
    if (pipe(pipe1) == -1) {
        perror("errore nella creazione della pipe1: ");
        exit(1);
    }
    if (pipe(pipe2) == -1) {
        perror("errore nella creazione della pipe2: ");
        exit(1);
    }      
   
    sblocca_fd(pipe1[0]);  //sblocchiamo la pipe1 in lettura
    sblocca_fd(pipe2[0]);  //sblocchiamo la pipe2 in lettura
   
    //Inizializziamo variabili di gestione della partita
    Statistiche  statistiche_gioco;
    statistiche_gioco.punteggio=0; 		    //tiene conto del punteggio di gioco
    statistiche_gioco.vite=parametri_gioco.vite;    //tiene conto del numero di vite rimaste
    statistiche_gioco.tempo=parametri_gioco.tempo;  //tiene conto del tempo rimasto nella manche
   
    int tane_occupate=0;
    for (int i=0;i<TANE-1;i++) {
        statistiche_gioco.tane[i]=0;  //impostiamo come 'aperte' tutte le tane
    }
    
    Flusso flussi[NUMERO_FLUSSI];
    velocità_flussi(flussi,parametri_gioco.velocità_coccodrilli);  //definiamo la velocità di ogni flusso                
    int array_pid[NUMERO_PID];  //array contenente i pid dei processi creati (utilizzato per uccidere o mettere in pausa i processi) 
    
   
    
    while (true) {
        
        direzione_flussi(flussi);  //definiamo la direzione di ogni flusso					
    	creazione_processi(flussi,array_pid, pipe1,finestra_gioco, &parametri_gioco);  //creiamo tutti i processi di gioco
	gestore_grafica(finestra_gioco,pipe1,pipe2,array_pid,parametri_gioco.velocità_proiettili,&statistiche_gioco);  //funzione di gestione grafica (gestisce collisioni e la grafica)
	
	
    	statistiche_gioco.tempo=parametri_gioco.tempo;  //riportiamo a default il tempo di gioco 	

        
   	kill_processi(array_pid);  //uccidiamo tutti i processi
   	svuota_pipe(pipe1[0]);  //svuotiamo la pipe per eliminare eventuali elementi rimasti all'interno
			
        
        //CONTROLLI DI FINE MANCHE
        
        if (statistiche_gioco.vite==0) { //controllo sul numero di vite
 	    statistiche_gioco.gioco=0;
 	    statistiche_gioco.punteggio-=50;
 	    return statistiche_gioco;  //uscita per vite finite
	}
	
   	for (int i=0;i<5;i++) {
 	    if (statistiche_gioco.tane[i]==1) {
  	        tane_occupate++;
  	    }
 	}
 	
 	if (tane_occupate==5) {	 //controllo sul numero di tane chiuse	
 	    statistiche_gioco.gioco=1;
 	    statistiche_gioco.punteggio+=100;
	    return statistiche_gioco;  //uscita per tane chiuse
 	}
	tane_occupate=0;
   
	wclear(finestra_gioco);
	wrefresh(finestra_gioco);     
	}
}


void gestore_grafica(WINDOW* finestra_gioco,int pipe1[], int pipe2[], int array_pid[],int velocità_proiettili, Statistiche * statistiche_gioco){			
	
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
    int id_coccodrillo;  //variabile utilizzata per indicare su quale coccodrillo posa la rana
    int distanze_coccodrilli[NUMERO_FLUSSI];  //array utilizzato per tenere conto della distanza da mantenere tra i coccodrilli per ogni flusso (evita che si sovrappongano quando spawnano)
    
    for (int i=0; i<NUMERO_FLUSSI;i++) {
	distanze_coccodrilli[i]=numero_random(13,16);  //impostiamo le distanze iniziali da mantenere tra i coccodrilli, per ogni flusso del fiume
    }
   
    int gioca=1;  //variabile utilizzata per terminare la manche quando si sono verificate le giuste condizioni
    
    while (gioca) {
    	
	gioca=1;  //se durante il ciclo viene impostata a 0 allora si esce dal gioco
	temp.id=-1;  //valore di default 
	
	//funzioni di gestione grafica del gioco
	werase(finestra_gioco);
	generatore_finestra(finestra_gioco,statistiche_gioco);
        disegna_coccodrilli(finestra_gioco,coccodrilli);
        disegna_granate(finestra_gioco,granate);
        disegna_proiettile(finestra_gioco,proiettili);
        disegna_rana(finestra_gioco,&rana);
        barra_tempo(finestra_gioco,statistiche_gioco,tempo); //mostra la barra dinamica del tempo  
        wrefresh(finestra_gioco);
	
	usleep(500); //sleep per temporizzare le letture
	
        read(pipe1[0],&temp,sizeof(Temp));  //leggiamo il messaggio nella pipe
	
       
	//se l'ID è della rana						
	if (temp.id==ID_RANA) {
	    if (rana_in_finestra(&rana,&temp)) {  //controllo per evitare che la rana fuoriesca dalla finestra di gioco
	        rana.x+=temp.x;
	        rana.y+=temp.y;	
	
		if (rana.y<10) {  //controllo se la rana si trova nella zona delle tane			
		    if (rana_su_tana(&rana,statistiche_gioco)) {  //controllo se la rana si trova su una delle tane        
		        statistiche_gioco->punteggio+=15;
			statistiche_gioco->punteggio+=(int)(15*(float)statistiche_gioco->tempo/100);
			gioca=0;						
		    } else {  //altrimenti la rana si trova nella zona circostante alle tane ( e muore)
		        statistiche_gioco->vite--;
			statistiche_gioco->punteggio-=10; 
		        gioca=0;
		    }
		}
		
		if(rana.y>=15 && rana.y<40){  //controllo se la rana si trova nella zona del fiume
		 
		    if(rana_su_coccodrillo(&rana,coccodrilli)==-1){ //verifichiamo che sia su un coccodrillo altrimenti muore
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
		gioca=0;
	    }
	}
	
	//se l'ID è di un coccodrillo
	if (temp.id<NUMERO_COCCODRILLI && temp.id>=0) {
	    
		id_coccodrillo=rana_su_coccodrillo(&rana,coccodrilli);  //controllo di verifica che la rana sia già su un coccodrillo (qualsiasi) prima dello spostamento
		coccodrilli[temp.id].x=temp.x;
		coccodrilli[temp.id].y=temp.y;
	        coccodrilli[temp.id].dir=temp.info;  
		
		//controlli per lo spostamento della rana
		gioca=movimento_rana_su_coccodrillo(temp.id, id_coccodrillo,coccodrilli, &rana,statistiche_gioco, gioca);	
		
		//controllo sullo stato del coccodrillo
		controllo_stato_coccodrillo(temp.id,coccodrilli);
			
			
		//gestione dello stato di attesa dei coccodrilli
		attesa_coccodrilli(temp.id, coccodrilli,distanze_coccodrilli,array_pid);
		riattivazione_coccodrilli(coccodrilli, distanze_coccodrilli, array_pid);
		
	    
	}
	

        
	//se l'id è delle granate
	if (temp.id>=ID_GRANATE && temp.id<=ID_GRANATE+2) {
	    if (temp.id==ID_GRANATE+2) {  //id relativo alla richiesta di sparo delle granate
	    
	        if (granate[0].vivo==0 && granate[1].vivo==0) {  //granate nuove create se le precedenti sono morte (il gioco è impostato in modo tale che si abbia solo un colpo in canna)		
	            spara_granata(rana.x,rana.y,velocità_proiettili,array_pid,pipe1,pipe2);
	           
	            granate[0].vivo=1;
                    granate[1].vivo=1;
          
                    
	            
	        }
	    } else {  
	        granate[temp.id-ID_GRANATE].x=temp.x;	
		granate[temp.id-ID_GRANATE].y=temp.y;
		
			   
	    }	  
	}
	

	
	//se l'id è di un proiettile
	if (temp.id>=ID_PROIETTILE && temp.id<ID_PROIETTILE+NUMERO_PROIETTILI) {	
	    proiettili[temp.id-ID_PROIETTILE].x=temp.x;
	    proiettili[temp.id-ID_PROIETTILE].y=temp.y;
	    
	}

        //controllo se sparare un nuovo proiettile
        controllo_sparo_proiettile(pipe1, array_pid,coccodrilli,&rana,proiettili, &inizio,&fine, &numero_randomico, velocità_proiettili);  	
	
	//controllo collisione proiettile-bordi
	collisione_proiettili_confine(array_pid, proiettili);
	
	//controllo collisione granate-bordi
	collisione_granate_confine(pipe2[1],granate);
	
	//controllo collisione rana-proiettile
	gioca=collisione_rana_proiettili(&rana, proiettili,statistiche_gioco, gioca);   
	
	//controllo collisione granata-proiettile
	collisione_granate_proiettili(pipe2[1],granate, proiettili, array_pid,statistiche_gioco);
	
	//controllo per killare il processo granate una volta morte
	uccidi_granate(granate, array_pid);
        
        
        
    }
}

void controllo_sparo_proiettile(int* pipe1, int* array_pid, Coccodrillo* coccodrilli, Rana* rana,  Proiettile* proiettili,struct timeval* inizio, struct timeval* fine, float* numero_randomico, int velocità_proiettili){
    int coccodrillo_scelto;
    int indice_proiettile=0;
    gettimeofday(fine,NULL);				
    if ((fine->tv_sec - inizio->tv_sec) + (fine->tv_usec - inizio->tv_usec)/1e6>=*numero_randomico) {  //controlliamo se il tempo trascorso supera la soglia per lo sparo del proiettile
	
	    
        do {
            coccodrillo_scelto = numero_random(0, NUMERO_COCCODRILLI - 1);
    
	} while (coccodrilli[coccodrillo_scelto].vivo != 1 || rana_su_coccodrillo(rana, coccodrilli)==coccodrillo_scelto);  //individuiamo un coccodrillo vivo da cui sparare	
	            
	while (array_pid[ID_PROIETTILE+indice_proiettile]!=0) {  //individuiamo uno slot libero da cui creare il proiettile
 	    indice_proiettile++;
	}
	
	spara_proiettile(indice_proiettile,coccodrillo_scelto,coccodrilli,velocità_proiettili,array_pid,pipe1); //spariamo il proiettile assocciandolo al coccodrillo scelto
	proiettili[indice_proiettile].vivo=1;  //impostiamo il proiettile sparato come vivo    
					
	gettimeofday(fine,NULL);		
	gettimeofday(inizio,NULL);
	*numero_randomico=numero_random_float(0.5,1.0);  //cambiamo l'intervallo di tempo per aggiungere casualità al gameplay;	
	
    }

}

//funzione che restituisce un numero numero_random tra due estremi (compresi), di tipo float
float numero_random_float(float minimo, float massimo) {
    return minimo + ((float)rand() / (float)RAND_MAX) * (massimo - minimo);
}


//funzione che gestisce la creazione del processo proiettile
void spara_proiettile(int id_proiettile,int id_coccodrillo, Coccodrillo* coccodrilli,int velocità_proiettili, int pid_array[], int pipe1[]){
   
    pid_array[ID_PROIETTILE+id_proiettile]= fork(); // Crea un nuovo processo per la granata
    
    
    if (pid_array[ID_PROIETTILE+id_proiettile] < 0) {
        perror("Errore nella creazione del processo proiettile: ");
        exit(1);
    }

    if (pid_array[ID_PROIETTILE+id_proiettile] == 0) {
        
        funzione_proiettile(id_proiettile,coccodrilli[id_coccodrillo],velocità_proiettili,pipe1);  //chiamiamo la funzione proiettile all'interno del processo figlio
        
    }
}

//funzione che gestisce il movimento della rana su un coccodrillo
int movimento_rana_su_coccodrillo(int id_attuale, int id_coccodrillo, Coccodrillo* coccodrilli, Rana* rana, Statistiche * statistiche_gioco, int gioca ) {

    if (id_coccodrillo==coccodrilli[id_attuale].id) {  //controlliamo che il coccodrillo attuale sia anche quello su cui la rana era posata prima dello spostamento
        if ((rana->x<=2 && coccodrilli[id_attuale].dir==-1) || (rana->x>=LARGHEZZA_GIOCO-3 && coccodrilli[id_attuale].dir==1)) {  //controlliamo se la rana si trova agli estremi della mappa			
	    if (rana_su_coccodrillo(rana,coccodrilli)!=id_coccodrillo) {  //se la rana non viene considerata più sopra l'attuale coccodrillo allora è caduta in acqua sul bordo della mappa 
	        statistiche_gioco->vite--;
		statistiche_gioco->punteggio-=10;	    
		return 0;
	    }	
	} else {  //altrimenti la rana si muove con il coccodrillo (ovvero non si trovava agli estremi della mappa)					
	rana->x+=coccodrilli[id_attuale].dir;
	}               			
    }
    return gioca;
}



void uccidi_granate(Granata* granate, int array_pid[]){
    
    if (granate[0].vivo==0 && granate[1].vivo==0 && array_pid[ID_GRANATE]!=0) {  //se entrambe sono morte killiamo il loro processo 
               
	kill_processo(array_pid[ID_GRANATE]);
    	
	array_pid[ID_GRANATE]=0;  //liberiamo la zona dell'array relativa al processo granate 
    }
}


void controllo_stato_coccodrillo(int id,Coccodrillo* coccodrilli){

    if ((coccodrilli[id].x>=SPAWN_DX_COCCODRILLO && coccodrilli[id].x==SPAWN_SX_COCCODRILLO && coccodrilli[id].dir==1) || (coccodrilli[id].x<=SPAWN_SX_COCCODRILLO&& coccodrilli[id].x==SPAWN_DX_COCCODRILLO && coccodrilli[id].dir==-1)) {  //verifichiamo se il coccodrillo è all'interno della mappa 
        coccodrilli[id].vivo=0;  //coccodrillo fuori dalla mappa = non attivo/morto
    } else {  
        coccodrilli[id].vivo=1;  //coccoddrillo dentro alla mappa = attivo/vivo
    }
}


void collisione_granate_proiettili(int pipe2, Granata* granate, Proiettile proiettili[], int array_pid[], Statistiche * statistiche_gioco){
        
    //controllo collione tra una qualsiasi granata e un qualsiasi proiettile
    for (int i=0; i<2; i++) {
	for (int j=0; j<NUMERO_PROIETTILI;j++) {
	    if (granate[i].x==proiettili[j].x && granate[i].y==proiettili[j].y && proiettili[j].vivo==1 && granate[i].vivo==1) {
	        
	        kill_processo(array_pid[ID_PROIETTILE+j]);  //killiamo il processo proiettile
	        array_pid[ID_PROIETTILE+j]=0;  //liberiamo la zona relativa al proiettile ucciso
		    
	        //impostiamo a morti il proiettile e la granata mettendoli anche nella posizione di default 
	        proiettili[j].vivo=0;
	        proiettili[j].x=-2;
	        granate[i].vivo=0;
	        granate[i].x=-1;
	        write(pipe2,&i, sizeof(int));  //avvertiamo il processo granata che una delle granate è morta
	        statistiche_gioco->punteggio+=5;
		    
 	    }
	}
    }
}


void attesa_coccodrilli(int id_coccodrillo, Coccodrillo* coccodrilli, int distanze_coccodrilli[], int array_pid[]){
    int indice_flusso;
    
    if ((coccodrilli[id_coccodrillo].x==SPAWN_DX_COCCODRILLO && coccodrilli[id_coccodrillo].dir==-1) || (coccodrilli[id_coccodrillo].x==SPAWN_SX_COCCODRILLO && coccodrilli[id_coccodrillo].dir==1)) {  //controlliamo che il coccodrillo corrente si trovi a inizio tragitto (quindi appena prima di entrare in mappa)		
        for (int i=0;i<NUMERO_COCCODRILLI;i++) {
            indice_flusso=(coccodrilli[i].y-37)/-3;  //individuiamo l'indice del flusso del coccoddrillo corrente
			
	    //controlliamo se il coccodrillo attuale stia spawnando con coccodrilli troppo vicini nelle stesso flusso (ovviamente esclusi quelli in attesa)
	    if (i!=coccodrilli[id_coccodrillo].id && coccodrilli[i].y==coccodrilli[id_coccodrillo].y && ((coccodrilli[id_coccodrillo].x-distanze_coccodrilli[indice_flusso]<coccodrilli[i].x && coccodrilli[id_coccodrillo].dir==-1) || (coccodrilli[id_coccodrillo].x+distanze_coccodrilli[indice_flusso]>coccodrilli[i].x && coccodrilli[id_coccodrillo].dir==1)) && coccodrilli[i].attesa!=1){
	        coccodrilli[id_coccodrillo].attesa=1;  //lo mettiamo in attesa perchè ci sono coccodrilli non in attesa vicino a dove spawna
	        kill(array_pid[id_coccodrillo],SIGSTOP);  //lo stoppiamo
	    }
        }
    }
}

void riattivazione_coccodrilli(Coccodrillo* coccodrilli, int distanze_coccodrilli[], int array_pid[]){

    int indice_flusso;
    int riattivare;
    
    for (int i=0;i<NUMERO_COCCODRILLI;i++) {  //controlliamo se dobbiamo sbloccare qualche coccodrillo
        if (coccodrilli[i].attesa==1) {
	    indice_flusso=(coccodrilli[i].y-37)/-3;  //individuiamo l'indice del flusso del coccoddrillo corrente	
	    riattivare=1;
	    for (int j=0;j<NUMERO_COCCODRILLI;j++) {  //per ogni coccodrillo in attesa controlliamo se ci sono ancora coccodrilli nelle vicinanze (quelli in attesa non contano)
	        if(i!=j && coccodrilli[j].y==coccodrilli[i].y && ((coccodrilli[i].x-distanze_coccodrilli[indice_flusso]<coccodrilli[j].x && coccodrilli[i].dir==-1) || (coccodrilli[i].x+distanze_coccodrilli[indice_flusso]>coccodrilli[j].x && coccodrilli[i].dir==1)) && coccodrilli[j].attesa!=1){
		    riattivare=0;
		    break; 
		}		
	    }
	    if (riattivare) {  //se riattivare è rimasto uguale a 1 significa che non aveva coccodrilli nelle vicinanze	  	  
	        distanze_coccodrilli[indice_flusso]=numero_random(13,16);   //modifichiamo la distanza da rispettare così da aggiungere casualità							    	
		coccodrilli[i].attesa=-1;  //togliamo la attesa e lo facciamo ripartire;
		kill(array_pid[i],SIGCONT);	
	    }
	}
    }
}

void collisione_proiettili_confine(int array_pid[], Proiettile proiettili[]){

    //per ogni proiettile controlliamo se ha colpito il bordo della mappa
    for (int i=0; i<NUMERO_PROIETTILI; i++) {
	if ((proiettili[i].x<1 || proiettili[i].x>LARGHEZZA_GIOCO-2) && proiettili[i].vivo==1 && proiettili[i].x!=-2) {	
	    kill_processo(array_pid[ID_PROIETTILE+i]);  //lo uccidiamo
	    array_pid[ID_PROIETTILE+i]=0;
	    //lo impostiamo a morto e in posizione di default
	    proiettili[i].vivo=0;
	    proiettili[i].x=-2;
	    
	}
    }
}

void collisione_granate_confine(int pipe2, Granata* granate){
    //per ogni granata controlliamo se ha colpito il bordo della mappa
    for (int i=0; i<2;i++) {
	if ((granate[i].x<1 || granate[i].x>LARGHEZZA_GIOCO-2) && granate[i].vivo==1 && granate[i].x!=-1) {
	    //la impostiamo a morta e in posizione i default
	    granate[i].vivo=0;
	    granate[i].x=-1;  
	    write(pipe2,&i, sizeof(int));  //scriviamo al processo granate quale delle due granate è morta    	
	}
    }
}

//funzione che mostra la barra del tempo rimanente
void barra_tempo(WINDOW* finestra_gioco,Statistiche * statistiche_gioco, int tempo){

    wattron(finestra_gioco, COLOR_PAIR(7));
    mvwhline(finestra_gioco,46,15, ' ', (int)(62*((float)statistiche_gioco->tempo/tempo)));
    wattroff(finestra_gioco, COLOR_PAIR(7));
}


//funzione che controlla se la rana si trova in mappa
int rana_in_finestra(Rana* rana, Temp* temp){

    if (rana->x+temp->x>LARGHEZZA_GIOCO-3) return 0;
    if (rana->x+temp->x<2) return 0;
    if (rana->y+temp->y>ALTEZZA_GIOCO-6) return 0;
    if (rana->y+temp->y<7) return 0;
    return 1;
}

//funzione di controllo collisione rana-proiettile
int collisione_rana_proiettili(Rana*rana,Proiettile proiettili[], Statistiche * statistiche_gioco, int gioca){
    
    //per ogni proiettile controlla se collide con la rana
    for (int i=0;i<NUMERO_PROIETTILI;i++) {
 
        if ((rana->x+1==proiettili[i].x || rana->x-1==proiettili[i].x) && rana->y==proiettili[i].y && proiettili[i].vivo) {
	    statistiche_gioco->vite--;
	    statistiche_gioco->punteggio-=15;
	    return 0;
        }
    }
    
    return gioca;    
}

//funzione di controllo se la rana è su una tana
int rana_su_tana(Rana* rana, Statistiche * statistiche_gioco){
	  
    //verifica se la rana si trova su una tana aperta, altrimenti restituisce 0
    for (int i=0;i<5;i++) {
        if (rana->x>=8+(15)*i && rana->x<8+(15)*i+5 && statistiche_gioco->tane[i]==0) {
   	    statistiche_gioco->tane[i]=1;
 	    return 1;
 	}
    }
    return 0;
}
	 	
	  		 

//funzione che svuota la pipe 
void svuota_pipe(int pipe_fd){
	
    int a;
    while (read(pipe_fd,&a,sizeof(int))!= -1) {  //leggiamo dalla pipe finchè non è vuota
	
    } 
}

//funzione che verifica se la rana è su un coccodrillo
int rana_su_coccodrillo(Rana* rana, Coccodrillo *coccodrilli){
    for (int i = 0; i < NUMERO_COCCODRILLI; i++) {
        if (coccodrilli[i].vivo && rana->y == coccodrilli[i].y &&  rana->x-1>= coccodrilli[i].x-4 && rana->x+1 <= coccodrilli[i].x+4) {  //la rana si trova su un coccodrillo se la sua intera figura è al suo interno
       
            return coccodrilli[i].id; 
            
        }
    }
    return -1; 
}




//funzione di creazione dei processi
void creazione_processi(Flusso *flussi, int array_pid[NUMERO_PID], int pipe1[], WINDOW* finestra_gioco, Parametri* parametri_gioco){
    
    for (int i=0;i<NUMERO_PID;i++) {
	array_pid[i]=0;  //settiamo tutti i pid a 0 per cominciare a creare i processi
    }	

    array_pid[ID_RANA]=fork();  //creaiamo il processo rana
    if (array_pid[ID_RANA]==-1) {
	perror("Errore nella fork della rana : ");
	exit(1);
    } else if (array_pid[ID_RANA]==0) {
	funzione_rana(finestra_gioco,pipe1); 

    } else {  
        array_pid[ID_TIME]=fork();  //creiamo il processo del tempo
	if (array_pid[ID_TIME]==-1) {
	    perror("Erorre nella fork del tempo: ");
	    exit(1);
	} else if (array_pid[ID_TIME]==0) {
	    tempo(pipe1);
	} else {
	   
	    funzione_gestione_coccodrilli(flussi,array_pid, pipe1 , parametri_gioco);
	
            return;
	    
	}
    }
    
    
}

//funzione che assegna una velocità a ogni flusso
void velocità_flussi(Flusso *flussi, int vel){
	
    int altezza_base=37;  //posizione del primo flusso (quello più in basso)
    int dimensione_flussi=3;  //larghezza dei flussi
    for (int i=0;i<NUMERO_FLUSSI;i++) {
        flussi[i].y=altezza_base - i*dimensione_flussi;  //impostiamo anche l'altezza di ogni flusso 
	flussi[i].velocità= numero_random(vel - 20000,vel+20000);
    }
}




//funzione che assegna una direzione ad ogni flusso
void direzione_flussi(Flusso *flussi){
    for (int i=0;i<NUMERO_FLUSSI;i++) {
	if (i>=1) {  //tutti i flussi dal secondo in poi hanno direzione opposta al flusso precedente
	    flussi[i].dir= -flussi[i-1].dir;

	} else {
	    //impostiamo la direzione del primo flusso in modo randomico
	    flussi[i].dir= rand()%2;
	    if (flussi[i].dir==0) flussi[i].dir=1;
	    else flussi[i].dir=-1;
	}
    }	
}

//funzione che gestisce il proiettile
void funzione_proiettile(int id, Coccodrillo coccodrillo,int vel,int pipe1[]){   
    close(pipe1[0]);
    Temp proiettile={ID_PROIETTILE+id,0,0,0};
    
    //impostiamo il proiettile in base al coccodrillo che lo ha sparato
    if (coccodrillo.dir==1) proiettile.x=coccodrillo.x+5;
    else proiettile.x=coccodrillo.x-5;	
    proiettile.y=coccodrillo.y;	

    while (true) {

	    
        write(pipe1[1], &proiettile, sizeof(Temp));  	
        		
        proiettile.x+=coccodrillo.dir;
        usleep(vel);
	
    }
}

//funzione di gestione dei coccodrilli
void funzione_gestione_coccodrilli(Flusso *flussi,int* array_pid, int pipe1[],Parametri* parametri_gioco){
    
    
    int numero_coccodrilli_flussi[NUMERO_FLUSSI]={0};  //variabile per tenere conto del numero di coccodrilli per ogni flusso
    int flusso_scelto;  //flusso scelto per lo spawn del coccodrillo
    Temp coccodrillo;
    Temp messaggio;   
    pid_t pid_coccodrillo;
    int numero_coccodrilli=0;  //numero di coccodrilli spawnati
    
    for(int i=0;i<NUMERO_COCCODRILLI;i++){
    
    	if ( numero_coccodrilli<10 ) {  //per i primi 10 coccodrilli lo spawn è completamente randomico
    	    do {
                flusso_scelto=numero_random(0,7);
            
            }while(numero_coccodrilli_flussi[flusso_scelto]==3);  //creiamo i coccodrilli in flussi casuali senza superare i 3 coccodrilli per flusso 
    	
    	} else {  //per i restanti 14 coccodrilli, prima dello spawn casuale controlliamo se ci sono flussi vuoti da riempire
    	    
    	    do {
                flusso_scelto=numero_random(0,7);
                for ( int j= 0; j<NUMERO_FLUSSI; j++) {
                    if ( numero_coccodrilli_flussi[j]==0 ) {
                        flusso_scelto=j;
                        break;
            
                    }
                }
            } while (numero_coccodrilli_flussi[flusso_scelto]==3);  //creiamo i coccodrilli in flussi casuali senza superare i 3 coccodrilli per flusso 
    	}
        
       	
	//impostiamo il coccodrillo
	coccodrillo.id=i;
	coccodrillo.info=0;
	coccodrillo.y=flussi[flusso_scelto].y;  //il coccodrillo prende l'altezza del flusso in cui deve spawnare
	
	numero_coccodrilli_flussi[flusso_scelto]++;  //aumentiamo di uno il numero di coccodrilli nel flusso selezionato
	numero_coccodrilli++;  //aumentiamo i coccodrilli totali spawnati
	
	//impostiamo la posizione di spawn in base alla direzione del flusso
	if (flussi[flusso_scelto].dir==1) {
	    coccodrillo.x=SPAWN_SX_COCCODRILLO-1;   
	} else {
	    coccodrillo.x=SPAWN_DX_COCCODRILLO+1;
	}
	
	pid_coccodrillo=fork();
	if (pid_coccodrillo==-1) {
	    perror("Errore nella fork del coccodrillo: ");
	    exit(1);
	    
	} else if (pid_coccodrillo==0) {
	    funzione_coccodrillo(coccodrillo,flussi,flusso_scelto,pipe1,parametri_gioco);
		
	} else {
	
	    array_pid[i]=pid_coccodrillo;      
	               
	}
     
    } 
}	

//funzione di controllo del coccodrillo
void funzione_coccodrillo(Temp coccodrillo,Flusso flussi[],int id_flusso_scelto, int pipe1[],Parametri* parametri_gioco){

    usleep(numero_random((int)(500000*((float)parametri_gioco->velocità_coccodrilli/120000)),(int)(800000*((float)parametri_gioco->velocità_coccodrilli/120000)))*coccodrillo.id); 
    
    
    coccodrillo.info=flussi[id_flusso_scelto].dir;
    int direzione= flussi[id_flusso_scelto].dir;
    
    while (true) {

	
	coccodrillo.x+=direzione;
	write(pipe1[1], &coccodrillo, sizeof(Temp)); 
	usleep(flussi[id_flusso_scelto].velocità);
	
	
	//se il coccodrillo esce fuori dalla mappa allora deve prepararsi a ricominciare il suo tragitto dall'inizio del flusso
	if ((coccodrillo.x>=SPAWN_DX_COCCODRILLO && direzione==1) || (coccodrillo.x<=SPAWN_SX_COCCODRILLO && direzione==-1)) {  
	  
	
	    if (direzione==1) {
	    
	        coccodrillo.x=SPAWN_SX_COCCODRILLO-1;
	        
	    } else {
	    
	        coccodrillo.x=SPAWN_DX_COCCODRILLO+1;
	    }
	    
	    usleep(numero_random((int)(300000*(1-parametri_gioco->livello_difficoltà*0.20)),(int)(3000000*(1-parametri_gioco->livello_difficoltà*0.20))));  //sleep basata sul livello di difficoltà scelto
	}
	    	
    }
}



//funzione che gestisce il tempo
void tempo(int* pipe1){
    close(pipe1[0]);
    Temp tempo={0,0,0,0};		
    tempo.id=ID_TIME;
    while (true) {
	sleep(1);
	write(pipe1[1], &tempo, sizeof(Temp)); //manda un messaggio alla funzione di controllo ogni secondo
    }
}
	

   
//funzione per killare un processo
void kill_processo(pid_t pid){
    int status;
    if (kill(pid, SIGKILL)!=0) {
	perror("errore nel killare il processo: ");
	exit(1);
    } else {
        waitpid(pid, &status, 0);
               
    }
}

//funzione per killare più processi
void kill_processi(pid_t pid[NUMERO_PID]) {
	
    for (int i=0; i<NUMERO_PID;i++) {
      
	if (pid[i]>0) {
	    
	    if (kill(pid[i],SIGKILL)!=0) {
		perror("Errore nel killare i processi: ");
		exit(1);
	    } else {
	     
	        waitpid(pid[i], NULL, 0);	   
	        pid[i]=0;
	    }
	}			
    }
}
   	
 


int menu(WINDOW *finestra_gioco, const char *titolo, const char *opzioni[], int num_opzioni){
    flushinp();  //elimina input residuo
    keypad(finestra_gioco, true);  //abilita l'input da tastiera

    int posizione = 0;
    int gameLINES = getmaxy(finestra_gioco);
    int gameCOLS = getmaxx(finestra_gioco);
    int scelta;

    while (true) {
        wclear(finestra_gioco);
        box(finestra_gioco, ACS_VLINE, ACS_HLINE);

        mvwprintw(finestra_gioco, gameLINES / 2 - 5, gameCOLS / 2 - strlen(titolo)/2, "%s", titolo);
        
        //mostriamo le opzioni
        for (int i = 0; i < num_opzioni; i++) {
            if (posizione == i) {
                wattron(finestra_gioco, COLOR_PAIR(9));
            }

            int x = gameCOLS / 2 - (strlen(opzioni[i]) / 2);
            mvwprintw(finestra_gioco, gameLINES / 2 - 1 + i, x, "%s", opzioni[i]);

            
            wattroff(finestra_gioco, COLOR_PAIR(9));
        }
        
        //gestiamo l'eventuale movimento del cursore sulle opzioni
        scelta = wgetch(finestra_gioco);
        switch (scelta) {
            case KEY_DOWN:
                posizione++;
                if (posizione >= num_opzioni) posizione = 0;
                break;
            case KEY_UP:
                posizione--;
                if (posizione < 0) posizione = num_opzioni-1;
                break;
            case 10: // Invio
                return posizione;
        }

        wrefresh(finestra_gioco);
    }
}


//funzione che mostra i crediti
void crediti(WINDOW *finestra_gioco){
    wclear(finestra_gioco);
    box(finestra_gioco, ACS_VLINE, ACS_HLINE);

    int gameLINES = getmaxy(finestra_gioco);
    int gameCOLS = getmaxx(finestra_gioco);

    mvwprintw(finestra_gioco, gameLINES / 2 - 2, gameCOLS / 2 - 15, "Marta Viglietti");
    mvwprintw(finestra_gioco, gameLINES / 2 - 1, gameCOLS / 2 - 15, "Gabriele Stampatori");
    mvwprintw(finestra_gioco, gameLINES / 2 + 1, gameCOLS / 2 - 17, "Premi un tasto per tornare al menu...");

    wrefresh(finestra_gioco); 
    wgetch(finestra_gioco);
}


//funzione utile per creare un numero numero_random tra un minimo e un massimo (compresi)
int numero_random(int min, int max){

    return min + rand() % (max-min+1);

}



void funzione_rana(WINDOW* finestra_gioco,int pipe1[2]){
    close(pipe1[0]);
    
    int key;
    Temp rana={ID_RANA,0,0,0};  //messaggio usato per inviare alla funzione di controllo i movimenti della rana     
    Temp granata={ID_GRANATE+2,0,0,0};  //messaggio con id apposito per la creazione delle granate
    keypad(finestra_gioco, true);
    

    while (true) {
    
        //cattura l'input da tastiera
        key = (int)wgetch(finestra_gioco);
        rana.x=0;
        rana.y=0;

        switch (key) {
            case KEY_UP:  //movimento verso l'alto
                rana.y=-3;
                write(pipe1[1], &rana, sizeof(Temp));
                break;		
                	
            case KEY_DOWN:  //movimento verso il basso
                rana.y=3;
                write(pipe1[1], &rana, sizeof(Temp));
                break; 		
  
               
            case KEY_LEFT:  //movimento verso sinistra
                rana.x=-2;
                write(pipe1[1], &rana, sizeof(Temp));		
                break;
                
            case KEY_RIGHT:  //movimento verso destra
                rana.x=2;
                write(pipe1[1], &rana, sizeof(Temp));		
                break;
            case 's':  //tasto per sparare le granate 
            	
            	write(pipe1[1],&granata,sizeof(Temp));  //mandiamo messaggio alla funzione di controllo indicando che abbiamo l'intenzione di sparare le granate
                
                break;
         
        }  
    }
}


//funzione che gestisce la creazione del processo granata
void spara_granata(int inizioX, int inizioY,int velocità_proiettile ,int pid_array[],int pipe1[], int pipe2[]){

    pid_array[ID_GRANATE]= fork(); //Crea un nuovo processo per la granata

    if (pid_array[ID_GRANATE] < 0) {
        perror("Errore nella creazione del processo granata: ");
        exit(1); 
    }

    if (pid_array[ID_GRANATE] == 0) {  
        
        funzione_granata(inizioX, inizioY,velocità_proiettile, pipe1,pipe2);  //chiamiamo la funzione granata all'interno del processo figlio
    }
}






//funzione che gestisce il processo granata
void funzione_granata(int inizioX, int inizioY, int velocità_proiettile, int pipe1[], int pipe2[]){		
    						
    close(pipe2[1]);
    close(pipe1[0]);
    Temp granate[2];
    int messaggio;  //variabile per leggere il messaggio dal gestore grafico
    
    //variabili che gestiscono lo stato delle granate
    int alive_destra=1;
    int alive_sinistra=1;
   
    //impostazione iniziale delle granate (id e posizione di partenza)
    granate[0].id=ID_GRANATE; 
    granate[1].id=ID_GRANATE + 1;
    granate[0].x=inizioX - 2;
    granate[1].x=inizioX + 2; 			
    granate[0].y=inizioY;
    granate[1].y=inizioY;   
    granate[0].info=0;
    granate[1].info=0;
    
    svuota_pipe(pipe2[0]);
    

    while (true) {
    	
    	
    	while (read(pipe2[0],&messaggio,sizeof(int))!=-1) {  //leggiamo se ci sono messaggi dal gestore grafico per eventuali granate morte
            if (messaggio) {
                
                alive_destra=0;
            } else { 
                alive_sinistra=0;
                	
            }
        }
        
        //per ogni granata scriviamo nella pipe le nuove coordinate solo se ancora vive
        if (alive_destra) {
	    write(pipe1[1],&granate[1],sizeof(Temp));	 
	    granate[1].x++; 
	}
        	
        if (alive_sinistra) {
	    write(pipe1[1],&granate[0],sizeof(Temp));
	    granate[0].x--;
	}
        usleep(velocità_proiettile);  
    }
 	       
        
}

//funzione che crea i colori utilizzati durante il finestra_gioco
void creazione_colori(){
    if (has_colors()) { //verifica il supporto ai colori da parte del terminale
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
        } else if (coccodrilli[i].vivo && coccodrilli[i].dir==-1 &&  coccodrilli[i].attesa!=1) {   //coccodrillo vivo, non in attesa e direzionato verso sinistra
       	    
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



//funzione che sblocca le pipe
void sblocca_fd(int fd) {

    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("Errore nell'ottenere i flag del fd: ");
        exit(1);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Errore nel settare O_NONBLOCK: ");
        exit(1);
    }
}


//funzione che blocca le pipe
void blocca_fd(int fd) {

    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("Errore nell'ottenere i flag del fd: ");
        exit(1);
    }
    flags &= ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("Errore nel rimuovere O_NONBLOCK: ");
        exit(1);
    }
}

