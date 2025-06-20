#include "header.h"

extern pthread_mutex_t mutex_tane;


void* Gestione_grafica(void* arg){
    ArgGrafica* dati = (ArgGrafica*)arg;
    Game_struct* game_struct = dati->game_struct;
    WINDOW* game = dati->game;
    int vel_proiettile = dati->vel_proiettile;

    free(dati);
    //Variabili per gestione proiettile
    struct timeval start, end; 
    gettimeofday(&end,NULL);		
    gettimeofday(&start,NULL);

    long time_proiettile; 
    int random=rand_funz(2,4);

    Temp temp={-1,0,0,0};
    

    //Inizializzazione degli elementi di gioco
    Coccodrillo coccodrilli[MAX_CROCODILES];
    for (int i=0; i<MAX_CROCODILES;i++) {
        coccodrilli[i].id=i;      //id coccodrillo
        coccodrilli[i].x=-1;	  //posizione x coccodrillo
        coccodrilli[i].y=-1;      //posizione y coccodrillo
        coccodrilli[i].dir=0;     //direzione coccodrillo
        coccodrilli[i].alive=0;   //stato del coccoddrillo
        coccodrilli[i].wait=-1;}  //variabile per stato di attesa

    Granata granate[2];
    for (int i=0; i<2;i++) {
	granate[i].id=i;
	granate[i].x=-1;
	granate[i].y=-1;
	granate[i].alive=0;}
		
    Rana rana;	
    rana.id=IDRANA;
    rana.x=40; 
    rana.y=43;

    Proiettile proiettile;
    proiettile.id=IDPROIETTILE;
    proiettile.x=50;
    proiettile.y=-1;
    proiettile.alive=0;
   
    int tempo=game_struct->tempo;

    
    int riattivare;
    int distanze_coc[8];
    for (int i=0; i<8;i++) {
	distanze_coc[i]=rand_funz(13,16);  //impostiamo le distanze iniziali tra coccodrilli per ogni flusso del fiume
    }
    int indice_flusso;

    while(true){
        messaggio temp = consumatore();  // Legge un messaggio dal buffer condiviso
        //se l'ID è della rana						
         if (temp.id == IDRANA && RanaInFinestra(rana, temp)) {
            rana.x += temp.x;
            rana.y += temp.y;
            if (rana.y < 10) {
                if (RanaSuTana(rana, game_struct)) {
                    game_struct->score += 15 + (int)(15 * (float)game_struct->tempo / 100);
                    break;
                } else {
                    game_struct->vite--;
                    game_struct->score -= 10;
                    break;
                }
            }
        }
        //se l'id è del TEMPO
        if (temp.id==IDTIME) {	
            game_struct->tempo-=1;
            if (game_struct->tempo==0) {  //controllo per verificare che il tempo non sia a zero
                game_struct->vite--;
            game_struct->score-=20;
            return 0;
            }
	    }

        // se l'ID è di un coccodrillo (thread produttore)
        if (temp.id >= 0 && temp.id < MAX_CROCODILES) {
            int id_coc = temp.id;
            int coc_scelto = RanaSuCoccodrillo(&rana, coccodrilli);
            coccodrilli[id_coc].x = temp.x;
            coccodrilli[id_coc].y = temp.y;
            coccodrilli[id_coc].dir = temp.info;

            // controllo movimento rana con il coccodrillo
            if (coc_scelto == coccodrilli[id_coc].id) {
                if ((rana.x <= 2 && coccodrilli[id_coc].dir == -1) || 
                    (rana.x >= LARGHEZZA_GIOCO - 3 && coccodrilli[id_coc].dir == 1)) {
                    // la rana è ai bordi: verifica se è ancora sopra il coccodrillo
                    if (RanaSuCoccodrillo(&rana, coccodrilli) != coc_scelto) {
                        game_struct->vite--;
                        game_struct->score -= 10;
                        return 0;
                    }
                } else {
                    // la rana si muove insieme al coccodrillo
                    rana.x += coccodrilli[id_coc].dir;
                }
            }
    
        // aggiornamento stato coccodrillo: vivo/morto
            if ((coccodrilli[id_coc].x >= POS_SPAWN_COC_DESTRA && coccodrilli[id_coc].dir == 1) ||
                (coccodrilli[id_coc].x <= POS_SPAWN_COC_SINISTRA && coccodrilli[id_coc].dir == -1)) {
                coccodrilli[id_coc].alive = 0;  // fuori mappa
            } else {
                coccodrilli[id_coc].alive = 1;  // dentro mappa
            }
            int indice_flusso = (coccodrilli[id_coc].y - 37) / -3;
            for (int i = 0; i < MAX_CROCODILES; i++) { 
                if (i != id_coc &&
                    coccodrilli[i].y == coccodrilli[id_coc].y &&
                    ((coccodrilli[id_coc].x - distanze_coc[indice_flusso] < coccodrilli[i].x && coccodrilli[id_coc].dir == -1) ||
                    (coccodrilli[id_coc].x + distanze_coc[indice_flusso] > coccodrilli[i].x && coccodrilli[id_coc].dir == 1)) &&
                    coccodrilli[i].wait != 1) {
                    coccodrilli[id_coc].wait = 1;  // metti il coccodrillo in stato di attesa
                    break;
                }
            }
            // controllo se qualche coccodrillo è in attesa
            for (int i = 0; i < MAX_CROCODILES; i++) {  
                if (coccodrilli[i].wait == 1) {
                    int indice_flusso = (coccodrilli[i].y - 37) / -3;
                    int riattivare = 1;
                    for (int j = 0; j < MAX_CROCODILES; j++) {
                        if (i != j &&
                            coccodrilli[j].y == coccodrilli[i].y &&
                            ((coccodrilli[i].x - distanze_coc[indice_flusso] < coccodrilli[j].x && coccodrilli[i].dir == -1) ||
                            (coccodrilli[i].x + distanze_coc[indice_flusso] > coccodrilli[j].x && coccodrilli[i].dir == 1)) &&
                            coccodrilli[j].wait != 1) {
                            riattivare = 0;
                            break;
                        }
                    }
                    if (riattivare) {
                        distanze_coc[indice_flusso] = rand_funz(13, 16);  // aggiorna distanza
                        coccodrilli[i].wait = 0;  // togli flag di attesa → il thread riprenderà in autonomia
                    }
                }
            }
        }
    
        // Gestione delle granate
        if (temp.id >= IDGRANATE && temp.id <= IDGRANATE + 2) {
            if (temp.id == IDGRANATE + 2) {  // richiesta di sparare
                if (granate[0].alive == 0 && granate[1].alive == 0) {
                    sparaGranata(rana.x, rana.y, vel_proiettile);  // chiamata a funzione modificata
                    granate[0].alive = 1;
                    granate[1].alive = 1;
                }
            } else {
                 // Aggiornamento posizione
                int i = temp.id - IDGRANATE;
                granate[i].x = temp.x;
                granate[i].y = temp.y;
            }
        }

        //PROIETTILE
        if (temp.id == IDPROIETTILE) {
        proiettile.x = temp.x;
        proiettile.y = temp.y;
        }


       // Collisioni proiettile con i bordi
        if ((proiettile.x < 1 || proiettile.x > LARGHEZZA_GIOCO - 2) && proiettile.alive == 1) {
            proiettile.alive = 0;
        }


        // Controllo uscita granate dai bordi
        for (int i = 0; i < 2; i++) {
            if ((granate[i].x < 1 || granate[i].x > LARGHEZZA_GIOCO - 2) &&
                granate[i].alive == 1 && granate[i].x != -1) {
                granate[i].alive = 0;
                granate[i].x = -1;
            }
        }


        // Controllo collisione rana-proiettile
        if (CollisioneRanaProiettile(rana, proiettile) == 1 && proiettile.alive == 1) {
            game_struct->vite--;
            game_struct->score -= 15;
            proiettile.alive = 0;  // disattiva il proiettile
            proiettile.x = -1;     // posizione fittizia per toglierlo dallo schermo
            return;
        }


            // Controllo collisione granata-proiettile
        for (int i = 0; i < 2; i++) {
            if (granate[i].x == proiettile.x &&
                granate[i].y == proiettile.y &&
                proiettile.alive == 1 &&
                granate[i].alive == 1) {

                // disattiva entrambi
                proiettile.alive = 0;
                granate[i].alive = 0;
                proiettile.x = -1;
                granate[i].x = -1;

            // aumento punteggio
                game_struct->score += 5;
            }
        }

        if (granate[0].alive == 0 && granate[1].alive == 0 && thread_granata_attivo) {
        thread_granata_attivo=0; // Segnala al thread granate di terminare
        // puoi anche usare pthread_cancel se proprio necessario (meno elegante)
        }

            // GESTIONE DELLO SPARO DEL PROIETTILE
        if (rana.y < 40 && rana.x >= 15) {
            gettimeofday(&end, NULL);

            for (int i = 0; i < MAX_CROCODILES; i++) {
                if (coccodrilli[i].alive == 1 &&
                    coccodrilli[i].y == rana.y &&
                    ((coccodrilli[i].x + 4 < rana.x - 6 && coccodrilli[i].dir == 1) ||
                    (coccodrilli[i].x - 4 > rana.x + 6 && coccodrilli[i].dir == -1))) {

                    if (end.tv_sec - start.tv_sec >= random) {
                        if (rand_funz(1,1) == 1) {
                            sparaProiettile_daCoccodrillo(coccodrilli[i], vel_proiettile);
                        }

                        gettimeofday(&start, NULL);
                        random = rand_funz(2, 4);
                    }
                }
            }
        }

    }
    pthread_mutex_lock(&mutex_ncurses);
    werase(game);
    windowGeneration(game, COLS, LINES, game_struct);
    drawCoccodrilli(game, coccodrilli);
    draw_granate(game, granate);
    draw_proiettile(game, proiettile);
    draw_frog(game, rana);

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
    mvwprintw(game, 46, 2, "Tempo: %d ", game_struct->tempo);
    wattroff(game, COLOR_PAIR(15));

    print_tempo(game, game_struct, tempo);
    wrefresh(game);
    pthread_exit(NULL);
    pthread_mutex_unlock(&mutex_ncurses);
    }
        