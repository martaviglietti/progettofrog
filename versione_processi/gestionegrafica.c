




void Gestione_grafica(WINDOW* game,int pipe1[], int pipe2[], int array_pid[],int vel_proiettile, Game_struct* game_struct){			
	
    //Variabili per gestione proiettile
    struct timeval start, end; 
    gettimeofday(&end,NULL);		
    gettimeofday(&start,NULL);
    long time_proiettile; 
    int random=rand_funz(2,4);

    int result; //contiene il risultato delle funzioni di collisione;	 
    Temp temp={-1,0,0,0};
    int b=0;

    //Inizializzazione degli elementi di gioco
    Coccodrillo coccodrilli[MAX_CROCODILES];
    for (int i=0; i<MAX_CROCODILES;i++) {
        coccodrilli[i].id=i;      //id coccodrillo
	coccodrilli[i].x=-1;	  //posizione x coccodrillo
	coccodrilli[i].y=-1;      //posizione y coccodrillo
	coccodrilli[i].dir=0;     //direzione coccodrillo
	coccodrilli[i].alive=0;   //stato del coccoddrillo
	coccodrilli[i].wait=-1;}  //variabile per stato di attesa

    Granata granate[2];
    for (int i=0; i<2;i++) {
	granate[i].id=i;
	granate[i].x=-1;
	granate[i].y=-1;
	granate[i].alive=0;}
		
    Rana rana;	
    rana.id=IDRANA;
    rana.x=40; 
    rana.y=43;

    Proiettile proiettili[15];
    for(int i=0;i<15;i++){
        proiettili[i].id=IDPROIETTILE+i;
        proiettili[i].x=-1;
        proiettili[i].y=-1;
        proiettili[i].alive=0;
    }
    
    
   
    int tempo=game_struct->tempo;
    int coc_scelto;  //variabile utilizzata per indicare su quale coccodrillo posa la rana
    
   
    int distanze_coc[8];
    
    for (int i=0; i<8;i++) {
	distanze_coc[i]=rand_funz(13,16);  //impostiamo le distanze iniziali tra coccodrilli per ogni flusso del fiume
    }
   
    
    int fine_manche=1;  //variabile utilizzata per terminare la manche
    int coccodrilloCheSpara;
    
    while (fine_manche) {
    	
	fine_manche=1;  //se durante il ciclo viene impostata a 0 allora si esce dal game
	temp.id=-1;
	
		//funzioni di gestione grafica del gioco
	    werase(game);
	    windowGeneration(game,COLS,LINES,game_struct);
	    drawCoccodrilli(game,coccodrilli);
	    draw_granate(game,granate);
	    draw_proiettile(game,proiettili);
	    draw_frog(game,&rana);
	    print_tempo(game,game_struct,tempo); //barra del tempo

	    
	    wrefresh(game);
	
	
        read(pipe1[0],&temp,sizeof(Temp));  //leggiamo il messaggio nella pipe
	
	//se l'ID è della rana						
	if (temp.id==IDRANA) {
	    if (RanaInFinestra(&rana,&temp)) {  //controllo per evitare che la rana fuoriesca dalla finestra di gioco
	        rana.x+=temp.x;
	        rana.y+=temp.y;	
	
		if (rana.y<10) {  //controllo se la rana si trova nella zona delle tane			
		    if (RanaSuTana(&rana,game_struct)) {  //controllo se la rana si trova su una delle tane        
		        game_struct->score+=15;
			game_struct->score+=(int)(15*(float)game_struct->tempo/100);
			fine_manche=0;						
		    } else {  //altrimenti la rana si trova nella zona circostante alle tane
		        game_struct->vite--;
			game_struct->score-=10; 
		        fine_manche=0;
		    }
		}
	    }  
        }
	
	//se l'id è del processo tempo
	if (temp.id==IDTIME) {	
	    game_struct->tempo-=1;
	    if (game_struct->tempo==0) {  //controllo per verificare se il tempo è terminato
	        game_struct->vite--;
		game_struct->score-=20;
		fine_manche=0;
	    }
	}

	
	//se l'ID è di un coccodrillo
	if (temp.id<MAX_CROCODILES && temp.id>=0) {
	    if (temp.y!=IDAGGIUNTAPID) {  //controlliamo se il messaggio proviene dal generatore dei coccodrilli
		coc_scelto=RanaSuCoccodrillo(&rana,coccodrilli);  //controllo di verifica che la rana sia già su un coccodrillo (qualsiasi) prima dello spostamento
		coccodrilli[temp.id].x=temp.x;
		coccodrilli[temp.id].y=temp.y;
	        coccodrilli[temp.id].dir=temp.info;  
		
		//controlli per lo spostamento della rana
		fine_manche=spostamento_ranaCoc(temp.id, coc_scelto,coccodrilli, &rana,game_struct, fine_manche);	
		
		//controllo sullo stato del coccodrillo
		controllo_stato_coccodrillo(temp.id,coccodrilli);
			
			
			
			
		//gestione dello stato di attesa 
		attesa_coccodrilli(temp.id, coccodrilli,distanze_coc,array_pid);
		riattivare_coccodrilli(coccodrilli, distanze_coc, array_pid);
		
		
				
			
		
	    } else {  //altrimenti aggiungiamo il pid del coccodrillo all'array dei pid
	        array_pid[temp.id]=temp.info; //salva l id del coccodrillo creato;	
	    }
	}
	
	


	//se l'id è delle granate
	if (temp.id>=IDGRANATE && temp.id<=IDGRANATE+2) {
	    if (temp.id==IDGRANATE+2) {
	        if (granate[0].alive==0 && granate[1].alive==0) {  //devono essere entrambe morte;		
	            sparaGranata(rana.x,rana.y,vel_proiettile,array_pid,pipe1,pipe2);
	            granate[0].alive=1;
                    granate[1].alive=1;
          
                    
	            
	        }
	    } else {
	        granate[temp.id-IDGRANATE].x=temp.x;	
		granate[temp.id-IDGRANATE].y=temp.y;
		
			   
	    }	  
	}
	
	
	
	
	//se l'id è del proiettile
	if (temp.id>=IDPROIETTILE && temp.id<IDPROIETTILE+15) {	
	    proiettili[temp.id].x=temp.x;
	    proiettili[temp.id].y=temp.y;
	}
	
	
	//controllo collisione proiettile-bordi
	collisione_ProiettileBordi(array_pid, proiettili);
	
	
	//controllo collisione granate/bordi
	collisione_GranataBordi(pipe2[1],granate);
	

	
	//controllo collisione rana-proiettile
	fine_manche=CollisioneRanaProiettile(&rana, proiettili,game_struct, fine_manche);   
	
	
	//controllo collisione granata-proiettile
	collisione_GranataProiettile(pipe2[1],granate, proiettili, array_pid,game_struct);
	
	
	//controllo per killare il processo granate una volta morte
	killa_granata(granate, array_pid);
	
	
	
    
    }
}


