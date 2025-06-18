#include "header.h"
void frog(WINDOW* game,int pipe1[2]){
    close(pipe1[0]);
    
    int key;
    Temp rana={IDRANA,0,0,0};  //messaggio usato per inviare alla funzione di controllo i movimenti della rana     
    Temp granata={IDGRANATE+2,0,0,0};  //messaggio con id apposito per la creazione delle granate
    keypad(game, true);
    

    while (true) {
    
        //Cattura l'input da tastiera
        key = (int)wgetch(game);
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
void sparaGranata(int startX, int startY,int velocità_proiettile ,int pid_array[],int pipe1[], int pipe2[]){

    pid_array[IDGRANATE]= fork(); //Crea un nuovo processo per la granata

    if (pid_array[IDGRANATE] < 0) {
        perror("Errore nella creazione del processo granata");
        exit(0); 
    }

    if (pid_array[IDGRANATE] == 0) {  
        
        granata(startX, startY,velocità_proiettile, pipe1,pipe2);  //chiamiamo la funzione granata all'interno del processo figlio
    }
}

//funzione che gestisce il processo granata
void granata(int startX, int startY, int velocità_proiettile, int pipe1[], int pipe2[]){		
    						
    close(pipe2[1]);
    close(pipe1[0]);
    Temp granate[2];
    int messaggio;
    //variabili che gestiscono lo stato delle granate
    int alive_destra=1;
    int alive_sinistra=1;
   
    //impostazione iniziale delle granate (id e posizione di partenza)
    granate[0].id=IDGRANATE; 
    granate[1].id=IDGRANATE + 1;
    granate[0].x=startX - 2;
    granate[1].x=startX + 2; 			
    granate[0].y=startY;
    granate[1].y=startY;   
    granate[0].info=0;
    granate[1].info=0;
    
    svuota_pipe(pipe2[0]);
    

    while (true) {
    	
    	
    	while(read(pipe2[0],&messaggio,sizeof(int))!=-1){  //leggiamo se ci sono messaggi dalla funzioni di controllo per granate morte
            if (messaggio){
                
                alive_destra=0;
            } else { 
                alive_sinistra=0;
                	
            }
        }
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




