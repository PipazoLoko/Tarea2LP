#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stddef.h>

struct Tablero; // declaracion anticipada
#include "armas.h"
#include "spawn.h"

typedef struct Juego {
    struct Tablero *t;      // encapsula W/H y política de memoria/render
    Armas armas;     // arsenal del jugador (puntero a funcion + municion)
    PoolAliens pool; // reserva de aliens fija por partida
    int dificultad;  // 0: facil, 1: dificil
    int turno;       // turno actual
    int vivos;       // aliens vivos actualmente en el tablero
    int jugador_x;   // posicion x del jugador (y = 0)
    char mensaje_hud[100];   // mensaje HUD, del ultimo evento
    int fin;         // flag de fin
} Juego;

// inicializa y libera el juego
bool juego_iniciar(Juego *juego, int dificultad);
void juego_cerrar(Juego *juego);

// ejecuta el ciclo de turnos hasta victoria/derrota
void juego_correr(Juego *juego);

#endif // MAIN_H
