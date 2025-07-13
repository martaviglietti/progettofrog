#include"header.h"

//sprite rana
const char *rana_sprite[2] = {
    "* *",
    "   "
};



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
        key= (int)wgetch(finestra_gioco);
        pthread_mutex_unlock(&semaforo_disegno);
        
        
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
            	
            	
            
                break;
            default:
            
                break;
         
        }  
    }
}
