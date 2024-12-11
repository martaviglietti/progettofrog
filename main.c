#define MAX_CROCODILES 7
void createCrocodile(int pipeGame[2], pid_t *pid);

int main(){
    initscr(); noecho(); curs_set(0);
    cbreak(); 
    int pipe_fds[2];
    //WINDOW *game; creazione finestra di gioco da continuare

//FUNZIONE ciao

    
    crocodile crocodiles[MAX_CROCODILES];
    pid_t frog_pid;


    //creazione della pipe
     while (true) {
        int pipe[2];
        if (pipe(pipe) == -1) { // gestisco possibile errore
            perror("errore crazione pipe");
            exit(1);
        }

  // Creazione iniziale dei coccodrilli
    for (int i = 0; i < MAX_CROCODILES; i++) {
        createCrocodile(pipe_fds, &crocodiles[i].pid);
    }

//creazione della rana
frog_pid = createFrog(pipe_fds);
    if (frog_pid == -1) {
        perror("Errore nella creazione del processo rana");
        exit(1);
    }





//creazione dei processi
pid = fork(); // creo il processo che gestisce la rana

        if (pid == -1) {
            perror("Chiamata fork fallita (rana)");
            exit(1);
        } else if (pid == 0) {
            frog(); // processo figlio rana
        } else {
            pid2 = fork(); // creo il processo che gestisce i coccodrilli

            if (pid2 == -1) {
                perror("Chiamata fork fallita (coccodrilli)");
                exit(1);
            } else if (pid2 == 0) {
                river(pipeGame); // processo figlio coccodrilli
            } else {
                pid3 = fork(); // creo il processo che gestisce le piante

            





}

void controllo(int pipe[2], ){








}