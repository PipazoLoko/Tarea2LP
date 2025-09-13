// tablero.h — Tablero (void ***) + UI/HUD
#ifndef TABLERO_H
#define TABLERO_H

#include <stdbool.h>
#include "main.h"

typedef struct Celda Celda;// forward para Celda (definida en entidades.h), Ademas asigna alias de Celda al typedef struct celda

typedef struct Tablero {
    int W, H;
    // celdas[y][x] -> (void*) que apunta a Celda*
    void ***celdas;
} Tablero;

/*
 * Nombre: celda_at
 * Parametros: struct Tablero *t, int x, int y
 * Retorno: Celda* // puntero a la celda en [y][x])
 * Descripcion: acceso auxiliar a la matriz void*** del tablero. recibe coordenadas (x,y) y devuelve un puntero a la Celda almacenada en esa posición, realiza internamente el cast desde void*. 
 */
static inline Celda* celda_at(struct Tablero *t, int x, int y) {
    return (Celda*)t->celdas[y][x];
}

Tablero *tablero_crear(int ancho, int alto);
void tablero_cerrar(Tablero *tablero);
void tablero_imprimir(struct Juego *juego);

#endif // TABLERO_H
