#ifndef COLLISIONI_H
#define COLLISIONI_H

#include "main.h"  // Per ID, buffer, struct comuni
#include "FrogHandle.h"
#include "CrocHandle.h

#endif

int collisione_rana_proiettili(Rana* rana,Proiettile proiettile[], Statistiche * statistiche_gioco, int giocare);
int rana_su_tana(Rana*rana, Statistiche * statistiche_gioco);
int rana_su_coccodrillo(Rana *rana, Coccodrillo *coccodrilli);

void collisione_granate_confine( Granata* granate);
void collisione_proiettili_confine(Thread_id thread_id[], Proiettile proiettile[]);
void collisione_granate_proiettili(Granata* granate, Proiettile proiettile[], int array_pid[], Statistiche * statistiche_gioco);
int movimento_rana_su_coccodrillo(int id, int coccodrillo_scelto, Coccodrillo* coccodrilli, Rana* rana, Statistiche * statistiche_gioco , int giocare);
