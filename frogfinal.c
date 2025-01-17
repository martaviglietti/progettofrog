#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/wait.h>
#include "menu.h"
#include <string.h>
#define MAX_CROCODILES 16
#define POS_SPAWN_COC_SINISTRA -4
#define POS_SPAWN_COC_DESTRA 83
#define VELMAX_FLUSSO
#define VELMIN_FLUSSO
#define DELAY 100000
#define TANE 6
#define FIUME 25
#define PRATO 5
#define MARCIAPIEDE 5
#define N_PID 100
//definizione id da usare;
#define IDTIME 40
#define IDCOC 0
#define IDRANA 20
#define IDGRANATE 30
#define IDPROIETTILE 50
#define IDAGGIUNTAPID 60
#define IDMORTE 70


// Opzioni del menu
static const char *OPZIONI[] = {
    "Inizia gioco", "Crediti", "Esci"
};

//sprite rana
const char *frog_sprite[ALTEZZARANA] = {
    "@ @",
    " - "
};

const char *coc_sprite[2][ALTEZZACOCCODRILLO]{" XXXXXXX ","XXXXXX0X0"},{" XXXXXXX ","0X0XXXXXX"};
            
           
          
typedef struct{
    int id;
    int y;
    int dir;
    int speed;
}Flusso;




typedef struct{
    int vite;
    int score;
    int tempo;
    int tane[5];
    int game; //serve per la condizione di uscita dal game;
}Game_struct;


typedef struct{
    int id;
    int y;
    int x;
    int alive;
}Coccodrillo;

typedef struct{
    int id;
    int y;
    int x;
    int alive;
}Proiettile;

typedef struct{
    int id;
    int y;
    int x;
}Granata;



typedef struct{
    int id;
    int x;
    int y;
}Rana;

typedef struct{
	int id;
	int x;
	int y;
}Temp;

void creazione_colori();
void startGame(WINDOW *game, int pipe1[],int pipe2[]);
void windowGeneration(WINDOW *game, int maxX, int maxY,int tane_chiuse[5]);
void definizione_flussi(Flusso *flussi);
void creazione_processi(Flusso *flussi,Coccodrillo *coccodrilli);

bool Gestione_grafica();
void Kill_processi();
void funzione_gestione_coccodrilli(Flusso *flussi,Coccodrillo *coccodrilli);
void funz_rana();
void funz_coccodrillo(Coccodrillo* coccodrillo);
int rand_funz(int min, int max);
void drawCoccodrilli(WINDOW *game, Coccodrillo *coccodrilli);



int main() {
    printf("\e[8;%d;%dt", 49, 80);  
    fflush(stdout);
    sleep(1);

    // Inizializza NCURSES
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    resizeterm(49, 80);       //avvisare ncurses del cambio di dimensioni
    
    
    //creazione pipe;
    int pipe1[2];	
    int pipe2[2];	
    pipe(pipe1);        //pipe per comunicare pid da generatore_coc verso la principale
    pipe(pipe2);	//pipe per comunicare morte da coc a generatore coc;

    int height = LINES;
    int width = COLS;

    WINDOW *game = newwin(height, width, 0, 0); //area gioco  

    box(game, ACS_VLINE, ACS_HLINE);
    wrefresh(game);

    while (true) {
        int result = menu(game);

        if (result == 0) {  // "Inizia gioco"
            startGame(game);
        } else if (result == 2) {  // "Esci"
            wclear(game);
            mvwprintw(game, height / 2, width / 2 - 5, "Uscita...");
            wrefresh(game);
            sleep(1);
            break; // Esci dal programma
        }
    }

    delwin(game);
    endwin();
    return 0;
}