int spostamento_ranaCoc(int id, int coc_scelto, Coccodrillo* coccodrilli, Rana* rana, Game_struct* game_struct, int fine_manche ) {

    if (coc_scelto==coccodrilli[id].id) {  //controlliamo che il coccodrillo attuale sia anche quello su cui la rana era posata prima dello spostamento
		    if ((rana->x<=2 && coccodrilli[id].dir==-1) || (rana->x>=LARGHEZZA_GIOCO-3 && coccodrilli[id].dir==1)) {  //controllo se la rana si trova agli estremi della mappa			
			if (RanaSuCoccodrillo(rana,coccodrilli)!=coc_scelto) {	//se la rana non viene considerata più sopra l'attuale coccodrillo allora è caduta in acqua sul bordo della mappa 
			    game_struct->vite--;
			    game_struct->score-=10;
			    
			    return 0;
			}	
		    } else {	 //altrimenti la rana si muove con il coccodrillo (ovvero non si trovava agli estremi della mappa)					
		        rana->x+=coccodrilli[id].dir;
		    }               			
    }
    return fine_manche;

}

void killa_granata(Granata* granate, int array_pid[]){
    //controllo per killare il processo granate una volta morte
    if (granate[0].alive==0 && granate[1].alive==0 && array_pid[IDGRANATE]!=0) { 
	   
	killProcess(array_pid[IDGRANATE]);	
	array_pid[IDGRANATE]=0;
    }

}


