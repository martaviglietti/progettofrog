#include"header.h"

//GESTIONE RANA-------------------------------------------------------------------------------------------------------------
void frogInit(){
    messaggio* this = &buffer.buffer[IDX_RANA];
    this->x = 40;
    this->y = 43;
}

void* thread_rana(void* arg) {
    Game_struct* game_struct = (Game_struct*)arg;
    WINDOW* game = stdscr;  // usa la finestra principale (in alternativa puoi passarla nel struct)

    int key;
    messaggio m;

    keypad(game, true);  // abilita frecce
    nodelay(game, FALSE); // aspetta input (puoi metterlo TRUE se vuoi non bloccare il loop)

    while (1) {
        key = wgetch(game);

        m.id = IDRANA;
        m.x = 0;
        m.y = 0;
        m.dir = 0;
        m.alive = true;

        switch (key) {
            case KEY_UP:
                m.y = -3;
                produttore(m);
                break;

            case KEY_DOWN:
                m.y = 3;
                produttore(m);
                break;

            case KEY_LEFT:
                m.x = -2;
                produttore(m);
                break;

            case KEY_RIGHT:
                m.x = 2;
                produttore(m);
                break;

            case 's':  // spara granate
                m.id = IDGRANATE + 2;  // messaggio  che attiva le granate
                produttore(m);
                break;
        }
    }

    pthread_exit(NULL);
}

void sparaGranata(int x, int y, int velocita) {
    messaggio* m1 = malloc(sizeof(messaggio));
    messaggio* m2 = malloc(sizeof(messaggio));

    pthread_t t_granata;
    *m1 = (messaggio){ .id = IDGRANATE,     .x = x - 1, .y = y - 1, .info = -1, .speed = velocita, .alive = true };
    *m2 = (messaggio){ .id = IDGRANATE + 1, .x = x + 1, .y = y - 1, .info = 1,  .speed = velocita, .alive = true };
    //pthread_create(NULL, NULL, thread_granata, (void*)m1);
    pthread_create(&t_granata, NULL, thread_granata, (void*)m1);
    pthread_create(&t_granata, NULL, thread_granata, (void*)m2);
}


//GESTIONE GRANATE-----------------------------------------
void GranateInit(){
    for (int i = IDX_GRANATE; i< IDX_GRANATE + 2; i++){
        messaggio* this = &buffer.buffer[i];
        this->x = -1;
        this->y = -1;
        this->alive = 0;
    }
}

void* thread_granata(void* arg) {
    messaggio* m = (messaggio*)arg;
    while (m->alive && m->y > 0 && m->x > 0 && m->x < LARGHEZZA_GIOCO - 1) {
        produttore(*m);  // invia posizione nel buffer
        m->x += m->dir;  // le granate si muovono in orizzontale
        usleep(m->speed);
    }
    free(m);
    pthread_exit(NULL);
}

