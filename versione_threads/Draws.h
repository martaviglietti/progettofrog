#ifndef DRAWS_H
#define DRAWS_H

#include "main.h"
#include "FrogHandle.h"
#include "CrocHandle.h"
#include "Partita.h"
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


void generatore_finestra(WINDOW *finestra_gioco, int maxX, int maxY, Statistiche * statistiche_gioco);
void gestore_grafica(WINDOW* finestra_gioco,int velocità_proiettili, Statistiche * statistiche_gioco, Thread_id thread_id[]);
void creazione_colori();
void disegna_proiettile(WINDOW* finestra_gioco, Proiettile proiettile[]);
void disegna_granate(WINDOW* finestra_gioco, Granata granate[2]);
void disegna_rana(WINDOW *finestra_gioco, Rana* rana);
void disegna_coccodrilli(WINDOW *finestra_gioco, Coccodrillo *coccodrilli);
void barra_tempo(WINDOW* finestra_gioco,Statistiche * statistiche_gioco, int tempo);  

#endif