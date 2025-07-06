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

        printf("GestGraph: Rana: alive=%d, x=%d, y=%d, tempo_prec=%f\n", frogLocal.alive, frogLocal.x, frogLocal.y, frogLocal.tempo_prec);
        bool endManche = false;

        if (!endManche && frogLocal.y < TANA_POS){
            endManche = true;
            if (RanaSuTana(&frogLocal, &gameLocal)){
                gameLocal.score += 15 + (int)(15 * gameLocal.time / 100);
                printf("GestGraph: rana ha raggiunto una delle tane\n");
            }
            else{
                gameLocal.vite--;
                gameLocal.score -= 10;
                printf("GestGraph: rana haraggiungo l'estremo superiore della mappa...\n");
            }
            
        }

        if (!endManche){

            int crocId = RanaSuCoccodrillo(&frogLocal);
            const int waterYtop = TANA_POS + SPONDA_SUPERIORE;
            const int waterYlow = waterYtop + NFLUSSI * DIM_FLUSSI;

            if (crocId != -1 && (crocId < IDX_COCCODRILLI || crocId >= IDX_COCCODRILLI + MAX_CROCODILES)){
                printf("ERROR: Crocodile idx is not valid!\n");
                exit(EXIT_FAILURE);
            }
            if(crocId == -1){                // rana non su un coccodrillo
                if (frogLocal.y < waterYlow  && frogLocal.y > waterYtop){      //rana fell in the water
                    endManche = true;
                    gameLocal.vite--;
                    gameLocal.score -= 10;
                    printf("GestGraph: la rana é cadut in acqua...\n");
                }
                else printf("GestGraph: la rana é sulla terraferma\n");
            }    
            else{
                LOCK_CROCS();
                const Crocodile* crocod = (Crocodile*)buffer.buffer[crocId];

                const float tempo_prec = (frogLocal.tempo_prec != -1) ? frogLocal.tempo_prec : crocod->tempo_prec;              //depends on if it is the first time the frog is on the crocodile  
                const int newX = frogLocal.x + crocod->dir * (int)(crocod->speed * (frogLocal.tempo_prec - gameLocal.time));

                UNLOCK_CROCS();
                
                if (newX != frogLocal.x){
                    LOCK_FROG();
                    Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
                    if ( newX > RANA_XMIN && newX < RANA_XMAX) frog->x = newX;     //if the new position is outside the window, the frog doesnt move. The next iteration, the crocodile will be off, and so the frog will be in the water
                    frog->tempo_prec = gameLocal.time;
                    UNLOCK_FROG();
                }
                printf("GestGraph: Rana é su coccodrillo %d, newPos = %d, time_prec = %f\n", crocId, newX, tempo_prec);
                
                
            }
        }

        if (!endManche){
            if(CollRanaProiettile(&frogLocal)){
                printf("GestGraph: rana colpita da un proiettile...\n");
                endManche = true;
                gameLocal.vite--;
                gameLocal.score -= 15;
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

            newManche(game_struct, gameCfg);
            UNLOCK_GAME();
        }
        
        LOCK_READ_GAME();
        gameLocal = *(Game_struct*)buffer.buffer[IDX_GAME];
        UNLOCK_GAME();

        LOCK_FROG();
        frogLocal = *(Frog*)buffer.buffer[IDX_RANA];
        UNLOCK_FROG();

        LOCK_GRAPH();
        WINDOW* game = (WINDOW*)buffer.buffer[IDX_GRAPH];
        werase(game);
        windowGeneration(game, COLS, LINES, &gameLocal);

        drawCoccodrilli(game);
        draw_granate(game);
        draw_proiettile(game);
        draw_frog(game, &frogLocal);

        // Punteggio
        wattron(game, COLOR_PAIR(15));
        mvwprintw(game, 2, 2, "Punteggio: %d ", gameLocal.score);
        wattroff(game, COLOR_PAIR(15));

        // Vite
        wattron(game, COLOR_PAIR(15));
        mvwprintw(game, 2, 50, "Vite:");
        mvwhline(game, 2, 55, ' ', 21);
        for (int i = 0; i < gameLocal.vite; i++) {
            mvwprintw(game, 2, 55 + i * 2, "❤️");
        }
        wattroff(game, COLOR_PAIR(15));
        // Tempo
        wattron(game, COLOR_PAIR(15));
        mvwhline(game, 46, 2, ' ', 10);
        mvwprintw(game, 46, 2, "Tempo: %d ", (int)gameLocal.time);
        wattroff(game, COLOR_PAIR(15));

        print_tempo(game, &gameLocal, (int)gameLocal.time);
        wrefresh(game);

        UNLOCK_GRAPH();
        usleep(3 * 1000);  // sleep 10 ms
    }
    pthread_exit(NULL);
}
        
//funzione che mostra la barra del tempo rimanente
void print_tempo(WINDOW* game,Game_struct* game_struct, int tempo){
    wattron(game, COLOR_PAIR(7));
    mvwhline(game,46,15, ' ', (int)(62*((float)game_struct->time/tempo)));
    wattroff(game, COLOR_PAIR(7));
}
