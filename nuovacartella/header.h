#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <locale.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#define NUMERO_COCCODRILLI 24
#define LARGHEZZA_GIOCO 81
#define ALTEZZA_GIOCO 49
#define SPAWN_SX_COCCODRILLO -4
#define SPAWN_DX_COCCODRILLO 84
#define NUMERO_PROIETTILI 15
#define ALTEZZA_RANA 2
#define LARGHEZZARANA 3
#define ALTEZZA_COCCODRILLO 2
#define LARGHEZZA_COCCODRILLO 9
#define TANE 6
#define FIUME 25
#define PRATO 5
#define SPONDA_SUPERIORE 5
#define NUMERO_TID 100
#define DIM_BUFFER 100
//definizione id da usare;
#define ID_TIME 45
#define ID_COCCODRILLI 0
#define ID_RANA 50
#define ID_GRANATE 60
#define ID_PROIETTILE 70


// opzioni del menu
static const char *opzioni[] = {
    "Inizia game", "Crediti", "Esci"
};

//sprite rana
const char *rana_sprite[2] = {
    "* *",
    "   "
};

const char *coccodrillo_sprite[2][2]={{"XXXXXXX","XXXXXX0X0"},{"XXXXXXX","0X0XXXXXX"}};
            


typedef struct{
    int id;
    int y;
    int dir;
    int velocità;
}Flusso;


typedef struct{
    int vite;
    int punteggio;
    int tempo;
    int tane[5];
    int gioco;  //la utilizziamo come condizione di uscita dal gioco;
}Statistiche;

typedef struct{
    int livello_difficoltà;
    int vite;
    int tempo;
    int velocità_proiettili;
    int velocità_coccodrilli;
    
 
}Parametri;

typedef struct{
    int id;
    int y;
    int x;
    int dir;
    int vivo;
    int attesa;
}Coccodrillo;

typedef struct{
    int id;
    int y;
    int x;
    int vivo;
}Proiettile;

typedef struct{
    int id;
    int y;
    int x;
    int vivo;
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
	int info;
}Temp;


typedef struct{

    pthread_t id;   // Id del thread
    int valido;


}Thread_id;


typedef struct{
	Temp coccodrillo;
	Flusso* flussi;
	int id_flusso;
	Parametri* parametri_gioco;
	


}Parametri_coccodrillo;

typedef struct{

        int id;
	Coccodrillo coccodrillo;
	int velocità_proiettili;
	
}Parametri_proiettile;

typedef struct{

	int x;
	int y;
	int velocità_granata;
	
}Parametri_granata;


int game_vinto(WINDOW *finestra_gioco, int punteggio);
int game_perso(WINDOW *finestra_gioco, int punteggio);
int scelta_difficoltà(WINDOW *finestra_gioco);
void generatore_finestra(WINDOW *finestra_gioco, Statistiche * statistiche_gioco);
Statistiche Partita(WINDOW *finestra_gioco,Parametri parametri_gioco);
void gestore_grafica(WINDOW* finestra_gioco,int velocità_proiettili, Statistiche * statistiche_gioco, Thread_id thread_id[]);
int collisione_rana_proiettili(Rana* rana,Proiettile proiettile[], Statistiche * statistiche_gioco, int gioca);
int rana_su_tana(Rana*rana, Statistiche * statistiche_gioco);
int rana_su_coccodrillo(Rana *rana, Coccodrillo *coccodrilli);
void creazione_processi(Flusso *flussi, int array_pid[NUMERO_TID],WINDOW* finestra_gioco, Parametri* parametri_gioco);
void* funzione_gestione_coccodrilli(Flusso *flussi, Parametri* parametri_gioco, Thread_id thread_id[], Parametri_coccodrillo parametri_coccodrillo[]);
void* funzione_coccodrillo(void* parametri_thread);
void* funzione_proiettile(void* parametri_thread);
void* funzione_tempo();

void velocità_flussi(Flusso *flussi, int velocità_coccodrillo);
void direzione_flussi(Flusso *flussi);
int menu(WINDOW *finestra_gioco, const char *title, const char *options[], int num_options);
void crediti(WINDOW *finestra_gioco);
int numero_random(int min, int max);
void* funzione_rana(void* parametri_thread);
void spara_granata(int inizioX, int inizioY,int velocità_proiettili ,Thread_id thread_id[]);
void spara_proiettile(int id, int identificatore_coc, Coccodrillo* coccodrilli,int velocità_proiettili, Thread_id thread_id[]);
void* funzione_granata(void* parametri_thread);
void creazione_colori();
void disegna_proiettile(WINDOW* finestra_gioco, Proiettile proiettile[]);
void disegna_granate(WINDOW* finestra_gioco, Granata granate[2]);
void disegna_rana(WINDOW *finestra_gioco, Rana* rana);
void disegna_coccodrilli(WINDOW *finestra_gioco, Coccodrillo *coccodrilli);
int rana_in_finestra(Rana* rana, Temp*temp);

void barra_tempo(WINDOW* finestra_gioco,Statistiche * statistiche_gioco, int tempo);    
void punteggio_tempo(Statistiche * statistiche_gioco);         
void collisione_granate_confine( Granata* granate);
void collisione_proiettili_confine(Thread_id thread_id[], Proiettile proiettile[]);
void collisione_granate_proiettili(Granata* granate, Proiettile proiettili[], Thread_id thread_id[], Statistiche * statistiche_gioco);
void uccidi_granate(Granata* granate, Thread_id thread_id[]);
void riattivazione_coccodrilli(Coccodrillo* coccodrilli, int distanze_coccodrilli[]);
void attesa_coccodrilli(int id, Coccodrillo* coccodrilli, int distanze_coccodrilli[]);
void controllo_stato_coccodrillo(int id,Coccodrillo* coccodrilli);
int movimento_rana_su_coccodrillo(int id, int coccodrillo_scelto, Coccodrillo* coccodrilli, Rana* rana, Statistiche * statistiche_gioco , int gioca);
void impostazioni_gioco(Parametri* parametri_gioco, int difficoltà );
float numero_random_float(float minimo, float max);
void controllo_sparo_proiettile(Thread_id thread_id[], Coccodrillo* coccodrilli,Rana* rana, Proiettile* proiettili, struct timeval* inizio, struct timeval* fine, float* numero_randomico, int velocità_proiettili);
void scrittura_buffer(Temp messaggio);
Temp lettura_buffer();
void cancel_thread(Thread_id thread_id[]);


Temp buffer[DIM_BUFFER];

int indice_scrittura=0;
int indice_lettura=0;
pthread_mutex_t semaforo_buffer;
pthread_mutex_t semaforo_disegno;
sem_t semafori_coccodrilli[NUMERO_COCCODRILLI]; //creo un mutex per ogni coccodrillo per gestire le pause
sem_t sem_posti_occupati;   // conta gli elementi presenti
sem_t sem_posti_liberi;

