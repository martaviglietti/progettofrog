
#include "header.h"

//creo un buffer produttore-consumatore che permette ai threads prodttori
//di scrivere dati e a quelli consumatori ddi leggere dati in modo sincornizzato



//il produttore inserisce un nuovo oggetto nel buffer
void produttore(messaggio m){
    sem_wait(&buffer.full); //aspetta che ci sia almeno uno slot vuoto
    pthread_mutex_lock(&buffer.mutex);     // entra nella sezione critica
    buffer.buffer[buffer.in] = m;  //scrive il messaggio nel buffer
    buffer.in = (buffer.in + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&buffer.mutex);   // esce dalla sezione critica
    sem_post(&buffer.empty);               // segnala che c’è un nuovo messaggio disponibile

}

//il consumatore estrae un oggetto dal buffer per usarlo 
messaggio consumatore() {
    sem_wait(&buffer.empty);               // aspetta che ci sia almeno un messaggio nel buffer
    pthread_mutex_lock(&buffer.mutex);     // entra nella sezione critica

    messaggio m = buffer.buffer[buffer.out]; // legge il messaggio
    buffer.out = (buffer.out + 1) % BUFFER_SIZE; // avanza in modo circolare

    pthread_mutex_unlock(&buffer.mutex);   // esce dalla sezione critica
    sem_post(&buffer.full);                // segnala che c’è uno slot vuoto

    return m;
}


void inizializza_buffer() {
    buffer.in = 0;
    buffer.out = 0;
    sem_init(&buffer.full, 0, BUFFER_SIZE);
    sem_init(&buffer.empty, 0, 0);
    pthread_mutex_init(&buffer.mutex, NULL);
}



//CREAZIONE THREAD PRINCIPALI GIOCO -------------------------------------------------------------------------
void crea_thread_gioco(Game_struct* game_struct, Flusso flussi[], int vel_proiettile, int tempo, WINDOW* game){
     // Thread grafica/consumatore
     ArgGrafica* arg = malloc(sizeof(ArgGrafica));
    if (!arg) {
    perror("Errore malloc ArgGrafica");
    exit(EXIT_FAILURE);
    }
    arg->game_struct = game_struct;
    arg->game = game;
    arg->vel_proiettile = vel_proiettile;
    pthread_t t_grafica, t_rana, t_tempo;


    pthread_create(&t_grafica, NULL, Gestione_grafica, (void*)arg);

    // Thread rana (input tastiera)
    pthread_create(&t_rana, NULL, thread_rana, (void *)game_struct);

    // Thread tempo
    int *tempo_ptr = malloc(sizeof(int));
    *tempo_ptr = tempo;
    pthread_create(&t_tempo, NULL, thread_tempo, tempo_ptr);
    crea_thread_coccodrilli(flussi, game_struct);
    
    // NB: Thread granate e proiettili vanno creati dinamicamente quando necessario (non qui)
}


void* thread_tempo(void* arg) {
    int tempo_totale = *(int*)arg;
    int tempo_rimanente = tempo_totale;

    while (tempo_rimanente >= 0) {
        messaggio m;
        m.id = IDTIME;
        m.x = tempo_rimanente;  // usiamo `x` per comunicare il tempo rimasto
        m.y = 0;
        m.dir = 0;
        m.speed = 0;
        m.alive = true;  // non important
        produttore(m);  // usa il buffer già condiviso
        sleep(1);
        tempo_rimanente--;
    }

    pthread_exit(NULL);
}







//GESTIONE GRANATE-----------------------------------------

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

//---------------------------------------------------------------------------------------------------------------------------

//GESTIONE RANA-------------------------------------------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------------------------------------------------