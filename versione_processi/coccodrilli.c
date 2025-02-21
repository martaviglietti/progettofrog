#include "header.h"
#include <stdbool.h>
#include <unistd.h> 
void funzione_gestione_coccodrilli(Flusso *flussi,int pipe1[]){
	int n_coc_alive=0;
	int n_flusso;
	int flusso_scelto;
	int indici_minimi[8];
	int minor_coccodrilli;
	int n_coc_flusso[8];
	int contatore;
	Temp messaggio={0,0,0,0};
	pid_t pid_coc;
	Temp coccodrilli[MAX_CROCODILES];

	for(int i=0; i<MAX_CROCODILES;i++){
		minor_coccodrilli=20;
		contatore=0;
		for(int i=0;i<8;i++){
			n_coc_flusso[i]=0;
		}
	   	for (int i = 0; i < 8; i++) {
			if (n_coc_flusso[i] < minor_coccodrilli) {		
		    		minor_coccodrilli = n_coc_flusso[i];
		}}
		
		for(int i=0;i<8;i++){							
			if(n_coc_flusso[i]== minor_coccodrilli){
				indici_minimi[contatore++]=i;}
		}
	       	
		flusso_scelto=indici_minimi[rand()%contatore];
   		coccodrilli[i].id=i;
   		coccodrilli[i].info=0;
   		coccodrilli[i].y=flussi[flusso_scelto].y;
   		n_coc_flusso[flusso_scelto]++;
   		if(flussi[flusso_scelto].dir==1){
   			coccodrilli[i].x=POS_SPAWN_COC_SINISTRA;   
   		}
   		else{
   			coccodrilli[i].x=POS_SPAWN_COC_SINISTRA;
   		}
   		
   		pid_coc=fork();
		if(pid_coc==-1){
			perror("Errorre nella fork coccodrillo: ");
			exit(1);
		}
		else if(pid_coc==0){
   			funzione_coccodrillo(coccodrilli[i],flussi,flusso_scelto,pipe1);
		}  
   		else{
   			messaggio.x=pid_coc;						//imposto il messsaggio fa inviare
   			messaggio.id=i;
   			messaggio.y=IDAGGIUNTAPID;                            //serve per far capire che è un nuovo pid da aggiungere nella lista dei pid;      
   		
   			write(pipe1[1], &messaggio,sizeof(Temp));                          //invio messaggio alla principale con pid del coc creato       
   			usleep(rand_funz(20000,40000));
		}
   		if(i== (MAX_CROCODILES/2 -1)){
   			usleep(rand_funz(3000000,4000000));				//a metà della creazione dei coc fermo la loro generazione per qualche secondo;
   		}
   		
   	} 
}	
  //aggiornato
  
  
void funzione_coccodrillo(Temp coccodrillo,Flusso flussi[8],int id_flusso_scelto, int pipe1[]){
	close(pipe1[0]);
	srand(time(NULL)+coccodrillo.id);
	Flusso flusso_scelto=flussi[id_flusso_scelto];									//serve per salvare il flusso scelto per ogni respawn;
	
	
	while(true){
	
		coccodrillo.x=coccodrillo.x+flusso_scelto.dir;
		coccodrillo.info=flusso_scelto.dir;
		write(pipe1[1], &coccodrillo, sizeof(Temp));		//invia la prima posizone quando il primo carattere del coccodrillo è nello schermo;
		
		usleep(flusso_scelto.speed);
		
		if(coccodrillo.x>=POS_SPAWN_COC_DESTRA && flusso_scelto.dir==1 || (coccodrillo.x<=POS_SPAWN_COC_SINISTRA && flusso_scelto.dir==-1)){
			
			flusso_scelto=flussi[rand()%8];
			while(coccodrillo.y==flusso_scelto.y){
				flusso_scelto=flussi[rand()%8];
			}
			coccodrillo.y=flusso_scelto.y;
			if(flusso_scelto.dir==1){
			coccodrillo.x=POS_SPAWN_COC_SINISTRA;}
			else {coccodrillo.x=POS_SPAWN_COC_DESTRA;}
			usleep(rand_funz(2000000,3000000));}
			
}}

//agg

void funz_proiettile(Coccodrillo coccodrillo,int vel,int pipe1[]){   //vediamo per le pipe;
	close(pipe1[0]);
	Temp proiettile={0,0,0,0};
	int segnale;
	if(coccodrillo.dir==1) proiettile.x=coccodrillo.x+5;
	else                   proiettile.x=coccodrillo.x-5;	
	
	
	proiettile.y=coccodrillo.y-1;		//perchè lo spariamo dal basso;
	proiettile.id= IDPROIETTILE;
	
	while(true){
		if(proiettile.x>78 || proiettile.x<1){
			proiettile.id=IDMORTE;		
			proiettile.x=IDPROIETTILE;
			write(pipe1[1], &proiettile, sizeof(Temp));  //avvisa della morte;
			break;}
		else{
		
			write(pipe1[1], &proiettile, sizeof(Temp));  //coordinate proiettile alla render;			
			proiettile.x+=coccodrillo.dir;
			usleep(vel);}
		
		
	}
	_exit(0);
}
//agg



void sparaProiettile(Coccodrillo coccodrillo,int velocità_proiettile, int pid_array[], int pipe1[]) {
    pid_array[IDPROIETTILE]= fork(); // Crea un nuovo processo per la granata

    if (pid_array[IDPROIETTILE] < 0) {
        perror("Errore nella creazione del processo proiettile");
        return;
    }

    if (pid_array[IDPROIETTILE] == 0) {
   
        funz_proiettile(coccodrillo,velocità_proiettile,pipe1);
    }
}

	
void tempo(int* pipe1){
	close(pipe1[0]);
	Temp tempo={0,0,0,0};		
	tempo.id=IDTIME;
	while(true){
		sleep(1);
		write(pipe1[1], &tempo, sizeof(Temp)); //manda segnale alla main
	}
	
	_exit(0);
}
	

   
void killProcess(pid_t pid) {
    int status;
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
}

void kill_processi(pid_t pid[N_PID]){
	
	for(int i=0; i<N_PID;i++){
		if(pid[i]>0){
			if(kill(pid[i],SIGKILL)!=0){
				perror("Error killing process");
			}
			else {waitpid(pid[i], NULL, 0);
			      pid[i]=0;}
		}		
	
	}
}
   	


void def_vel_flussi(Flusso *flussi, int vel){
	
	srand(time(NULL));
	int altezza_base=37;				//posizione verticale di ogni flusso (apparte l ultimo) è la posizione in alto  quindi la terza fila= y di riferimento dei coccodrilli e anche quello dove arriva la rana saltando;
	int dist_flussi=3;
	for(int i=0;i<8;i++){
    		flussi[i].y=altezza_base - i*dist_flussi;
    		flussi[i].speed= rand_funz(vel - 10000,vel+10000);
	}
}


void def_dir_flussi(Flusso *flussi){
	for(int i=0;i<8;i++){
   		 if(i>=1){
   		     flussi[i].dir= -flussi[i-1].dir;
        
  		  }else{
  		  flussi[i].dir= rand()%2;
  		  if(flussi[i].dir==0) flussi[i].dir=1; else flussi[i].dir=-1;
  		  }
}	
}