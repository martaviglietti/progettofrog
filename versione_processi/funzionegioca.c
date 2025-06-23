#include "header.h"







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
    int tane_occupate=0;
    Statistiche  statistiche_gioco;
    statistiche_gioco.punteggio=0; 		    //tiene conto del punteggio di gioco
    statistiche_gioco.vite=parametri_gioco.vite;    //tiene conto del numero di vite rimaste
    statistiche_gioco.tempo=parametri_gioco.tempo;  //tiene conto del tempo rimasto nella manche
   
    for (int i=0;i<TANE-1;i++) {
        statistiche_gioco.tane[i]=0;  //impostiamo come 'aperte' tutte le tane
    }
    
    Flusso flussi[8];
    velocità_flussi(flussi,parametri_gioco.velocità_coccodrilli);  //definiamo la velocità di ogni flusso                
    int array_pid[NUMERO_PID];  //array contenente i pid dei processi creati (utilizzato per uccidere o mettere in pausa i processi) 
    
   
    
    while (true) {
        
        direzione_flussi(flussi);  //definiamo la direzione di ogni flusso					
    	creazione_processi(flussi,array_pid, pipe1,finestra_gioco, &parametri_gioco);  //creiamo tutti i processi di gioco
	gestore_grafica(finestra_gioco,pipe1,pipe2,array_pid,parametri_gioco.velocità_proiettili,&statistiche_gioco);  //funzione di gestione grafica (gestisce collisioni e la grafica)
    	statistiche_gioco.tempo=parametri_gioco.tempo;  //riportiamo a default il tempo di gioco 	
	svuota_pipe(pipe1[0]);  //svuotiamo la pipe per eliminare eventuali elementi rimasti all'interno
	
        
        //uccidiamo tutti i processi
   	kill_processi(array_pid);		
        
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






//funzione che restituisce un numero numero_random tra due estremi (compresi), di tipo float
float numero_random_float(float minimo, float massimo) {
    return minimo + ((float)rand() / (float)RAND_MAX) * (massimo - minimo);
}


//funzione che svuota la pipe 
void svuota_pipe(int pipe_fd){
	
    int a;
    while (read(pipe_fd,&a,sizeof(int))!= -1) {  //leggiamo dalla pipe finchè non è vuota
	
    } 
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
	    array_pid[NUMERO_COCCODRILLI]=fork();  //creiamo il processo che gestisce la creazione dei coccodrilli
	    if (array_pid[NUMERO_COCCODRILLI]==-1) {
		perror("Errore nella fork del generatore dei coccodrilli: ");
		exit(1);
	    } else if (array_pid[NUMERO_COCCODRILLI]==0) {
	        funzione_gestione_coccodrilli(flussi,pipe1, parametri_gioco);
	        exit(1);
	    } else {
                return;
	    }
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
	
    for (int i=24; i<NUMERO_PID;i++) {
      
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
