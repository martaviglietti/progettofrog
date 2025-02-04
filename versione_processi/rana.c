#include "header.h"
void frog(WINDOW* game,int pipe1[2]) {
    close(pipe1[0]);
    int x = 40, y = 43; // Posizione iniziale della rana (nel marciapiede)
    int key;
    Temp rana={IDRANA,x,y,0};      
    Temp granata={IDGRANATE+2,0,0,0}; //usiamo per chiedere alla render di creare granata
    // Abilita l'input da tastiera;
    keypad(game, true);
    

    while (1) {
    
        // Cattura l'input da tastiera
        key = (int)wgetch(game);
        

        switch (key) {
            case KEY_UP:
                if(rana.y>7) rana.y-=3;break;		//7 è l'altezza dove si trova la rana quando entra in una tana(noi non vogliamo che vada oltre);
                	
            case KEY_DOWN:
                if(rana.y<43) rana.y+=3;break; 		//43 è la posizione più in basso per la rana; 
  
               
            case KEY_LEFT:
                if(rana.x>3) rana.x-=2;else if(rana.x==3) rana.x-=1;		
                break;
            case KEY_RIGHT:
                if(rana.x<76) rana.x+=2;else if(rana.x==76) rana.x+=1; 		
                break;
            case 's': 
            	
            	write(pipe1[1],&granata,sizeof(Temp));   //mando un messaggio alla render con un id che gli fa capire che voglio creare la granata;(sarà la render a decidere se crearla o no!!;
                
                break;
         
        }


        // Invia posizione aggiornata al processo principale tramite pipe
        write(pipe1[1], &rana, sizeof(Temp));
       
        
    }
}


void sparaGranata(int startX, int startY,int velocità_proiettile ,int pid_array[],int pipe1[], int pipe2[]) {
    pid_array[IDGRANATE]= fork(); // Crea un nuovo processo per la granata

    if (pid_array[IDGRANATE] < 0) {
        perror("Errore nella creazione del processo granata");
        exit(0);
    }

    if (pid_array[IDGRANATE] == 0) {
        // Processo figlio: gestisce la granata
        granata(startX, startY,velocità_proiettile, pipe1,pipe2);
    }
}



void granata(int startX, int startY, int velocità_proiettile, int pipe1[], int pipe2[]) {		//posizione della rana;
    						//facciamo partire dal basso i proie#include <sys/time.h>ttili essendo la bocca della rana e del coccodrillo nella parte bassa;
    close(pipe2[1]);
    close(pipe1[0]);
    Temp granate[2];
    int granata_morta;	//serve per ricevere messaggio dalla render;
    
    int alive_sinistra=1;
    int alive_destra=1;
    granate[0].id=IDGRANATE; 
    granate[1].id=IDGRANATE + 1;
    granate[0].x=startX - 2;	//le granate partono dal carattere appena esterno a quelli della rana;
    granate[1].x=startX + 2; 				
    //verificare se si bugga sul muro
    granate[0].y=startY-1;
    granate[1].y=startY-1;   
    granate[0].info=0;
    granate[1].info=0;
    

    while (true) {
    
        if(read(pipe2[0],&granata_morta,sizeof(int))!=-1){ 		//read non bloccante;
        	if(granata_morta==0) alive_sinistra=0;					//se il main riconosce una collisione manda il messaggio al processo granate che disattiva l'invio delle coordinate;
		else alive_destra=0;
        	
        } //se 0 è morta a sinistra se 1 a destra; 
         
        if(granate[1].x>78 && alive_destra){       			
        	alive_destra=0;
        	granate[1].id=IDGRANATE+1;
    		granate[1].x=IDMORTE;
    		write(pipe1[1], &granate[1], sizeof(Temp));  //avvisa della morte;
        }else if(alive_destra){
        	write(pipe1[1],&granate[1],sizeof(Temp));	 //manda coordinate;
        	granate[1].x++;}
        	
        if(granate[0].x<1 && alive_sinistra){
        	alive_sinistra=0;
        	granate[0].id=IDGRANATE;
    		granate[0].x=IDMORTE;
    		write(pipe1[1], &granate[0], sizeof(Temp));  //avvisa della morte;
        }else if(alive_sinistra){
        	write(pipe1[1],&granate[0],sizeof(Temp));
        	granate[0].x--;}
        
        usleep(velocità_proiettile); // Velocità del movimento   
    }
    
    _exit(0);
}