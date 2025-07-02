#include"header.h"

//GESTIONE RANA-------------------------------------------------------------------------------------------------------------
void frogInit(){

    buffer.buffer[IDX_RANA] = malloc(sizeof(Frog));
    if (buffer.buffer[IDX_RANA]== NULL) {
        fprintf(stderr, "malloc failed at Frog Initialization at index %d\n", IDX_RANA);
        exit(EXIT_FAILURE);
    }
    
    Frog* obj = (Frog*)buffer.buffer[IDX_RANA];
    obj->x = 40;
    obj->y = 43;
}

void* thread_rana(void* arg) {
    
    ThreadArgs* args = (ThreadArgs*)arg;
    Game_struct* game_struct = args->Game_struct;
    WINDOW* game = game_struct->game;
    gameConfig* gameConfig = args->gameConfig;

    while(1){
        if ((game_struct->win) ||  (game_struct->vite == 0)){
            break;
        }

        keypad(game, true);  // abilita frecce
        nodelay(game, FALSE); // aspetta input (puoi metterlo TRUE se vuoi non bloccare il loop)

        while (1) {

            int key = wgetch(game);

            pthread_mutex_lock(&buffer.mutex);

            const float* time = (float*)buffer.buffer[IDX_TEMPO];

            Frog* obj = (Frog*)buffer.buffer[IDX_RANA];
            if (!obj->alive) continue;

            int newPos;
            switch (key) {
                case KEY_UP:
                    newPos = obj->y - ALTEZZARANA;
                    if (newPos > 0) obj->y = newPos;
                    else obj->alive = 0; 
                    break;

                case KEY_DOWN:
                    newPos = obj->y + ALTEZZARANA;
                    if (newPos < ALTEZZA_GIOCO) obj->y = newPos;
                    else obj->alive = 0;
                    break;

                case KEY_LEFT:
                    newPos = obj->x - LARGHEZZARANA;
                    if (newPos > 0) obj->x = newPos;
                    else obj->alive = 0;
                    break;

                case KEY_RIGHT:
                    newPos = obj->x + LARGHEZZARANA;
                    if (newPos < LARGHEZZA_GIOCO) obj->x = newPos;
                    else obj->alive = 0;
                    break;

                case 's':  // spara granate
                    sparaGranata(*time, gameConfig);
                    break;
            }
            pthread_mutex_unlock(&buffer.mutex); 
        }
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
        const Frog* rana = (Frog*)buffer.buffer[IDX_RANA];

        for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){
            Projectile* obj = (Projectile*)buffer.buffer[i];
            
            obj->x = rana->x;
            obj->y = rana->y;
            obj->alive = 1;
            obj->tempo_prec = time;
            obj->dir = 2*(i%2) -1;
            obj->speed = gameConfig->velocità_proiettili;
        }
    }
}


//GESTIONE GRANATE-----------------------------------------
void GranateInit(){
    const float* time = (float*)buffer.buffer[IDX_TEMPO];

    for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){

        buffer.buffer[i] = malloc(sizeof(Projectile));
        if (buffer.buffer[i]== NULL) {
            fprintf(stderr, "malloc failed at Granade Initialization at index %d\n", i);
            exit(EXIT_FAILURE);
        }

        Projectile* obj = (Projectile*)buffer.buffer[i];
        obj->x = -1;
        obj->y = -1;
        obj->alive = 0;
        obj->tempo_prec = *time;
    }
}

void* thread_granata(void* arg) {

    ThreadArgs* args = (ThreadArgs*)arg;
    Game_struct* game_struct = args->Game_struct;

    while(1){

        if ((game_struct->win) ||  (game_struct->vite == 0)){
            break;
        }

        pthread_mutex_lock(&buffer.mutex);

        const float* time = (float*)buffer.buffer[IDX_TEMPO];

        for (int i = IDX_GRANATE; i < IDX_GRANATE + 2; i++){
            Projectile* obj = (Projectile*)buffer.buffer[i];

            if (!obj->alive) continue;

            int newX = obj->x + obj->dir * obj->speed * (obj->tempo_prec - *time);
            if (newX > 0 && newX < LARGHEZZA_GIOCO){
                obj->x = newX;
                obj->tempo_prec = *time;
            }
            else obj->alive=0;
        }

        pthread_mutex_unlock(&buffer.mutex); 
    }
    pthread_exit(NULL);
}

