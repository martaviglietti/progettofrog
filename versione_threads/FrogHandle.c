#include"header.h"
#include <ncurses.h>

//GESTIONE RANA-------------------------------------------------------------------------------------------------------------
void frogInit(){

    buffer.buffer[IDX_RANA] = malloc(sizeof(Frog));
    if (buffer.buffer[IDX_RANA]== NULL) {
        fprintf(stderr, "malloc failed at Frog Initialization at index %d\n", IDX_RANA);
        exit(EXIT_FAILURE);
    }
    
    Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
    frog->x = RANA_XINIT;
    frog->y = RANA_YINIT;
    frog->alive = true;
    frog->tempo_prec = -1;
    //printf("Rana inizializzata con alive=%d, x=%d, y=%d, tempo_prec=%f\n", frog->alive, frog->x, frog->y, frog->tempo_prec);
}

void* thread_rana(void* arg) {
    gameConfig* gameCfg = (gameConfig*)arg;
    Frog frogLocal;

    while(1){

        LOCK_READ_GAME();
        const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            UNLOCK_GAME();
            break;
        }
        const float time = game_struct->time;
        UNLOCK_GAME();

        LOCK_GRAPH();
        WINDOW* game = (WINDOW*)buffer.buffer[IDX_GRAPH];;
        keypad(game, true);  // abilita frecce
        nodelay(game, FALSE); // aspetta input (puoi metterlo TRUE se vuoi non bloccare il loop)
        int key = wgetch(game);
        UNLOCK_GRAPH();
        //int key = 258 +rand_funz(0,3);

        LOCK_FROG();
        frogLocal = *(Frog*)buffer.buffer[IDX_RANA];
        UNLOCK_FROG();

        if (!frogLocal.alive){
            continue;
        }

        int old = frogLocal.y;
        int newPos;
        switch (key) {
            case KEY_UP:
                frogLocal.y  -=  ALTEZZARANA;
                if (frogLocal.y < TANA_POS) frogLocal.alive = false;
                break;

            case KEY_DOWN:
                newPos = frogLocal.y + ALTEZZARANA;
                if (newPos < RANA_YINIT) frogLocal.y = newPos;
                break;

            case KEY_LEFT:
                newPos = frogLocal.x - LARGHEZZARANA;
                if (newPos > RANA_XMIN) frogLocal.x = newPos;
                break;

            case KEY_RIGHT:
                newPos = frogLocal.x + LARGHEZZARANA;
                if (newPos < RANA_XMAX) frogLocal.x = newPos;
                break;

            case 's':  // spara granate
                sparaGranata(&frogLocal, time, gameCfg);
                break;
        }    
        printf("frog moved from %d to %d, at time=%f\n", old, frogLocal.y, time);    
        LOCK_FROG();
        Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
        frog->x = frogLocal.x;
        frog->y = frogLocal.y;
        frog->alive = frogLocal.alive; 
        UNLOCK_FROG();
        usleep(3 * 1000);  // sleep 10 ms
    }
    pthread_exit(NULL);
}

void sparaGranata(const Frog* frog, const float time, const gameConfig* gameConfig) {

    bool allowed = true;

    for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){

        const Projectile* gran = (Projectile*)buffer.buffer[i];
        if(gran->alive) allowed = false;
    }

    if (allowed){

        for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){
            
            Projectile* gran = (Projectile*)buffer.buffer[i];
            
            gran->x = frog->x;
            gran->y = frog->y;
            gran->alive = 1;
            gran->tempo_prec = time;
            gran->dir = 2*(i%2) -1;
            gran->speed = gameConfig->velocità_proiettili;
        }
    }
}

//GESTIONE GRANATE-----------------------------------------
void GranateInit(){

    for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){

        buffer.buffer[i] = malloc(sizeof(Projectile));
        if (buffer.buffer[i]== NULL) {
            fprintf(stderr, "malloc failed at Granade Initialization at index %d\n", i);
            exit(EXIT_FAILURE);
        }

        Projectile* gran = (Projectile*)buffer.buffer[i];
        gran->x = -1;
        gran->y = -1;
        gran->alive = 0;
        gran->tempo_prec = -1;
        gran->dir = -1;
        gran->speed =-1;

        //printf("Granata %d inizializzata con alive=%d, x=%d, y=%d, speed=%d, dir=%d, tempo_prec=%f\n", i, gran->alive, gran->x, gran->y, gran->speed, gran->dir, gran->tempo_prec);

    }
}

void* thread_granata(void* arg) {

    while(1){

        LOCK_READ_GAME();
        const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            UNLOCK_GAME();
            break;
        }
        const float time = game_struct->time;
        UNLOCK_GAME();

        LOCK_FROG();
        for (int i = IDX_GRANATE; i < IDX_GRANATE + 2; i++){

            Projectile* gran = (Projectile*)buffer.buffer[i];

            if (!gran->alive || CollGranataProiettile(gran)){
                continue;
            }

            const int newX = gran->x + gran->dir * (int)(gran->speed * (gran->tempo_prec - time));
            if (newX == gran->x) continue;
            if (newX > 0 && newX < LARGHEZZA_GIOCO && !CollGranataProiettile(gran)){
                gran->x = newX;
                gran->tempo_prec = time;
            }
            else gran->alive=0;
        }
        UNLOCK_FROG();
        usleep(10 * 1000);  // sleep 10 ms
    }
    pthread_exit(NULL);
}

