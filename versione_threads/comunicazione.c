
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
    //sem_init(&buffer.full, 0, BUFFER_SIZE);
    //sem_init(&buffer.empty, 0, 0);
    pthread_mutex_init(&buffer.mutex, NULL);
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











//---------------------------------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------------------------------