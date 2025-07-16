#ifndef CROC_HANDLE_H
#define CROC_HANDLE_H

#include "main.h"
#include "FrogHandle.h"
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

#define NUMERO_COCCODRILLI 24
#define SPAWN_SX_COCCODRILLO -4
#define SPAWN_DX_COCCODRILLO 84
#define ALTEZZA_COCCODRILLO 2
#define LARGHEZZA_COCCODRILLO 9

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


void* funzione_gestione_coccodrilli(Flusso *flussi, Parametri* parametri_gioco, Thread_id thread_id[], Parametri_coccodrillo parametri_coccodrillo[]);
void* funzione_coccodrillo(void* parametri_thread);
void* funzione_proiettile(void* parametri_thread);

//funzioni flussi
void velocità_flussi(Flusso *flussi, int velocità_coccodrillo);
void direzione_flussi(Flusso *flussi);
void spara_proiettile(int id, int identificatore_coc, Coccodrillo* coccodrilli,int velocità_proiettili, Thread_id thread_id[]);
void uccidi_granate(Granata* granate, int array_pid[]);
void riattivazione_coccodrilli(Coccodrillo* coccodrilli, int distanze_coccodrilli[]);
void attesa_coccodrilli(int id, Coccodrillo* coccodrilli, int distanze_coccodrilli[]);
void controllo_stato_coccodrillo(int id,Coccodrillo* coccodrilli);
void controllo_sparo_proiettile(Thread_id thread_id[], Coccodrillo* coccodrilli,Rana* rana, Proiettile* proiettili, struct timeval* inizio, struct timeval* fine, float* numero_randomico, int velocità_proiettili);

#endif