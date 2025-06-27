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

#define BUFFER_SIZE 51
#define MAX_CROCODILES 24
#define NFLUSSI 8

//// Graphics
#define LARGHEZZA_GIOCO 81
#define ALTEZZA_GIOCO 49
#define POS_SPAWN_COC_SINISTRA -4
#define POS_SPAWN_COC_DESTRA 84
#define ALTEZZARANA 2
#define LARGHEZZARANA 3
#define ALTEZZACOCCODRILLO 2
#define LARGHEZZACOCCODRILLO 9
#define DELAY 100000
#define NTANE 6
#define FIUME 25
#define PRATO 5
#define SPONDA_SUPERIORE 5
#define DIM_FLUSSI 3

//// Buffer positions
#define IDX_COCCODRILLI 3
#define IDX_RANA 0
#define IDX_GRANATE 1
#define IDX_PROIETTILI 27

//// definizione id da usare (?)
#define IDTIME 45
#define IDCOC 0
#define IDRANA 50
#define IDGRANATE 60
#define IDPROIETTILE 70
#define IDMORTE -10 
#define IDRICHIESTA 90

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
    int tane[NTANE];
    int win; //serve per la condizione di uscita dal game
    WINDOW* game;
}Game_struct;

typedef struct{
    int vite;
    int tempo;
    int velocità_proiettili;
    int velocità_coccodrilli;
    Flusso flussi[NFLUSSI];
}gameConfig;

typedef struct {
    Game_struct* Game_struct;
    gameConfig* gameConfig;
} ThreadArgs;

//è il formato del contenuto del gioco, quindi è un oggetto con dentro le informazioni su ciascun oggetto
typedef struct {
    int y;
    int x;
    int dir;
    int speed;
    bool alive; //serve per capire se l ente è vivo
    float wait;  //per i coccodrilli, quando sono offline
    int tempo_prec;
    int shoot;
} messaggio;  

//buffer c è un contenutor organizzata con sincronizzazione 
//creo una struttura globale condivisa 
typedef struct {
    //array di strutture di tipo messaggio 
    messaggio buffer[BUFFER_SIZE];
    int in;  // indice di scrittura
    int out; // indice di lettura
    pthread_mutex_t mutex; //serve per fare entrare un thread alla volta
    //sem_t empty; // quanti slot vuoti
    //sem_t full;  // quanti slot pieni
} BufferC; 

// --- VARIABILI GLOBALI ---
//serve per proteggere la stampa a schermo
//extern pthread_mutex_t mutex_ncurses;

extern const char *OPZIONI[];
extern const char *frog_sprite[2];
extern const char *coc_sprite[2][2];
//ho la struttura buffer c  pe rimpacchettar ein un solo oggetto 7
extern BufferC buffer;


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
void fluxInit(gameConfig *gameConfig);
void def_dir_flussi(Flusso *flussi);

//funzioni di gioco
void startGame(Game_struct *game_struct, gameConfig* gameConfig);
void crea_thread_gioco(Game_struct *game_struct, gameConfig* gameConfig);

//funzioni inizializzazione oggetti
void CrocodileInit(Flusso *flussi, Game_struct* game_struct);
void GranateInit(Game_struct* game_struct);
void frogInit();
void ProjectileInit();

//produttori
void* thread_rana(void* arg);
void* thread_tempo(void* arg);
void* thread_coccodrillo(void* arg);
void* thread_proiettile(void* arg);
void* thread_granata(void* arg);


// --- Lancio dinamico di granate/proiettili ---
void sparaGranata(Game_struct* game_struct, gameConfig* gameConfig);
void sparaProiettile(Game_struct* game_struct, gameConfig* gameConfig, int idx);

// --- Buffer ---
void produttore(messaggio m);
messaggio consumatore();


// --- Gestore grafico e logica ---
void* Gestione_grafica(void* arg);
void draw_proiettile(WINDOW* game, messaggio proiettile);
void draw_granate(WINDOW* game, messaggio granate[2]);
void draw_frog(WINDOW *game, messaggio rana);
void drawCoccodrilli(WINDOW *game, messaggio *coccodrilli);


// --- Utility di gioco e collisioni ---
int RanaInFinestra(messaggio rana, messaggio temp);
int CollisioneRanaProiettile(messaggio rana, messaggio proiettile);
int RanaSuTana(messaggio rana, Game_struct* game_struct);
int RanaSuCoccodrillo(messaggio *rana, messaggio *coccodrilli);
void print_tempo(WINDOW* game, Game_struct* game_struct, int tempo);
void punteggio_tempo(Game_struct* game_struct);
int rand_funz(int min, int max);
#endif
