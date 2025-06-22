#ifndef FROGGER_THREADS_H
#define FROGGER_THREADS_H
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
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

#define BUFFER_SIZE 200
#define MAX_CROCODILES 24
#define LARGHEZZA_GIOCO 81
#define ALTEZZA_GIOCO 49
#define POS_SPAWN_COC_SINISTRA -4
#define POS_SPAWN_COC_DESTRA 84
#define ALTEZZARANA 2
#define LARGHEZZARANA 3
#define ALTEZZACOCCODRILLO 2
#define LARGHEZZACOCCODRILLO 9
#define DELAY 100000
#define TANE 6
#define FIUME 25
#define PRATO 5
#define SPONDA_SUPERIORE 5
//definizione id da usare;
#define IDTIME 45
#define IDCOC 0
#define IDRANA 50
#define IDGRANATE 60
#define IDPROIETTILE 70
#define IDMORTE -10 
#define IDRICHIESTA 90
// --- VARIABILI GLOBALI ---
//serve per proteggere la stampa a schermo
extern pthread_mutex_t mutex_ncurses;


extern const char *OPZIONI[];
extern const char *frog_sprite[2];
extern const char *coc_sprite[2][2];

           
typedef struct{
    int id;
    int y;
    int dir;
    int speed;
}Flusso;

typedef struct{
    int vite;
    int score;
    int tempo;
    int tane[5];
    int game_over; //serve per la condizione di uscita dal game
    pthread_mutex_t mutex_game;  // mutex per modifiche sicure
}Game_struct;

typedef struct{
    int vite;
    int tempo;
    int velocità_proiettili;
    int velocità_coccodrilli;
}Stat_game;

typedef struct{
    int id;
    int y;
    int x;
    int dir;
    int alive;
    int wait;
}Coccodrillo;

typedef struct{
    int id;
    int y;
    int x;
    int alive;
}Proiettile;

typedef struct{
    int id;
    int y;
    int x;
    int alive;
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

//è il formato del contenuto del gioco, quindi è un oggetto con dentro le informazioni su ciascun oggetto
typedef struct {
    int id;
    int y;
    int x;
    int dir;
    int speed;
    bool alive; //serve per capire se l ente è vivo
    int info; 
    pthread_t tid;
} messaggio;  

typedef struct {
    //array di strutture di tipo messaggio 
    messaggio buffer[BUFFER_SIZE];
    int in;  // indice di scrittura
    int out; // indice di lettura
    pthread_mutex_t mutex; //serve per fare entrare un thread alla volta
    sem_t empty; // quanti slot vuoti
    sem_t full;  // quanti slot pieni
} BufferC; 
//buffer c è un contenutor organizzata con sincronizzazione 
//creo una struttura globale condivisa 

//ho la struttura buffer c  pe rimpacchettar ein un solo oggetto 7
extern BufferC buffer;

typedef struct {
    Game_struct* game_struct;
    WINDOW* game;
    int vel_proiettile;
} ArgGrafica;

//funzioni per l'interfaccia di gioco--
int gameWin(WINDOW *game, int score);
int gameOver(WINDOW *game, int score);
int scegliDifficolta(WINDOW *game);
int menu(WINDOW *game, const char *title, const char *options[], int num_options);
void credits(WINDOW *game);
void windowGeneration(WINDOW *game, int maxX, int maxY, Game_struct* game_struct);
void creazione_colori();

//inizializzazione e flussi
void inizializza_buffer();
void def_vel_flussi(Flusso *flussi, int velocità_coccodrillo);
void def_dir_flussi(Flusso *flussi);

//funzioni di gioco
Game_struct startGame(WINDOW *game,Stat_game stat_game);
void crea_thread_gioco(Game_struct *game_struct, Flusso flussi[], int vel_proiettile, int tempo_totale, WINDOW* game);

void crea_thread_coccodrilli(Flusso flussi[], Game_struct *game_struct);

//produttori
void* thread_rana(void* arg);
void* thread_tempo(void* arg);
void* produttore_coccodrillo(void* arg);
void* thread_proiettile(void* arg);
void* thread_granata(void* arg);


// --- Lancio dinamico di granate/proiettili ---
void sparaGranata(int x, int y, int velocita);
void sparaProiettile_daCoccodrillo(Coccodrillo coccodrillo, int vel_proiettile);

// --- Buffer ---
void produttore(messaggio m);
messaggio consumatore();


// --- Gestore grafico e logica ---
void* Gestione_grafica(void* arg);
void draw_proiettile(WINDOW* game, Proiettile proiettile);
void draw_granate(WINDOW* game, Granata granate[2]);
void draw_frog(WINDOW *game, Rana rana);
void drawCoccodrilli(WINDOW *game, Coccodrillo *coccodrilli);


// --- Utility di gioco e collisioni ---
int RanaInFinestra(Rana rana, messaggio temp);
int CollisioneRanaProiettile(Rana rana, Proiettile proiettile);
int RanaSuTana(Rana rana, Game_struct* game_struct);
int RanaSuCoccodrillo(Rana *rana, Coccodrillo *coccodrilli);
void print_tempo(WINDOW* game, Game_struct* game_struct, int tempo);
void punteggio_tempo(Game_struct* game_struct);
int rand_funz(int min, int max);
#endif
