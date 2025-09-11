// main.h — Juego y ciclo principal
#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stddef.h>

// Forward declarations para evitar ciclos de include
struct Tablero; // definido en tablero.h
#include "armas.h"
#include "spawn.h"

typedef struct Juego {
    struct Tablero *t;      // encapsula W/H y política de memoria/render
    Armas armas;     // inventario de armas y punteros a función
    PoolAliens pool; // inventario único por partida
    int dificultad;  // 0: fácil, 1: difícil
    int turno;       // turno actual
    int vivos;       // aliens vivos actualmente en el tablero
    int jugador_x;   // posición x del jugador (y = 0)
    char msg[128];   // mensaje HUD del último evento
} Juego;

// Inicializa y libera el juego
bool juego_iniciar(Juego *juego, int dificultad);
void juego_cerrar(Juego *juego);

// Ejecuta el ciclo de turnos hasta victoria/derrota
void juego_correr(Juego *juego);

#endif // MAIN_H
