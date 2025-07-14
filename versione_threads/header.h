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
#include <stdatomic.h>

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
#define PROJ_MIN_WAIT 3
#define PROJ_MAX_WAIT 8
#define CROC_MAX_WAIT 2
#define CROC_MIN_WAIT 7

typedef enum {
    FROG_STATUS,
    TIME_STATUS,
    PROJ_STATUS,
    GRAN_STATUS,
    CROC_STATUS,
} MessageType;

typedef struct {
    MessageType type;
    void* data; // dynamically allocated content
} Message;

typedef struct{
    int y;
    int dir;
    int speed;
}Flusso;

typedef struct{
    int vite;
    float tempo;
    int velocità_proiettili;
    int velocità_coccodrilli;
    Flusso flussi[NFLUSSI];
}gameConfig;

typedef struct{
    int vite;
    int score;
    int tane[NTANE];
    int tane_count;
    int win; //serve per la condizione di uscita dal game
    gameConfig* gameCfg;
    WINDOW *game;
}Game_struct;

//è il formato del contenuto del gioco, quindi è un oggetto con dentro le informazioni su ciascun oggetto
typedef struct {
    int y;
    int x;
    int dir;
    int speed;
    _Atomic bool alive; //serve per capire se l ente è vivo
    int wait;  //per i coccodrilli, quando sono offline
    struct timeval prev;
    int idx;
} Crocodile;  

typedef struct {
    int y;
    int x;
    int crocIdx;
    int key;
    _Atomic bool alive;
    pthread_mutex_t mutex; // protects head/tail and buffer access
} Frog;

typedef struct {
    _Atomic float time;
    _Atomic bool alive;
} Time;

typedef struct {
    int y;
    int x;
    int dir;
    int speed;
    _Atomic bool alive; //serve per capire se l ente è vivo
    struct timeval prev;
} Projectile;

//buffer c è un contenutor organizzata con sincronizzazione 
//creo una struttura globale condivisa
#define BUFFER_SIZE 200 

typedef struct {
    Message buffer[BUFFER_SIZE];
    int head;   // index to insert next event
    int tail;   // index to remove next event
    sem_t full; // counts how many items are available to consume
    sem_t empty; // counts how many slots are free for writing
    pthread_mutex_t mutex; // protects head/tail and buffer access
} messageBuffer;


// --- VARIABILI GLOBALI ---
extern const char *OPZIONI[];
extern const char *frog_sprite[2];
extern const char *coc_sprite[2][2];
//ho la struttura buffer c  pe rimpacchettar ein un solo oggetto 7
extern messageBuffer myBuffer;


//funzioni per l'interfaccia di gioco--
int gameWin(WINDOW *game, int score);
int gameOver(WINDOW *game, int score);
int scegliDifficolta(WINDOW *game);
int menu(WINDOW *game, const char *title, const char *options[], int num_options);
void credits(WINDOW *game);
void windowGeneration(WINDOW *game, int maxX, int maxY, const Game_struct* game_struct);
void creazione_colori();

//inizializzazione e flussi
void bufferInit();
void fluxInit(gameConfig *gameConfig);
void def_dir_flussi(Flusso *flussi);

//funzioni di gioco
Game_struct* startGame(WINDOW *game, gameConfig* gameConfig);
void crea_thread_gioco(gameConfig* gameConfig);

//funzioni inizializzazione oggetti
Time* timeInit();
Crocodile* CrocodileInit(Flusso *flussi, const float time);
Projectile* GranateInit(const Frog* frog, const float time, const gameConfig* gameConfig);
Frog* frogInit();
Projectile* ProjectileInit(const Crocodile* croc, const float time, const gameConfig* gameCfg);

//produttori
void* thread_rana(void* arg);
void* thread_tempo(void* arg);
void* thread_coccodrillo(void* arg);
void* thread_proiettile(void* arg);
void* thread_granata(void* arg);
void* thread_grafica(void* arg);


// --- Lancio dinamico di granate/proiettili ---
void sparaProiettile(Projectile* proj, const Crocodile* croc, const float time, const gameConfig* gameCfg);

// --- Buffer ---
//void produttore(messaggio m);
//messaggio consumatore();


// --- Gestore grafico e logica ---
void draw_proiettile(WINDOW* game, const Projectile* projectiles);
void draw_granate(WINDOW* game, const Projectile* granates);
void draw_frog(WINDOW *game, Frog* rana);
void drawCoccodrilli(WINDOW *game, const Crocodile* crocodiles);


// --- Utility di gioco e collisioni ---
bool CollRanaProiettile(const Frog* frog, Projectile* projectiles);
bool CollGranataProiettile(const Projectile* gran, Projectile* projectiles);
bool RanaSuTana(const Frog* frog, Game_struct* game_struct);
int RanaSuCoccodrillo(const Frog *frog, const Crocodile* croc);
void print_tempo(WINDOW* game, Game_struct* game_struct, int tempo);
void punteggio_tempo(Game_struct* game_struct);
int rand_funz(int min, int max);

void push_event(messageBuffer* b, Message* m);
Message pop_event(messageBuffer* b);

#endif
