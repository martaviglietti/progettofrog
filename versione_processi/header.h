#include <ncurses.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "menu.h"
#include <string.h>
#include <fcntl.h>
#define MAX_CROCODILES 8
#define POS_SPAWN_COC_SINISTRA -4
#define POS_SPAWN_COC_DESTRA 83
#define ALTEZZARANA 2
#define LARGHEZZARANA 3
#define ALTEZZACOCCODRILLO 2
#define LARGHEZZACOCCODRILLO 9
#define DELAY 100000
#define TANE 6
#define FIUME 25
#define PRATO 5
#define SPONDA_SUPERIORE 5
#define N_PID 100
//definizione id da usare;
#define IDTIME 40
#define IDCOC 0
#define IDRANA 20
#define IDGRANATE 30
#define IDPROIETTILE 50
#define IDAGGIUNTAPID 60
#define IDMORTE -10
#define IDRICHIESTA 70

#ifndef HEADER_H
#define HEADER_H

// Opzioni del menu
static const char *OPZIONI[] = {
    "Inizia gioco", "Crediti", "Esci"
};



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
    int game; //serve per la condizione di uscita dal game;
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
}Coccodrillo;

typedef struct{double elapsed_time;
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

// Dichiarazioni delle variabili globali
extern const char *frog_sprite[2];
extern const char *coc_sprite[2][2];

int gameWin(WINDOW *game, int score);
int gameOver(WINDOW *game, int score);
int scegliDifficolta(WINDOW *game);
void windowGeneration(WINDOW *game, int maxX, int maxY, Game_struct* game_struct);
Game_struct startGame(WINDOW *game,Stat_game stat_game);
void Gestione_grafica(WINDOW* game,int pipe1[], int pipe2[], int array_pid[],int vel_proiettili, Game_struct* game_struct);
int CollisioneRanaProiettile(Rana rana,Proiettile proiettile);
int RanaSuTana(Rana rana, Game_struct* game_struct);
int RanaSuCoccodrillo(Rana *rana, Coccodrillo *coccodrilli);
void creazione_processi(Flusso *flussi, int array_pid[N_PID], int pipe1[],WINDOW* game);
void funzione_gestione_coccodrilli(Flusso *flussi,int pipe1[]);
void funzione_coccodrillo(Temp coccodrillo,Flusso flussi[8],int id_flusso_scelto, int pipe1[]);
void funz_proiettile(Coccodrillo coccodrillo,int vel_proiettile, int pipe1[]);
void tempo(int pipe1[]);
void kill_processi(pid_t* pid);
void def_vel_flussi(Flusso *flussi, int velocità_coccodrillo);
void def_dir_flussi(Flusso *flussi);
int menu(WINDOW *game, const char *title, const char *options[], int num_options);
void credits(WINDOW *game);
int rand_funz(int min, int max);
void frog(WINDOW* game,int pipe1[2]);
void sparaGranata(int startX, int startY,int vel_proiettile ,int pid_array[],int pipe1[], int pipe2[]);
void sparaProiettile(Coccodrillo coccodrillo,int vel_proiettile,int pid_array[], int pipe1[]);
void granata(int startX, int startY,int vel_proiettile,int pipe1[], int pipe2[]);
void creazione_colori();
void draw_proiettile(WINDOW* game, Proiettile proiettile);
void draw_granate(WINDOW* game, Granata granate[2]);
void draw_frog(WINDOW *game, Rana rana);
void drawCoccodrilli(WINDOW *game, Coccodrillo *coccodrilli);
void sblocca_fd(int fd);
void killProcess(pid_t pid);

#endif // HEADER_H

