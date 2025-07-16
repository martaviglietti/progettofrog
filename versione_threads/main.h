#ifndef MAIN_H
#define MAIN_H

#include "FrogHandle.h"
#include "CrocHandle.h"
#include "Partita.h"
#include "Draws.h"
#include "collisioni.h"

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


#define LARGHEZZA_GIOCO 81
#define ALTEZZA_GIOCO 49
#define NUMERO_PROIETTILI 15
#define DELAY 100000
#define TANE 6
#define FIUME 25
#define PRATO 5
#define SPONDA_SUPERIORE 5
#define NUMERO_PID 100
#define DIM_BUFFER 100
// ID entità
#define ID_TIME 45
#define ID_COCCODRILLI 0
#define ID_RANA 50
#define ID_GRANATE 60
#define ID_PROIETTILE 70
#define ID_AGGIUNTAPID 30
#define ID_MORTE -10
#define ID_RICHIESTA 90



// Variabili globali condivise
extern Temp buffer[DIM_BUFFER];
extern int indice_scrittura;
extern int indice_lettura;

extern pthread_mutex_t semaforo_buffer;
extern pthread_mutex_t semaforo_disegno;

extern sem_t semafori_coccodrilli[NUMERO_COCCODRILLI];
extern sem_t spazi_occupati;
extern sem_t spazi_liberi;

extern int fine_gioco;

// opzioni del menu
static const char *opzioni[] = {
    "Inizia game", "Crediti", "Esci"
};

//sprite rana
const char *rana_sprite[2] = {
    "* *",
    "   "
};

const char *coccodrillo_sprite[2][2]={{"///////","XXXXXX0X0"},{"XXXXXXX","0X0XXXXXX"}};
            

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
	int x;
	int y;
	int info;
}Temp;


typedef struct{
    pthread_t id;   // ID del thread
    int valido;
}Thread_id;

void* funzione_tempo();
int menu(WINDOW *finestra_gioco, const char *title, const char *options[], int num_options);
void crediti(WINDOW *finestra_gioco);



#endif
