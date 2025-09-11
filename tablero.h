// tablero.h — Tablero (void ***) + UI/HUD
#ifndef TABLERO_H
#define TABLERO_H

#include <stdbool.h>
#include "main.h"

// Forward para Celda (definida en entidades.h)
typedef struct Celda Celda;

typedef struct Tablero {
    int W, H;
    // celdas[y][x] -> (void*) que apunta a Celda*
    void ***celdas;
} Tablero;

// Administración de memoria del tablero
Tablero *tablero_crear(int ancho, int alto);
void tablero_cerrar(Tablero *tablero);

// Render del tablero + HUD
void tablero_imprimir(struct Juego *juego);

#endif // TABLERO_H
