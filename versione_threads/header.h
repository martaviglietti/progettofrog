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

#define MAX_CROCODILES 24
#define NFLUSSI 8
#define NGRANATE 2

//// Graphics
#define LARGHEZZA_GIOCO 81
#define ALTEZZA_GIOCO 49
#define POS_SPAWN_COC_SINISTRA -4
#define POS_SPAWN_COC_DESTRA 84
#define FIUME 25
#define PRATO 5
#define SPONDA_SUPERIORE 5
#define DIM_FLUSSI 3

#define ALTEZZACOCCODRILLO 2
#define LARGHEZZACOCCODRILLO 9

#define TANA_POS 10
#define NTANE 5
#define LARGHEZZA_TANA 5

#define ALTEZZARANA 2
#define LARGHEZZARANA 3
#define RANA_XINIT 40
#define RANA_YINIT 43
#define RANA_XMAX 78
#define RANA_XMIN 2

#define DELAY 100000


//// Buffer positions
#define IDX_GAME 0
#define IDX_RANA 1
#define IDX_GRANATE 2
#define IDX_COCCODRILLI 4
#define IDX_PROIETTILI 28
#define BUFFER_SIZE 52

typedef struct{
    int y;
    int dir;
    int speed;
}Flusso;

typedef struct{
    int vite;
    int score;
    int tane[NTANE];
    int tane_count;
    int win; //serve per la condizione di uscita dal game
    float time;
}Game_struct;

typedef struct{
    int vite;
    float tempo;
    int velocità_proiettili;
    int velocità_coccodrilli;
    Flusso flussi[NFLUSSI];
}gameConfig;

//è il formato del contenuto del gioco, quindi è un oggetto con dentro le informazioni su ciascun oggetto
typedef struct {
    int y;
    int x;
    int dir;
    int speed;
    bool alive; //serve per capire se l ente è vivo
    int wait;  //per i coccodrilli, quando sono offline
    float tempo_prec;
} Crocodile;  

typedef struct {
    int y;
    int x;
    int crocIdx;
    int key;
} Frog;

typedef struct {
    int y;
    int x;
    int dir;
    int speed;
    bool alive; //serve per capire se l ente è vivo
    float tempo_prec;
} Projectile;

//buffer c è un contenutor organizzata con sincronizzazione 
//creo una struttura globale condivisa 
#define NMUTEX 3
typedef struct {
    //array di strutture di tipo messaggio 
    void* buffer[BUFFER_SIZE];
    pthread_mutex_t mutex[NMUTEX]; //serve per fare entrare un thread alla volta
    pthread_rwlock_t mutex_gameStat;

} BufferC; 

// --- VARIABILI GLOBALI ---
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
void windowGeneration(WINDOW *game, int maxX, int maxY, const Game_struct* game_struct);
void creazione_colori();

//inizializzazione e flussi
void inizializza_buffer();
void fluxInit(gameConfig *gameConfig);
void def_dir_flussi(Flusso *flussi);

//funzioni di gioco
Game_struct* startGame(WINDOW *game, gameConfig* gameConfig);
void crea_thread_gioco(gameConfig* gameConfig);
void restartFrog();

//funzioni inizializzazione oggetti
void CrocodileInit(Flusso *flussi);
void GranateInit();
void frogInit();
void ProjectileInit();

//produttori
void* thread_rana(void* arg);
void* thread_tempo(void* arg);
void* thread_coccodrillo(void* arg);
void* thread_proiettile(void* arg);
void* thread_granata(void* arg);
void* Gestione_grafica(void* arg);


// --- Lancio dinamico di granate/proiettili ---
void sparaGranata(const Frog* frog, const float time, const gameConfig* gameConfig);
void sparaProiettile(const float time, const gameConfig* gameConfig, const int idx);

// --- Buffer ---
//void produttore(messaggio m);
//messaggio consumatore();


// --- Gestore grafico e logica ---
void draw_proiettile(WINDOW* game);
void draw_granate(WINDOW* game);
void draw_frog(WINDOW *game, Frog* rana);
void drawCoccodrilli(WINDOW *game);


// --- Utility di gioco e collisioni ---
bool CollRanaProiettile(const Frog* frog);
bool CollGranataProiettile(Projectile* gran);
void RanaSuTana(const Frog* frog, const gameConfig* gameConfig);
int RanaSuCoccodrillo(const Frog *frog, const gameConfig* gameConfig);
void print_tempo(WINDOW* game, Game_struct* game_struct, int tempo);
void punteggio_tempo(Game_struct* game_struct);
int rand_funz(int min, int max);

#define LOCK_FROG() pthread_mutex_lock(&buffer.mutex[0])
#define UNLOCK_FROG() pthread_mutex_unlock(&buffer.mutex[0])

#define LOCK_CROCS() pthread_mutex_lock(&buffer.mutex[1])
#define UNLOCK_CROCS() pthread_mutex_unlock(&buffer.mutex[1])

#define LOCK_PROJ() pthread_mutex_lock(&buffer.mutex[2])
#define UNLOCK_PROJ() pthread_mutex_unlock(&buffer.mutex[2])

#define LOCK_READ_GAME() pthread_rwlock_rdlock(&buffer.mutex_gameStat)
#define LOCK_WRITE_GAME() pthread_rwlock_wrlock(&buffer.mutex_gameStat)
#define UNLOCK_GAME() pthread_rwlock_unlock(&buffer.mutex_gameStat)

#endif
