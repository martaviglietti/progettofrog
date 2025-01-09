#ifndef MENU_H
#define MENU_H

#include <ncurses.h> // Per il tipo WINDOW

int menu(WINDOW *game);
void credits(WINDOW *game);
void startGame(WINDOW *game); // Dichiarazione della funzione startGame

#endif
