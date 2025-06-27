#include"header.h"

//GESTIONE RANA-------------------------------------------------------------------------------------------------------------
void frogInit(){
    messaggio* this = &buffer.buffer[IDX_RANA];
    this->x = 40;
    this->y = 43;
}

void* thread_rana(void* arg) {
    
    ThreadArgs* args = (ThreadArgs*)arg;
    Game_struct* game_struct = args->Game_struct;
    WINDOW* game = game_struct->game;
    gameConfig* gameConfig = args->gameConfig;

    if ((game_struct->win) ||  (game_struct->vite == 0)){
        pthread_exit(NULL);
    }

    keypad(game, true);  // abilita frecce
    nodelay(game, FALSE); // aspetta input (puoi metterlo TRUE se vuoi non bloccare il loop)

    while (1) {

        int key = wgetch(game);

        pthread_mutex_lock(&buffer.mutex);

        messaggio* this = &buffer.buffer[IDX_RANA];
        if (!this->alive) continue;

        int newPos;
        switch (key) {
            case KEY_UP:
                newPos = this->y - ALTEZZARANA;
                if (newPos > 0) this->y = newPos;
                else this->alive = 0; 
                break;

            case KEY_DOWN:
                newPos = this->y + ALTEZZARANA;
                if (newPos < ALTEZZA_GIOCO) this->y = newPos;
                else this->alive = 0;
                break;

            case KEY_LEFT:
                newPos = this->x - LARGHEZZARANA;
                if (newPos > 0) this->x = newPos;
                else this->alive = 0;
                break;

            case KEY_RIGHT:
                newPos = this->x + LARGHEZZARANA;
                if (newPos < LARGHEZZA_GIOCO) this->x = newPos;
                else this->alive = 0;
                break;

            case 's':  // spara granate
                sparaGranata(game_struct, gameConfig);
                break;
        }
        pthread_mutex_unlock(&buffer.mutex); 
    }
}

void sparaGranata(Game_struct* game_struct, gameConfig* gameConfig) {

    bool allowed = true;
    for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){
        const messaggio* this = &buffer.buffer[i];
        if(this->alive) allowed = false;
    }

    if (allowed){
        const messaggio* rana = &buffer.buffer[IDX_RANA];

        for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){
            messaggio* this = &buffer.buffer[i];
            
            this->x = rana->x;
            this->y = rana->y;
            this->alive = 1;
            this->tempo_prec = game_struct->tempo;
            this->dir = 2*(i%2) -1;
            this->speed = gameConfig->velocità_proiettili;
        }
    }
}


//GESTIONE GRANATE-----------------------------------------
void GranateInit(Game_struct* game_struct){
    for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){
        messaggio* this = &buffer.buffer[i];
        this->x = -1;
        this->y = -1;
        this->alive = 0;
        this->tempo_prec = game_struct->tempo;
    }
}

void* thread_granata(void* arg) {

    ThreadArgs* args = (ThreadArgs*)arg;
    Game_struct* game_struct = args->Game_struct;

    if ((game_struct->win) ||  (game_struct->vite == 0)){
        pthread_exit(NULL);
    }

    pthread_mutex_lock(&buffer.mutex);

    for (int i = IDX_GRANATE; i < IDX_GRANATE + 2; i++){
        messaggio* this = &buffer.buffer[i];

        if (!this->alive) continue;

        int newX = this->x + this->dir * this->speed * (this->tempo_prec - game_struct->tempo);
        if (newX > 0 && newX < LARGHEZZA_GIOCO){
            this->x = newX;
            this->tempo_prec = game_struct->tempo;
        }
        else this->alive=0;
    }

    pthread_mutex_unlock(&buffer.mutex); 

}