void collisione_GranataProiettile(int pipe2, Granata* granate, Proiettile proiettili[], int array_pid[], Game_struct* game_struct){

    
	for (int i=0; i<2; i++) {
	    for(int j=0; j<15;j++){
	    
		    if (granate[i].x==proiettili[j].x && granate[i].y==proiettili[j].y && proiettili[j].alive==1 && granate[i].alive==1) {
			killProcess(array_pid[IDPROIETTILE+j]);  //killiamo il proiettile
			array_pid[IDPROIETTILE+j]=0;
			//impostiamo a morti il proiettile e la granata
			proiettili[j].alive=0;
			granate[i].alive=0;
			granate[i].x=-1;
			write(pipe2,&i, sizeof(int));  //avverte il processo granata che una delle granate è morta
			game_struct->score+=5;
	 	    }
	    }
	}


}


void collisione_ProiettileBordi(int array_pid[], Proiettile proiettili[]){
    for(int i=0; i<15; i++){
    
	    if((proiettili[i].x<1 || proiettili[i].x>LARGHEZZA_GIOCO-2) && proiettili[i].alive==1){
		
			killProcess(array_pid[IDPROIETTILE+i]);
			array_pid[IDPROIETTILE+i]=0;
			proiettili[i].alive=0;
		}

    }
}

void collisione_GranataBordi(int pipe2, Granata* granate){
    
    
    for (int i=0; i<2;i++) {
	if((granate[i].x<1 || granate[i].x>LARGHEZZA_GIOCO-2) && granate[i].alive==1 && granate[i].x!=-1) {
	    granate[i].alive=0;
	    granate[i].x=-1;  
	    write(pipe2,&i, sizeof(int));  //scriviamo al processo granate quale delle due granate è morta
	    	
	}
    }

}




//funzione che controlla se la rana si trova in mappa
int RanaInFinestra(Rana* rana, Temp* temp){

    if(rana->x+temp->x>78) return 0;
    if(rana->x+temp->x<2) return 0;
    if(rana->y+temp->y>43) return 0;
    if(rana->y+temp->y<7) return 0;
    return 1;

}

//funzione di controllo collisione rana-proiettile
int CollisioneRanaProiettile(Rana*rana,Proiettile proiettili[], Game_struct* game_struct, int fine_manche){

    for(int i=0;i<15;i++){
    
    
    
	    if((rana->x+1==proiettili[i].x || rana->x-1==proiettili[i].x) && rana->y==proiettili[i].y && proiettili[i].alive) {
		game_struct->vite--;
		game_struct->score-=15;
		return 0;
	    }
    }
    
    return fine_manche;
       
}

//funzione di controllo se la rana è su una tana
int RanaSuTana(Rana* rana, Game_struct* game_struct){
	 
    for (int i=0;i<5;i++) {
        if (rana->x>=8+(15)*i && rana->x<8+(15)*i+5 && game_struct->tane[i]==0) {
   	    game_struct->tane[i]=1;
 	    return 1;
 	}
    }
    return 0;
}
	 	
	 	
	 		




//funzione che verifica se la rana è su un coccodrillo
int RanaSuCoccodrillo(Rana* rana, Coccodrillo *coccodrilli){
    for (int i = 0; i < MAX_CROCODILES; i++) {
        if (coccodrilli[i].alive && rana->y == coccodrilli[i].y &&  rana->x-1>= coccodrilli[i].x-4 && rana->x+1 <= coccodrilli[i].x+4) {
            
            return coccodrilli[i].id; 
            
        }
    }
    return -1; 
}










