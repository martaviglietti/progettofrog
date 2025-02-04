//aggiornato
#include "header.h"
void Gestione_grafica(WINDOW* game,int pipe1[], int pipe2[], int array_pid[],int vel_proiettile, Game_struct* game_struct){			
	
	//per generazione proiettile
	struct timeval start, end; //strutture per utilizzare le funzioni sul tempo;
	gettimeofday(&end,NULL);		
	gettimeofday(&start,NULL);
	long time_proiettile; 
	int random=rand_funz(2,4);
	//
	
	int result; //contiene il risultato delle funzioni di collisione;	 
	Temp temp={-1,0,0,0};
	Temp risposta={0,0,0,0};

	//settare impostazioni default dei vari processi
	Coccodrillo coccodrilli[MAX_CROCODILES];
	for(int i=0; i<MAX_CROCODILES;i++){
		coccodrilli[i].id=i;
		coccodrilli[i].x=-1;
		coccodrilli[i].y=-1;
		coccodrilli[i].alive=0;}
	
	Granata granate[2];
	for(int i=0; i<2;i++){
		granate[i].id=i;
		granate[i].x=-1;
		granate[i].y=-1;
		granate[i].alive=0;}
		
	Rana rana;	
	rana.id=IDRANA;
	rana.x=40; 
	rana.y=43;
	
	Proiettile proiettile;
	proiettile.id=IDPROIETTILE;
	proiettile.x=-1;
	proiettile.y=-1;
	proiettile.alive=0;
	
	int coc_scelto;
	int granata_morta;
	
	while(true){	
		
		
		
		temp.id=-1;
		
		read(pipe1[0],&temp,sizeof(Temp));               //Non bloccante;
		
		
		//se l'ID è della rana						
		if(temp.id==IDRANA){
			rana.x=temp.x;
			rana.y=temp.y;	
			
			if(CollisioneRanaProiettile(rana,proiettile)==1){      //c'è stata collisione con proiettile
				game_struct->vite--;
				//perdita punti?
				return;
			}
			if(rana.y<10){					       //controllo se la rana sta su una tana oppure muore;				
				if(RanaSuTana(rana,game_struct)==1){
					
					//punti
										
				}
				else{
					game_struct->vite--;
					//perdita di punti?;
					return;}
			}
			
			if(rana.y>=15 && rana.y<40){
				if(RanaSuCoccodrillo(&rana,coccodrilli)==-1){ 	//significa che è nel fiume ma non su un coccodrillo;
					game_struct->vite--;
					//perdita punti
					return;
					
					}
			}
		
		}
	
		
		//se l'id è del tempo
		if(temp.id==IDTIME){	//probabilemnte quando otteniamo lo 0 è morto il processo tempo e bisogna fre la wait;
			game_struct->tempo-=1;
			if(game_struct->tempo==0){
				game_struct->vite--;
				//perdita punti?
				return;
		}}
		
		
		
		//se l'ID è di un coccodrillo
		if(temp.id<MAX_CROCODILES && temp.id>=0){
			if(temp.y!=IDAGGIUNTAPID){
					coc_scelto=RanaSuCoccodrillo(&rana,coccodrilli);		//controllo prima del movimento del coc se la rana è sopra un coccodrillo;
					coccodrilli[temp.id].x=temp.x;
					coccodrilli[temp.id].y=temp.y;
					coccodrilli[temp.id].dir=temp.info;
					
					if(coc_scelto==coccodrilli[temp.id].id){
						if((rana.x<=2 && coccodrilli[temp.id].dir==-1) || (rana.x>=77 && coccodrilli[temp.id].dir==1)){        //rana agli estremi della mappa sopra coccodrillo (che si è già mosso nella direzione);				
							if(RanaSuCoccodrillo(&rana,coccodrilli)!=coc_scelto){	//la rana è caduta in acqua dopo che il coccodrillo si è mosso;
								game_struct->vite--;
								//togli punti;
								return;
							}	
						
						} else{						//rana non agli estremi;
							rana.x=coccodrilli[temp.id].x;
						
						}               
					
					}
						
					
					if(coccodrilli[temp.id].x>=POS_SPAWN_COC_DESTRA || coccodrilli[temp.x].x<=POS_SPAWN_COC_SINISTRA){	//verifichiamo se il coccodrillo è all'interno dello schermo;
						coccodrilli[temp.id].alive=0;									//ci serve per sapere se stamparli e per verificare se la rana sta su di loro
					}
					else{ coccodrilli[temp.id].alive=1;}
						
				
			
			
			}	
			else{
				array_pid[temp.id]=temp.x; //salva l id del coccodrillo creato;	
			}
		
		}
		
		
		
		
		
	
		
		for(int i=0;i<3;i++){			//scorre per vedere se è uno dei 3 id possibili per le granate;
			if(temp.id==IDGRANATE+i){
				switch(i){
					case 0:
					case 1:
						if(temp.x==IDMORTE){
							granate[temp.id-IDGRANATE].alive=0;}    //vuoldire che è morta quella granata;
						
						else{
							granate[i].x=temp.x;	
							granate[i].y=temp.y;
							if(granate[i].x==proiettile.x && granate[i].y==proiettile.y && proiettile.alive==1){
								killProcess(array_pid[IDPROIETTILE]);		//killa proiettile;
								array_pid[IDPROIETTILE]=0;
								proiettile.alive=0;
								granate[i].alive=0;
								granata_morta=i;
								write(pipe2[1],&granata_morta,sizeof(int));}}	//avverte il processo granata che una delle due deve smettere di inviare la posizione;
								
						
						break;
					case 2: //id creazione granata;
						if(granate[0].alive==0 && granate[1].alive==0){	//devono essere entrambe morte;
						
							sparaGranata(rana.x,rana.y,vel_proiettile,array_pid,pipe1,pipe2);
							granate[0].alive=1;
							granate[1].alive=1;
							break;}
			
				}
			}
		}
		
		if(temp.id==IDPROIETTILE){	//non abbiamo bisogno di controllare se sbatte ai bordi perchè lo fa lui stesso;
			proiettile.x=temp.x;
			proiettile.y=temp.y;
			
			for(int i=0;i<2;i++){
				if(granate[i].alive==1 && granate[i].x==proiettile.x && granate[i].y==proiettile.y){			
							killProcess(array_pid[IDPROIETTILE]);		
							array_pid[IDPROIETTILE]=0;
							proiettile.alive=0;
							granate[i].alive=0;
							granata_morta=i;
							write(pipe2[1],&granata_morta,sizeof(int));	
				}
				
			}
			
			if(CollisioneRanaProiettile(rana,proiettile)==1){      //c'è stata collisione con proiettile
				game_struct->vite--;
				//assegno punti
				return;
			}
		}
		
	        if(temp.id==IDMORTE){		//usato per il proiettile;
			waitpid(array_pid[temp.x], NULL, 0);	
			array_pid[temp.x]=0;			//metto a 0 il pid visto che è morto;
			
		}
		
		
		if(granate[0].alive==0 && granate[1].alive==0 && array_pid[IDGRANATE]!=0){ //vuoldire che la funzione granata è terminata;
			killProcess(array_pid[IDGRANATE]);	
			array_pid[IDGRANATE]=0;
		}
		
		
		if(rana.y<40 && rana.x>=15 && proiettile.alive==0){	//funzione che fa sparare proiettili;
			gettimeofday(&end,NULL);	//se sto nel fiume aggiorno il timer;
			for(int i=0;i<MAX_CROCODILES;i++){
				if(coccodrilli[i].alive==1 && coccodrilli[i].y==rana.y && (coccodrilli[i].x+4<rana.x-1-10 && coccodrilli[i].dir==1) || (coccodrilli[i].x-4>rana.x+1+10 && coccodrilli[i].dir==-1))							{   //in pratica il coccodrillo deve essere vivo, nello stesso flusso della rana e essere dietro di lei nel flusso;(in questo caso ho messo la condizione che ci devono essere almeno 10 caratteri tra rana e coccodrillo;
					
					
					if(end.tv_sec - start.tv_sec>=random){
						if(rand_funz(1,5)==1){
							sparaProiettile(coccodrilli[i],vel_proiettile,array_pid,pipe1); //sarebbe da passargli solo quello in scrittura!!!
							proiettile.alive=1;
						}
						
						gettimeofday(&end,NULL);		
						gettimeofday(&start,NULL);
						random=rand_funz(2,4);	          //cambio intervallo di tempo;
					}
						
						
					
				}
			}
		}
				//in pratica il timer va a conteggiare il tempo anche quando siamo fuori dal fiume però a noi non ce ne frega niete😎️(si forse si è capito che ho scoperto come si mettono le emoji dal tasto destro);
		
		
		
		
		
		
		
		
		
		windowGeneration(game,COLS,LINES,game_struct);
		drawCoccodrilli(game,coccodrilli);
		draw_granate(game,granate);
		draw_proiettile(game,proiettile);
		draw_frog(game,rana);
		//print vite-score
		wattron(game, COLOR_PAIR(7));
 	        mvwprintw(game, 2, 2, "Punteggio: %d | Vite: %d",game_struct->score,game_struct->vite);
                wattroff(game, COLOR_PAIR(7));
		//print tempo
 	        wattron(game, COLOR_PAIR(7));
		mvwprintw(game, 46, 2, "Tempo: %d",game_struct->tempo);
   	        wattroff(game, COLOR_PAIR(7));
   		
   	
		wrefresh(game);
}}