void windowGeneration(WINDOW *game, int maxX, int maxY, int tane_chiuse[5]){
    
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
    
    
    
    
    
    
    
    
    // Sezione superiore: Punteggio e vite
    wattron(game, COLOR_PAIR(7));
    mvwprintw(game, offsetSumV, 2, "Punteggio: 0 | Vite: 3");
    wattroff(game, COLOR_PAIR(7));
    
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
    	if(tane_chiuse[i]==0){
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
    
    // Sezione Marciapiede
    wattron(game, COLOR_PAIR(6));
    for (int i = 0; i < MARCIAPIEDE; i++) {
        
        mvwhline(game, offsetSumV + i, offsetSumH, ' ', maxX - 2);
        
    }
    wattroff(game, COLOR_PAIR(6));
    
    
    
    
    offsetSumV += MARCIAPIEDE;  //siamo a 15



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
    
    
    
    wrefresh(game);
};















void startGame(WINDOW *game, int pipe1[],int pipe2[]) {			
    
    
    	
    wclear(game);
    box(game, 0, 0);
    wrefresh(game);
    
    Game_struct game_struct;
    game_struct.score=0;
    game_struct.vite=3;
    game_struct.time=30;
    game_struct.game=0;   //lo metteremo a uno quando bisognerà concludere la partita;
    
    for(int i=0;i<TANE -1;i++){
    	game_struct.tane[i]=0;}
    
    
    def_vel_flussi(flussi);                            
    int array_pid[N_PID];   
    
    while(gioca){
    	
    	windowGeneration(game,game_struct.tane);      
    	def_dir_flussi(flussi);	
    					
    	creazione_processi(array_pid);		
    
    	
	game_struct.game=Gestione_grafica(pipe1,pipe2,array_pid,&game_struct);		
	
	
	
	
	
   	kill_processi(array_pid);		//killiamo tutto per ricominciare;
   	wclear(game); 
   	wrefresh(game);
	} 
}


int Gestione_grafica(int pipe1[], int pipe2[], int array_pid[]){			
	
	
	
				 
	Temp temp={0,0,0};
	int time; //tempo da stampare a schermo;
	//settare impostazioni default dei vari processi
	Coccodrillo coccodrilli[MAX_CROCODILES];
	for(int i=0; i<MAX_CROCODILES;i++{
		coccodrilli[i].id=i;
		coccodrilli[i].x=-1;
		coccodrilli[i].y=-1}
	
	Rana rana;
	Proiettile proiettile;
	
	Granata granate[2];
	for(int i=0; i<2;i++{
		coccodrilli[i].id=i;
		coccodrilli[i].x=-1;
		coccodrilli[i].y=-1}
		
	rana.id=IDRANA;
	rana.x=-1;
	rana.y=-1;
	
	proiettile.id=IDPROIETTILE;
	proiettile.x=-1;
	proiettile.y=-1;
	
	
	
	while(true){	
		//serve resettare temp? bho vediamo😶‍🌫️️
		windowGeneration(game,game_struct.tane);
					
		read(pipe_fd[0],&temp,sizeof(Temp)); //per adesso la considero bloccante;
		
		//se l'ID è della rana						
		if(temp.id=IDRANA){
			rana.x=temp.x;
			rana.y=temp.y;	
			//verificare collisione tra rana e proiettile
			//verificare se è entrata in una tana se no muore se l'ha missata;
			//verificare che, se è sul fiume, sta su un coccodrillo se no muore;
	
				
	
	
	
	
		}
		//se l'ID è di un coccodrillo
		if(temp.id<MAX_CROCODILES && temp.id>=0){
			if(temp.y!=IDAGGIUNTAPID){
				coccodrilli[temp.id].x=temp.x;
				coccodrilli[temp.id].y=temp.y;
				}
				
			else{
				array_pid[temp.id]=temp.x; //salva l id del coccodrillo creato;
			
			}
		
		
		
		
		
		
		
		
		}
		
		//se l'id è del tempo
		if(temp.id==IDTIME){
			time=temp.x;
			if(time==0){
				game_struct.vite--;
			}
			
		
		
		for(int i=0;i<3;i++){			//scorre per vedere se è uno dei 3 id possibili per le granate;
			if(temp.id==IDGRANATE+i){
				switch(i):
					case 0:
					case 1:
						granate[i].x=temp.x;	
						granate[i].y=temp.y;
						break;
					case 2:
					
						spara_granata(rana.x,rana.y,array_pid); 
						break;
			
			
			}
		
		}
		
		if(temp.id==IDPROIETTILE){
			if(temp.x!=IDMORTE){
				proiettile.x=temp.x;
				proiettile.y=temp.y;
				//bisogna controllare se ha colpito la rana!
				
				
				
				
				
				
				
			else{
				array_pid[IDPROIETTILE]=0;
				//forse la wait ma devo vedere;
		
			}
		}
		
		
		
	DrawCoccodrilli(game,coccodrilli);
	draw_frog(game,rana.x,rana.y);
}

   
   
   




void updateFrogState(Rana *rana, Coccodrillo *coccodrilli, Game_struct* game_struct) {
    // Controlla se la rana è nel fiume
    if (frog->y >= 16 && frog->y <= 37) { // Intervallo del fiume
        int collidedCrocID = frogOnTopOfCrocodrile(rana, coccodrilli);

        if (collidedCrocID != -1) {
            // La rana è sopra un coccodrillo
 
            frog->x += coccodrilli[collidedCrocID].dir;

            // Controllo dei limiti dello schermo
            if (frog->x < 1 || frog->x > screenWidth - LARGHEZZARANA - 1) {
                frog->moving = 0;  // La rana smette di muoversi
                frog->alive = 0;   // La rana affoga
                frog->y = 43;      // Torna al marciapiede
                game_struct->vite--;
            }
        } else {
            // La rana non è sopra un coccodrillo
            frog->moving = 0;
            frog->alive = 0;   // La rana affoga
            frog->y = 43;      // Torna al marciapiede
            decrementaVite();  //funzione per gestire le vite? come si gestisce?
        }
    } else {
        // La rana non è nel fiume
        frog->moving = 0;
    }
}


bool frogOnCrocodile(rana *frog, Coccodrillo *coccodrilli, int maxCoccodrilli) {
    for (int i = 0; i < maxCoccodrilli; i++) {
        if (coccodrilli[i].alive) {
            int crocodileLeft = coccodrilli[i].x;
            int crocodileRight = coccodrilli[i].x + LARGHEZZA_COC;
            int frogLeft = frog->x;
            int frogRight = frog->x + LARGHEZZARANA;

            if (frog->y == coccodrilli[i].y && frogLeft >= crocodileLeft && frogRight <= crocodileRight) {
                frog->moving = true;
                frog->x += coccodrilli[i].dir;
                return true; // La rana è sopra un coccodrillo
            }
        }
    }

    frog->moving = false;
    return false; // Nessuna collisione
}

























void creazione_processi(Flusso *flussi,Coccodrillo *coccodrilli, int array_pid[N_PID]){
	
	for(int i=0;i<N_ID;i++){
		array_pid[i]=0;}	//resettiamo tutti i pid per ricominciare a creare i processi;
	
        array_pid[IDRANA]=fork();
	if(array_pid[IDRANA]==-1){
		perror("Erorre nella fork della rana : ");
		exit(1);}
	else if(array_pid[IDRANA]==0){
		funzione_rana() }                       
	else{
		array_pid[MAX_CROCODILES]=fork();
		if(array_pid[MAX_CROCODILES]==-1){
			perror("Errore nella fork del generatore coccodrilli: ");
			exit(-1);}
		else if(array_pid[MAX_CROCODILES]==0){
			funzione_gestione_coccodrilli(flussi,coccodrilli);}
			     
	 	else{
	 		array_pid[IDTIME]=fork();
			if(array_pid[IDTIME]==-1){
			perror("Errore nella fork del generatore coccodrilli: ");
			exit(-1);}
			else if(array_pid[IDTIME]==0){
				tempo(pipe_fd);} 
				
			else return;	
				
				
		}}}	 		




		
void funzione_gestione_coccodrilli(Flusso *flussi,Coccodrillo *coccodrilli){
	int n_coc_alive=0;
	int n_flusso;
	int flusso_scelto;
	int dir;
	int indici_minimi[8];
	int minor_coccodrilli;
	int n_coc_flusso[8];
	int flusso_scelto;
	minor_coccodrilli=n_coc_flusso[0];
	Temp messaggio={0,0,0};
	pid_t pid_coc;
	

	for(int i=0; i<MAX_CROCODILES;i++){
		for(int i=0;i<8;i++)}
			n_coc_flusso[i]=0;
		}
	   	for (int i = 0; i < 8; i++) {
			if (n_coc_flusso[i] < min_coccodrilli) {		
		    		minor_coccodrilli = n_coc_flusso[i];
		}}
		
		for(int i=0;i<8;i++){							
			if(n_coc_flusso[i]== min_coccodrilli){
				indici_minimvoid draw_frog(WINDOW *game, int x, int y)i[contatore++]=i;}
		}
	       	
		flusso_scelto=indici_minimi[rand()%contatore];
   		
   		coccodrilli[i]->dir=    flussi[flusso_scelto].dir;
   		coccodrilli[i]->y=    flussi[flusso_scelto].y;
   		n_coc_flusso[flusso_scelto]++;
   		if(coccodrilli[i]->dir==1){
   			coccodrilli[i].x=POS_SPAWN_COC_SINISTRA;   
   		}
   		else{
   			coccodrilli[i].x=POS_SPAWN_COC_SINISTRA;
   		}
   		
   		pid_coc=fork();
		if(pid_coc==-1){
			perror("Errorre nella fork coccodrillo: ");
			exit(1);
		else if(pid_coc==0){
   			funzione_coccodrillo(coccodrilli[i],flussi,flusso_scelto);}  
   		else{
   		
   		messaggio.x=pid_coc;						//imposto il messsaggio fa inviare
   		messaggio.id=i;
   		messaggio.y=IDAGGIUNTAPID;                            //serve per far capire che è un nuovo pid da aggiungere nella lista dei pid;      
   		write(pipe_fds[1], &messaggio,sizeof(Temp);                          //invio messaggio alla principale con pid del coc creato       
   		usleep(rand_funz(20000,40000);}
   		if(i== (MAX_CROCODILES/2 -1){
   			usleep(rand_funz(3000000,4000000);				//a metà della creazione dei coc fermo la loro generazione per qualche secondo;
   		}
   		
   	} 
   	
  
  
   	
void funzione_coccodrillo(coccodrilli coccodrillo,Flusso flussi[8],int id_flusso_scelto){
	Flusso flusso_scelto=flussi[id_flusso_scelto];									//serve per salvare il flusso scelto per ogni respawn;
	srand(time(NULL)+coccodrillo.id);
	usleep(rand_funz(2000000,3000000);
	int larghezza_coccoddrillo=4;
	int posiz_destra=83;
	int posiz_sinistra=-4;
	Temp coccodrillo={coccodrillo.id,coccodrillo.x,coccodrillo.y};			//x e y sono già impostate prima della creazione;
	while(true){
		
		coccodrillo.x=coccodrillo.x+flusso_scelto.dir;
		write(pipe[1], &coccodrillo, sizeof(Temp));		//invia la prima posizone quando il primo carattere del coccodrillo è nello schermo;
		
		usleep(flusso_scelto.vel);
		
		if(coccoddrillo.x-larghezza_coccodrillo>78 && coccodrillo.dir=1 || (coccoddrillo.x + larghezza_coccorillo)<1 && coccodrillo.dir=-1){
			flusso_scelto=flusso[rand()%8];
			while(coccodrillo.y==flusso_scelto.y){
				flusso_scelto=flussi[rand()%8];
			}
			
			coccodrillo.y=flusso_scelto.y;
			if(flusso_scelto.dir==1){
			coccodrillo.x=posiz_sinistra;}
			else coccodrillo.x=posix_destra;
			
			usleep(rand_funz(2000000,3000000);
		}
		void draw_frog(WINDOW *game, int x, int y)
}}
}



void proiettile(Coccodrillo coccodrillo){   //vediamo per le pipe;
	
	Temp proiettile={0,0,0};
	int segnale;
	if(coccodrillo.dir==1) proiettile.x=coccodrillo.x+5;
	else                   proiettile.x=coccodrillo.x-5;	
	
	
	proiettile.y=coccodrillo.y-1;		//perchè lo spariamo dal basso;
	proiettile.id= IDPROIETTILE
	
	while(true){
		if(proiettile.x>78 || proiettile.x<1){
			proiettile.x=IDMORTE;
			write(pipe[1], &temp, sizeof(messaggio));  //avvisa della morte;
			break;
		else{
		
			write(pipe[1], &temp, sizeof(messaggio));  //coordinate proiettile alla render;			
			proiettile.x+=coccodrillo.dir;
			usleep(MACRO VEL PROIETT);}
		
		
	}
	_exit(0);
}

	
   
void tempo(pipe()){
	
	temp tempo={0,0,0};		//non manda 30 quindi lo dobbiamo printare noi all inzio (con il primo windowgeneration);
	tempo.id=IDTEMPO;
	tempo.x=30
	while(tempo.x){
		sleep(1);
		tempo.x--;
		write(pipe[1], &temp, sizeof(messaggio)); //manda il tempo attuale alla principale
	}
	
	//vedo se serve una write;
}	
   	
   

void kill_processi(pid_t* pid,int count){
	
	for(int i=0; i<count;i++){
		if(pid[i]>0){
			if(kill(pid[i],SIGKILL)!=0){
				perror("Error killing process");
			}
			else waitpid(pid[i], NULL, 0);}		
	
	}
}
   
   	
   	
   	
   	
   	


void enqueue(int* coc_morti,int* tail,int id){

	coc_morti[*tail]					//funzioni utili per la gestione di una coda mediante array;
	*tail=(*tail+1)%MAX_CROCODILES;
}

void dequeue(int* head){
	*head= (*head +1)% MAX_CROCODILES;
}




void def_vel_flussi(Flusso *flussi){
	
	srand(time(NULL));
	int altezza_base=37;				//posizione verticale di ogni flusso (apparte l ultimo) è la posizione in alto  quindi la terza fila= y di riferimento dei coccodrilli e anche quello dove arriva la rana saltando;
	int dist_flussi=3;
	for(int i=0;i<8;i++){
    		flussi[i].y=altezza_base - i*dist_flussi;
    		flussi[i].speed= VELMIN_FLUSSO + rand()%(VELMAX_FLUSSO - VELMIN_FLUSSO +1);
	}
}





void def_dir_flussi(Flusso *flussi){

    		flussi[i].dir= rand()%2;
    		if(flussi[i].dir==0) flussi[i].dir=1; 
    		else flussi[i].dir=-1;
    
	}
}


int menu(WINDOW *game) {
    flushinp();            // Elimina input residuo
    keypad(game, true);    // Abilita l'input da tastiera

    int position = 0;
    int gameLINES = getmaxy(game);
    int gameCOLS = getmaxx(game);

    while (true) {
        wclear(game);
        box(game, ACS_VLINE, ACS_HLINE);

        mvwprintw(game, gameLINES / 2 - 5, gameCOLS / 2 - 4, "Frogger");
        for (int i = 0; i < 3; i++) {
            if (position == i) {
                wattron(game, COLOR_PAIR(12));
            } else {
                wattron(game, COLOR_PAIR(1));
            }

            int x = gameCOLS / 2 - (strlen(OPZIONI[i]) / 2);
            mvwprintw(game, gameLINES / 2 - 1 + i, x, "%s", OPZIONI[i]);

            wattroff(game, COLOR_PAIR(1));
            wattroff(game, COLOR_PAIR(12));
        }

        int choice = wgetch(game);
        switch (choice) {
            case KEY_DOWN:
                position++;
                if (position >= 3) position = 0;
                break;
            case KEY_UP:
                position--;
                if (position < 0) position = 2;
                break;
            case 10: // Invio
                return position;
        }

        wrefresh(game);
    }
}

void credits(WINDOW *game) {
    wclear(game);
    box(game, ACS_VLINE, ACS_HLINE);

    int gameLINES = getmaxy(game);
    int gameCOLS = getmaxx(game);

    mvwprintw(game, gameLINES / 2 - 2, gameCOLS / 2 - 15, "Marta Viglietti");
    mvwprintw(game, gameLINES / 2 - 1, gameCOLS / 2 - 15, "Gabriele Stampatori");
    mvwprintw(game, gameLINES / 2 + 1, gameCOLS / 2 - 17, "Premi un tasto per tornare al menu...");

    wrefresh(game);
    wgetch(game);
}

int rand_funz(int min, int max){

	return min + rand() % (max-min+1);

}

int rand_funz(int min, int max){

	return min + rand() % (max-min+1);
}











































void drawCoccodrilli(WINDOW *game, Coccodrillo *coccodrilli) {
    for (int i = 0; i < MAX_CROCODILES; i++) {
        if (coccodrilli[i].alive && coccodrilli[i].dir==1) { 	   //direzione a destra
        
            
            wattron(game, COLOR_PAIR(1)); 
	
            // Disegna la sprite del coccodrillo riga per riga
            for (int j = 0; j < ALTEZZACOCCODRILLO; j++) {
                mvwprintw(game, coccodrilli[i].y + j, coccodrilli[i].x-4, "%s", sprite[0][j]);       //supponiamo posizione di riferimento= centro-alto;
            }

            
            wattroff(game, COLOR_PAIR(1));
        }
        else if(coccodrilli[i].alive && coccodrilli[i].dir==-1){   //direzione verso sinistra
       	    
       	     wattron(game, COLOR_PAIR(1)); 
	
            // Disegna la sprite del coccodrillo riga per riga
            for (int j = 0; j < ALTEZZACOCCODRILLO; j++) {
                mvwprintw(game, coccodrilli[i].y + j, coccodrilli[i].x-4, "%s", sprite[1][j]);       //supponiamo posizione di riferimento= centro-alto del coccodrillo;
            }

            
            wattroff(game, COLOR_PAIR(1));
        
        
    }}
    wrefresh(game); // Aggiorna la finestra di gioco
}


void draw_frog(WINDOW *game, int x, int y) {
	//attivare colore rana;
    for (int i = 0; i < ALTEZZARANA; i++) {
        for (int j = 0; j < LARGHEZZARANA; j++) {			//suppongo posizione di riferimento = centro-alto della rana
            mvwaddch(game, y + i, x -1 + j, frog_sprite[i][j]);
        }
    }
    wrefresh(game);
}



void frog(int pipe_fd[2]) {
    int x = 40, y = 43; // Posizione iniziale della rana (nel marciapiede)
    int key;
    Rana rana={IDRANA,x,y};      
    Granata granata={IDGRANATA+2,0,0}; //usiamo per chiedere alla render di creare granata
    // Abilita l'input da tastiera;
    keypad(stdscr, true);
    

    while (1) {
    
        // Cattura l'input da tastiera
        key = (int)wgetch(game);
        

        switch (key) {
            case KEY_UP:
                if(y>7) rana.y-=2;break;		//7 è l'altezza dove si trova la rana quando entra in una tana(noi non vogliamo che vada oltre);
                	
            case KEY_UP:
                if(y<43) rana.y+=2;break; 		//43 è la posizione più in basso per la rana; 
  
                break;
            case KEY_LEFT:
                if(x>2) rana.x-=2; 		//2 è la posizione più a sinistra raggiungibile;
                break;
            case KEY_RIGHT:
                if(x<77) rana.x+=2; 		//77 è la posizione più a destra raggiungibile;
                break;
            case 's': 
            	
            	write(pipe_fd[1],&granata,sizeof(int));   //mando un messaggio alla render con un id che gli fa capire che voglio creare la granata;(sarà la render a decidere se crearla o no!!;
                
                break;
            case 'q': // Esci dal gioco
                return;
        }


        // Invia posizione aggiornata al processo principale tramite pipe
        write(pipe_fd[1], &rana, sizeof(int));
       
        
    }
}



void sparaGranata(int startX, int startY,int pid_array[]) {
    pid_array[IDGRANATA]= fork(); // Crea un nuovo processo per la granata

    if (pid_array[IDGRANATA] < 0) {
        perror("Errore nella creazione del processo granata");
        return;
    }

    if (pid_array[IDGRANATA] == 0) {
        // Processo figlio: gestisce la granata
        granata(startX, startY);
    }
}

void granata(int startX, int startY) {		//posizione della rana;
    						//facciamo partire dal basso i proiettili essendo la bocca della rana e del coccodrillo nella parte bassa;

    int alive_sinistra = 1; // Stato attivo della granata a sinistra
    int alive_destra = 1;   // Stato attivo della granata a destra
    Granata granate[2];
    int granata_morta;
    
    granate[0].id=IDGRANATA 
    granate[1].i=IDGRANATA + 1;
    granate[0].x=startX - 2;	//le granate partono dal carattere appena esterno a quelli della rana;
    granate[1].x=startX + 2; 				
    //verificare se si bugga sul muro
    granate[0].y=startY-1;
    granate[1].y=startY-1;   //facciamo partire dal basso i proiettili essendo la bocca della rana e del coccodrillo nella parte bassa;
    
    
    
//continuo finche almeno una delle granta è attiva
    while (alive_sinistra && alive_destra) {
        if(granate[1].x>78 && alive_destra){       			
        	alive_destra=0;
        }else if(alive_destra){
        	write(pipe_fd[1],granate[1],sizeof(Temp));		//manda coordinate;
        	granate[1].x++;}
        	
        if(granate[0].x<1 && alive_sinistra){
        	alive_sinistra=0;
        }else if(alive_sinistra){
        	write(pipe_fd[1],granate[0],sizeof(Temp));
        	granate[0].x--;}
        	
        if(read(pipe_da_creare,&granata_morta,sizeof(granata_morta))!=-1){ 		//read non bloccante;
        	if(granata_morta=0) alive_sinistra=1;					//se il main riconosce una collisione manda il messaggio al processo granate che disattiva l'invio delle coordinate;
		else alive_destra=1;
        	
        }
    
        
        

        wrefresh(game);
        usleep(GRANATA_VELOCITA); // Velocità del movimento   
    }
    write(pipe[1], &temp, sizeof(messaggio));  //avvisa della morte; (suppongo serva);
    _exit(0);
}


void creazione_colori(){
    if (has_colors()) {       //verifica il supporto ai colori da parte del terminale
        start_color();
        init_color(8,34,139,34); //verde foresta;
   	init_color(9,0,123,184); //blu scuro;		//in futuro li possiamo modificare ma l importante è non modificare l id;
  	init_color(10,95,95,95); //grigio scuro;
  	init_color(11,192,192,192); //grigio chiaro;
   	init_color(12,101,67,33); //marrone
   	init_color(13,152,118,84); //marrone pastello
    
    
    
    
   	init_pair(2, 9, COLOR_BLUE);	      // Fiume "~"
   	init_pair(3, COLOR_YELLOW, COLOR_YELLOW); // Tane zona intorno
   	init_pair(4, 10, 11);                     // Tane recinzione
    	init_pair(5, 13, 13); 		      //tana vera e propria
   	init_pair(6, COLOR_BLACK, COLOR_RED);     // Marciapiede
   	init_pair(7, COLOR_WHITE, COLOR_MAGENTA); // Punteggio e vite		//questi sono da attivare a inizio programma in una funzione apposita;
   	init_pair(8, 8, COLOR_GREEN);   	      //prato;
        
    }else{
    	printw("Il terminale non permette la visualizzazione dei colori\n"); 
    	mvprintw(LINES-1/2,COLS/2,"Premi un tasto per uscire...");refresh();
    	getch();
    	_exit(0);
    }
	
	
}



void draw_proiettile(WINDOW* game, int x, int y){
	
	//devo inizializzare matrice caratteri cosi' che il proiettile avrà lo stesso sfondo della mappa
	



}

void draw_granate(WINDOW* game, int x, int y){
	
	//devo vedere;


}
