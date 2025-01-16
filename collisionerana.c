

bool frogOnCrocodile(rana *frog, Coccodrillo *coccodrilli, int maxCoccodrilli) {
    for (int i = 0; i < maxCoccodrilli; i++) {
        if (coccodrilli[i].alive) {
            int crocodileLeft = coccodrilli[i].x;
            int crocodileRight = coccodrilli[i].x + LARGHEZZA_COC;
            int frogLeft = frog->x;
            int frogRight = frog->x + LARGHEZZARANA;

            if (frog->y == coccodrilli[i].y && frogLeft >= crocodileLeft && frogRight <= crocodileRight) {
                frog->moving = true;
                frog->x += coccodrilli[i].dir;
                return true; // La rana è sopra un coccodrillo
            }
        }
    }

    frog->moving = false;
    return false; // Nessuna collisione
}


 //controllo se la rana si trova sul coccodrillo o sul fiume  

void updateFrogState(Rana *frog, Coccodrillo *coccodrilli, int maxCoccodrilli, int screenWidth) {
    // Controlla se la rana è nel fiume
    if (frog->y >= 16 && frog->y <= 37) { // Intervallo del fiume
        int collidedCrocID = frogOnTopOfCrocodrile(frog, coccodrilli, maxCoccodrilli);

        if (collidedCrocID != -1) {
            // La rana è sopra un coccodrillo
            frog->moving = 1; // La rana si muove con il coccodrillo
            frog->x += coccodrilli[collidedCrocID].dir;

            // Controllo dei limiti dello schermo
            if (frog->x < 1 || frog->x > screenWidth - LARGHEZZARANA - 1) {
                frog->moving = 0;  // La rana smette di muoversi
                frog->alive = 0;   // La rana affoga
                frog->y = 43;      // Torna al marciapiede
                decrementaVite();  // Perdita di una vita
            }
        } else {
            // La rana non è sopra un coccodrillo
            frog->moving = 0;
            frog->alive = 0;   // La rana affoga
            frog->y = 43;      // Torna al marciapiede
            decrementaVite();  //funzione per gestire le vite? come si gestisce?
        }
    } else {
        // La rana non è nel fiume
        frog->moving = 0;
    }
}
