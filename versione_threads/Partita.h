#ifndef PARTITA_H
#define PARTITA_H
#include "main.h" 




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