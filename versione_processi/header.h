#ifndef HEADER_H     
#define HEADER_H  






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
#define NUMERO_COCCODRILLI 24
#define LARGHEZZA_GIOCO 81
#define ALTEZZA_GIOCO 49
#define SPAWN_SX_COCCODRILLO -4
#define SPAWN_DX_COCCODRILLO 84
#define NUMERO_PROIETTILI 15
#define ALTEZZA_RANA 2
#define LARGHEZZARANA 3
#define ALTEZZA_COCCODRILLO 2
#define LARGHEZZA_COCCODRILLO 9
#define DELAY 100000
#define TANE 6
#define FIUME 25
#define PRATO 5
#define SPONDA_SUPERIORE 5
#define NUMERO_PID 100
//definizione id da usare;
#define ID_TIME 45
#define ID_COCCODRILLI 0
#define ID_RANA 50
#define ID_GRANATE 60
#define ID_PROIETTILE 70
#define ID_AGGIUNTAPID 30
#define ID_MORTE -10
#define ID_RICHIESTA 90





            


typedef struct{
    int id;
    int y;
    int dir;
    int velocità;
}Flusso;


typedef struct{
    int vite;
    int punteggio;
    int tempo;
    int tane[5];
    int gioco;  //la utilizziamo come condizione di uscita dal gioco;
}Statistiche;

typedef struct{
    int livello_difficoltà;
    int vite;
    int tempo;
    int velocità_proiettili;
    int velocità_coccodrilli;
    
 
}Parametri;

typedef struct{
    int id;
    int y;
    int x;
    int dir;
    int vivo;
    int attesa;
}Coccodrillo;

typedef struct{
    int id;
    int y;
    int x;
    int vivo;
}Proiettile;

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
	int id;
	int x;
	int y;
	int info;
}Temp;


// Dichiarazioni delle variabili globali
extern const char *rana_sprite[2];
extern const char *coccodrillo_sprite[2][2];
extern const char *opzioni[];

int game_vinto(WINDOW *finestra_gioco, int punteggio);
int game_perso(WINDOW *finestra_gioco, int punteggio);
int scelta_difficoltà(WINDOW *finestra_gioco);
void generatore_finestra(WINDOW *finestra_gioco, int maxX, int maxY, Statistiche * statistiche_gioco);
Statistiche Partita(WINDOW *finestra_gioco,Parametri parametri_gioco);
void gestore_grafica(WINDOW* finestra_gioco,int pipe1[], int pipe2[], int array_pid[],int velocità_proiettili, Statistiche * statistiche_gioco);
int collisione_rana_proiettili(Rana* rana,Proiettile proiettile[], Statistiche * statistiche_gioco, int giocare);
int rana_su_tana(Rana*rana, Statistiche * statistiche_gioco);
int rana_su_coccodrillo(Rana *rana, Coccodrillo *coccodrilli);
void creazione_processi(Flusso *flussi, int array_pid[NUMERO_PID], int pipe1[],WINDOW* finestra_gioco, Parametri* parametri_gioco);
void funzione_gestione_coccodrilli(Flusso *flussi,int pipe1[],Parametri* parametri_gioco);
void funzione_coccodrillo(Temp coccodrillo,Flusso flussi[8],int id_flusso_scelto, int pipe1[],Parametri* parametri_gioco);
void funzione_proiettile(int id,Coccodrillo coccodrillo,int velocità_proiettili, int pipe1[]);
void tempo(int pipe1[]);
void kill_processi(pid_t* pid);
void velocità_flussi(Flusso *flussi, int velocità_coccodrillo);
void direzione_flussi(Flusso *flussi);
int menu(WINDOW *finestra_gioco, const char *title, const char *options[], int num_options);
void crediti(WINDOW *finestra_gioco);
int numero_random(int min, int max);
void funzione_rana(WINDOW* finestra_gioco,int pipe1[2]);
void spara_granata(int inizioX, int inizioY,int velocità_proiettili ,int pid_array[],int pipe1[], int pipe2[]);
void spara_proiettile(int id, int identificatore_coc, Coccodrillo* coccodrilli,int velocità_proiettili,int pid_array[], int pipe1[]);
void funzione_granata(int inizioX, int inizioY,int velocità_proiettili,int pipe1[], int pipe2[]);
void creazione_colori();
void disegna_proiettile(WINDOW* finestra_gioco, Proiettile proiettile[]);
void disegna_granate(WINDOW* finestra_gioco, Granata granate[2]);
void disegna_rana(WINDOW *finestra_gioco, Rana* rana);
void disegna_coccodrilli(WINDOW *finestra_gioco, Coccodrillo *coccodrilli);
void sblocca_fd(int fd);
void kill_processo(pid_t pid);
int rana_in_finestra(Rana* rana, Temp*temp);
void blocca_fd(int fd);
void svuota_pipe(int pipe_fd);
void barra_tempo(WINDOW* finestra_gioco,Statistiche * statistiche_gioco, int tempo);    
void punteggio_tempo(Statistiche * statistiche_gioco);         
void collisione_granate_confine(int pipe, Granata* granate);
void collisione_proiettili_confine(int array_pid[], Proiettile proiettile[]);
void collisione_granate_proiettili(int pipe2, Granata* granate, Proiettile proiettile[], int array_pid[], Statistiche * statistiche_gioco);
void uccidi_granate(Granata* granate, int array_pid[]);
void riattivazione_coccodrilli(Coccodrillo* coccodrilli, int distanze_coccodrilli[], int array_pid[]);
void attesa_coccodrilli(int id, Coccodrillo* coccodrilli, int distanze_coccodrilli[], int array_pid[]);
void controllo_stato_coccodrillo(int id,Coccodrillo* coccodrilli);
int movimento_rana_su_coccodrillo(int id, int coccodrillo_scelto, Coccodrillo* coccodrilli, Rana* rana, Statistiche * statistiche_gioco , int giocare);
void impostazioni_gioco(Parametri* parametri_gioco, int difficoltà );
float numero_random_float(float minimo, float max);
void controllo_sparo_proiettile(int* pipe1, int* array_pid, Coccodrillo* coccodrilli, Proiettile* proiettili, struct timeval* inizio, struct timeval* fine, float* numero_randomico, int velocità_proiettili);





#endif 

