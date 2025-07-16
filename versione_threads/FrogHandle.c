#include<FrogHandle.h>
#include "main.h"
#include "Partita.h"
#include "CrocHandle.h"

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


//funzione che gestisce il processo granata
void* funzione_granata(void* parametri_thread){		
    						
    Parametri_granata* param= (Parametri_granata*) parametri_thread;
    int velocità= param->velocità_granata;
    Temp granate[2];
    int messaggio;  //variabile per leggere il messaggio dal gestore grafico
    
    //variabili che gestiscono lo stato delle granate
    int alive_destra=1;
    int alive_sinistra=1;
   
    //impostazione iniziale delle granate (id e posizione di partenza)
    granate[0].id=ID_GRANATE; 
    granate[1].id=ID_GRANATE + 1;
    granate[0].x=param->x - 2;
    granate[1].x=param->x + 2; 			
    granate[0].y=param->y;
    granate[1].y=param->y;   
    granate[0].info=0;
    granate[1].info=0;
    
    
    
    while (true) {
       
        if(granate[1].x>79 && alive_destra){       			
        	alive_destra=0;
        	scrittura_buffer(granate[1]);
    		
        }else if(alive_destra){
        	scrittura_buffer(granate[1]);
        	granate[1].x++;}
        	
        if(granate[0].x<1 && alive_sinistra){
        	alive_sinistra=0;
    		scrittura_buffer(granate[0]);  //avvisa della morte;
        }else if(alive_sinistra){
        	scrittura_buffer(granate[0]);
        	granate[0].x--;
        }
        	         
        usleep(velocità); // Velocità del movimento   
    }	       
        
}



void* funzione_rana(void* parametri_thread){
    WINDOW* finestra_gioco = (WINDOW *)parametri_thread;
    nodelay(finestra_gioco,true);
    int key;
    Temp rana={ID_RANA,0,0,0};  //messaggio usato per inviare alla funzione di controllo i movimenti della rana     
    Temp granata={ID_GRANATE+2,0,0,0};  //messaggio con id apposito per la creazione delle granate
    keypad(finestra_gioco, true);
    

    while (true) {
        //cattura l'input da tastiera
        pthread_mutex_lock(&semaforo_disegno);
        pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, &semaforo_disegno);

        key = (int)wgetch(finestra_gioco);

        pthread_cleanup_pop(1); // 1 => esegue pthread_mutex_unlock(&semaforo_disegno)
        
        rana.x=0;
        rana.y=0;

        switch (key) {
            case KEY_UP:  //movimento verso l'alto
                rana.y=-3;
                scrittura_buffer(rana);
                break;		
                	
            case KEY_DOWN:  //movimento verso il basso
                rana.y=3;
                scrittura_buffer(rana);
                break; 		
  
               
            case KEY_LEFT:  //movimento verso sinistra
                rana.x=-2;
                scrittura_buffer(rana);	
                break;
                
            case KEY_RIGHT:  //movimento verso destra
                rana.x=2;
                scrittura_buffer(rana);		
                break;
            case 's':  //tasto per sparare le granate 
            	scrittura_buffer(granata);
            	
          
                break;
     
        }
        
       usleep(5000);
    }
}