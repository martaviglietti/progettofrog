#include"header.h"

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
}

void* thread_rana(void* arg) {
    gameConfig* gameCfg = (gameConfig*)arg;

    while(1){

        pthread_mutex_lock(&buffer.mutex);

        const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];
        WINDOW* game = game_struct->game;

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            pthread_mutex_unlock(&buffer.mutex);
            break;
        }

        Frog* frog = (Frog*)buffer.buffer[IDX_RANA];
        if (!frog->alive){
            pthread_mutex_unlock(&buffer.mutex); 
            continue;
        }

        keypad(game, true);  // abilita frecce
        nodelay(game, FALSE); // aspetta input (puoi metterlo TRUE se vuoi non bloccare il loop)

        int key = wgetch(game);

        int newPos;
        switch (key) {
            case KEY_UP:
                frog->y  -=  ALTEZZARANA;
                if (frog->y < TANA_POS) frog->alive = false;
                break;

            case KEY_DOWN:
                newPos = frog->y + ALTEZZARANA;
                if (newPos < RANA_YINIT) frog->y = newPos;
                break;

            case KEY_LEFT:
                newPos = frog->x - LARGHEZZARANA;
                if (newPos > RANA_XMIN) frog->x = newPos;
                break;

            case KEY_RIGHT:
                newPos = frog->x + LARGHEZZARANA;
                if (newPos < RANA_XMAX) frog->x = newPos;
                break;

            case 's':  // spara granate
                sparaGranata(game_struct->tempo, gameCfg);
                break;
        }
        pthread_mutex_unlock(&buffer.mutex); 
    }
    pthread_exit(NULL);
}

void sparaGranata(const float time, const gameConfig* gameConfig) {

    bool allowed = true;
    for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){
        const Projectile* obj = (Projectile*)buffer.buffer[i];
        if(obj->alive) allowed = false;
    }

    if (allowed){
        const Frog* frog = (Frog*)buffer.buffer[IDX_RANA];

        for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){
            Projectile* proj = (Projectile*)buffer.buffer[i];
            
            proj->x = frog->x;
            proj->y = frog->y;
            proj->alive = 1;
            proj->tempo_prec = time;
            proj->dir = 2*(i%2) -1;
            proj->speed = gameConfig->velocità_proiettili;
        }
    }
}

//GESTIONE GRANATE-----------------------------------------
void GranateInit(){
    const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

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

    }
}

void* thread_granata(void* arg) {

    while(1){

        pthread_mutex_lock(&buffer.mutex);

        const Game_struct* game_struct = (Game_struct*)buffer.buffer[IDX_GAME];

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            pthread_mutex_unlock(&buffer.mutex);
            break;
        }

        for (int i = IDX_GRANATE; i < IDX_GRANATE + 2; i++){
            Projectile* gran = (Projectile*)buffer.buffer[i];

            if (!gran->alive) continue;

            if (CollGranataProiettile(gran)) continue;

            int newX = gran->x + gran->dir * gran->speed * (gran->tempo_prec - game_struct->tempo);
            if (newX > 0 && newX < LARGHEZZA_GIOCO && !CollGranataProiettile(gran)){
                gran->x = newX;
                gran->tempo_prec = game_struct->tempo;
            }
            else gran->alive=0;
        }

        pthread_mutex_unlock(&buffer.mutex); 
    }
    pthread_exit(NULL);
}

