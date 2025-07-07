#include "header.h"
#include <ncurses.h>
#include <stdio.h>

extern pthread_mutex_t mutex_tane;


void* Gestione_grafica(void* arg){
    printf("...Inizio gest grafica...\n");

    gameConfig* gameCfg = (gameConfig*)arg;
    Game_struct gameLocal;
    Frog frogLocal;


    while(1){
        LOCK_READ_GAME();
        Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            UNLOCK_GAME();
            break;
        }

        gameLocal = *game_struct;
        UNLOCK_GAME();

        // Check frog position
        LOCK_FROG();
        frogLocal = *(Frog*)buffer.buffer[IDX_RANA];;
        UNLOCK_FROG();

        bool endManche = false;

        if (!endManche){
            if(CollRanaProiettile(&frogLocal)){
                printf("GestGraph: rana colpita da un proiettile...\n");
                endManche = true;
                gameLocal.vite--;
                gameLocal.score -= 15;
                restartFrog();
                //game_struct->time = gameConfig->tempo;
            }
        }

        if (endManche) {
            printf("Nuova manche...\n");
            LOCK_WRITE_GAME();
            Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];
            game_struct->score = gameLocal.score;
            game_struct->vite = gameLocal.vite;
            for (int i=0;i<NTANE-1;i++) {
                game_struct->tane[i]=gameLocal.tane[i];
            }

            UNLOCK_GAME();
        }

        //usleep(1 * 1000);  // sleep 10 ms
        
    }
    pthread_exit(NULL);
}
        
//funzione che mostra la barra del tempo rimanente
void print_tempo(WINDOW* game,Game_struct* game_struct, int tempo){
    wattron(game, COLOR_PAIR(7));
    mvwhline(game,46,15, ' ', (int)(62*((float)game_struct->time/tempo)));
    wattroff(game, COLOR_PAIR(7));
}
