#include "header.h"

extern pthread_mutex_t mutex_tane;


void* Gestione_grafica(void* arg){

    gameConfig* gameCfg = (gameConfig*)arg;

    while(1){

        pthread_mutex_lock(&buffer.mutex);

        Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];
        WINDOW* game = game_struct->game;

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            pthread_mutex_unlock(&buffer.mutex);
            break;
        }

        // Check frog position
        Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
        bool endManche = false;

        if (!frog->alive){
            endManche=true;
        }

        if (!endManche && frog->y < TANA_POS){
            endManche = true;
            frog->alive = false;
            if (RanaSuTana(frog, game_struct)){
                game_struct->score += 15 + (int)(15 * (float)game_struct->tempo / 100);
            }
            else{
                game_struct->vite--;
                game_struct->score -= 10;
            }
        }

        if (!endManche){

            int crocId = RanaSuCoccodrillo(frog);
            const int waterYtop = TANA_POS + SPONDA_SUPERIORE;
            const int waterYlow = waterYtop + NFLUSSI * DIM_FLUSSI;

            if (crocId != -1 && (crocId < IDX_COCCODRILLI || crocId >= IDX_COCCODRILLI + MAX_CROCODILES)){
                printf("ERROR: Crocodile idx is not valid!");
                exit(EXIT_FAILURE);
            }
            if(crocId == -1 && frog->y < waterYlow  && frog->y > waterYtop){            //rana fell in the water
                endManche = true;
                game_struct->vite--;
                game_struct->score -= 10;
                frog->alive = false;
            }
            else{
                const Crocodile* crocod = (Crocodile*)buffer.buffer[crocId];

                const float tempo_prec = (frog->tempo_prec != -1) ? frog->tempo_prec : crocod->tempo_prec;              //depends on if it is the first time the frog is on the crocodile  
                const int newX = frog->x + crocod->dir * crocod->speed * (tempo_prec - game_struct->tempo);  
                if ( newX > RANA_XMIN && newX < RANA_XMAX) frog->x = newX;     //if the new position is outside the window, the frog doesnt move. The next iteration, the crocodile will be off, and so the frog will be in the water

                frog->tempo_prec = game_struct->tempo;   
            }
        }

        if (!endManche){
            if(CollRanaProiettile(frog)){
                endManche = true;
                game_struct->vite--;
                game_struct->score -= 15;
                frog->alive = false;
            }
        }

        if (endManche) newManche(game_struct, gameCfg);
                
        werase(game);
        windowGeneration(game, COLS, LINES, game_struct);



        drawCoccodrilli(game);
        draw_granate(game);
        draw_proiettile(game);
        draw_frog(game, frog);

        // Punteggio
        wattron(game, COLOR_PAIR(15));
        mvwprintw(game, 2, 2, "Punteggio: %d ", game_struct->score);
        wattroff(game, COLOR_PAIR(15));

        // Vite
        wattron(game, COLOR_PAIR(15));
        mvwprintw(game, 2, 50, "Vite:");
        mvwhline(game, 2, 55, ' ', 21);
        for (int i = 0; i < game_struct->vite; i++) {
            mvwprintw(game, 2, 55 + i * 2, "❤️");
        }
        wattroff(game, COLOR_PAIR(15));
        // Tempo
        wattron(game, COLOR_PAIR(15));
        mvwhline(game, 46, 2, ' ', 10);
        mvwprintw(game, 46, 2, "Tempo: %d ", (int)game_struct->tempo);
        wattroff(game, COLOR_PAIR(15));

        print_tempo(game, game_struct, (int)game_struct->tempo);
        wrefresh(game);

        pthread_mutex_unlock(&buffer.mutex);
    }
    pthread_exit(NULL);
}
        
//funzione che mostra la barra del tempo rimanente
void print_tempo(WINDOW* game,Game_struct* game_struct, int tempo){
    wattron(game, COLOR_PAIR(7));
    mvwhline(game,46,15, ' ', (int)(62*((float)game_struct->tempo/tempo)));
    wattroff(game, COLOR_PAIR(7));
}
