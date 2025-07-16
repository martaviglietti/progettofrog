#ifndef PARTITA_H
#define PARTITA_H

#include "main.h"
#include "FrogHandle.h"
#include "CrocHandle.h"
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




int game_vinto(WINDOW *finestra_gioco, int punteggio);
int game_perso(WINDOW *finestra_gioco, int punteggio);
int scelta_difficoltà(WINDOW *finestra_gioco);
Statistiche Partita(WINDOW *finestra_gioco,Parametri parametri_gioco);
int numero_random(int min, int max);  
void punteggio_tempo(Statistiche * statistiche_gioco); 

void impostazioni_gioco(Parametri* parametri_gioco, int difficoltà );
float numero_random_float(float minimo, float max);
void scrittura_buffer(Temp messaggio);
Temp lettura_buffer();
void cancel_thread(Thread_id thread_id[]);

// Funzioni del buffer
void scrittura_buffer(Temp messaggio);
Temp lettura_buffer();
void svuota_buffer();
#endif