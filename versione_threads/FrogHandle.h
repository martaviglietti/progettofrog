#ifndef FROG_HANDLE_H
#define FROG_HANDLE_H

#define ALTEZZA_RANA 2
#define LARGHEZZARANA 3
#include "main.h"
#endif

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

	int x;
	int y;
	int velocità_granata;
	
}Parametri_granata;

void* funzione_rana(void* parametri_thread);
void spara_granata(int inizioX, int inizioY,int velocità_proiettili ,Thread_id thread_id[]);
void* funzione_granata(void* parametri_thread);
int rana_in_finestra(Rana* rana, Temp*temp);
void uccidi_granate(Granata* granate, int array_pid[]);