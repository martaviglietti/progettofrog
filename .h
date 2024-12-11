#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>


// struttura coccodrillo
typedef struct{
 int id; // identificativo
    int y; // posizione y
    int x; // posizione x
    int dir; // direzione
    int speed; // velocità
    bool alive; // è presente nello schermo?
} coccodrillo;

typedef struct{
    int x;
    int y; 
    bool alive;  //per contorllar ese il proiettile è presente o meno 
    int dir; 
}proiettile;
