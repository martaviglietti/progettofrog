#ifndef FROG_HANDLE_H
#define FROG_HANDLE_H
#include "main.h"

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
#include "menu.h"
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#define ALTEZZA_RANA 2
#define LARGHEZZARANA 3


void* funzione_rana(void* parametri_thread);
void spara_granata(int inizioX, int inizioY,int velocità_proiettili ,Thread_id thread_id[]);
void* funzione_granata(void* parametri_thread);
int rana_in_finestra(Rana* rana, Temp*temp);
void uccidi_granate(Granata* granate, int array_pid[]);

#endif