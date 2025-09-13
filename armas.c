#include <stdio.h>
#include <stdlib.h>

#include "armas.h"
#include "entidades.h"
#include "tablero.h"
#include "main.h"

/*
 * Nombre: celda
 * Parametros: Juego *j, int x, int y
 * Retorno: Celda* (puntero a la celda en (x,y))
 * Descripcion: Acceso auxiliar a la celda tipada a partir de la matriz void***.
 */
static Celda *celda(struct Juego *j, int x, int y) {
    return celda_at(j->t, x, y);
}

/*
 * Nombre: buscar_primer_alien_en_col
 * Parametros: Juego *j, int col
 * Retorno: int (fila y del primer alien; -1 si no hay)
 * Descripcion: Busca el primer alien visible desde la base hacia arriba en la
 *              columna indicada.
 */
static int buscar_primer_alien_en_col(struct Juego *j, int col) {
    // desde y=0 (base) hacia arriba; jugador esta en y=-1
    for (int y = 0; y < j->t->H; ++y) {
        if (celda(j, col, y)->alien) return y;
    }
    return -1;
}

/*
 * Nombre: arma_normal
 * Parametros: Juego *j
 * Retorno: bool (true: procede y consume turno)
 * Descripcion: Disparo recto con daño 1 al primer alien de la columna. Municion
 *              infinita.
 */
bool arma_normal(struct Juego *j) {
    int c = j->jugador_x;
    int y = buscar_primer_alien_en_col(j, c);
    if (y >= 0) {
        celda(j, c, y)->dano_pend += 1;
        snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Normal impacto en (%d,%d)", c, y);
    } else {
        snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Normal: sin objetivo");
    }
    return true; // siempre procede
}

/*
 * Nombre: arma_perforador
 * Parametros: Juego *j
 * Retorno: bool (true si procede; requiere municion)
 * Descripcion: Disparo que atraviesa y daña con 1 a todos los aliens de la
 *              columna actual. Consume municion.
 */
bool arma_perforador(struct Juego *j) {
    if (j->armas.ammo_perforador <= 0) {
        snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Sin municion Perforador");
        return false; // no procede
    }
    int c = j->jugador_x;
    int impactos = 0;
    for (int y = 1; y < j->t->H; ++y) {
        if (celda(j, c, y)->alien) {
            celda(j, c, y)->dano_pend += 1;
            impactos++;
        }
    }
    j->armas.ammo_perforador--;
    snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Perforador: %d impacto(s)", impactos);
    return true;
}

/*
 * Nombre: arma_especial
 * Parametros: Juego *j
 * Retorno: bool (true si procede; requiere municion)
 * Descripcion: Disparo especial con daño 2 al impacto y daño 1 en rebotes
 *              diagonales una fila arriba.
 */
bool arma_especial(struct Juego *j) {
    if (j->armas.ammo_especial <= 0) {
        snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Sin municion Especial");
        return false;
    }
    int c = j->jugador_x;
    int y = buscar_primer_alien_en_col(j, c);
    if (y >= 0) {
        // Daño principal 2
        celda(j, c, y)->dano_pend += 2;
        // Rebote diagonal a columnas vecinas una fila arriba (si existen)
        if (y + 1 < j->t->H) {
            if (c - 1 >= 0) celda(j, c - 1, y + 1)->dano_pend += 1;
            if (c + 1 < j->t->W) celda(j, c + 1, y + 1)->dano_pend += 1;
        }
        snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Especial: golpe y reboto en %d,%d", c, y);
    } else {
        snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Especial: sin objetivo");
    }
    j->armas.ammo_especial--;
    return true;
}

/*
 * Nombre: disparar_armas
 * Parametros: Juego *j, int arma_id (0..2)
 * Retorno: bool (true si el disparo se ejecuto)
 * Descripcion: Valida el id y resuelve el disparo via arreglo de punteros a
 *              funcion (armas.fn).
 */
bool disparar_armas(struct Juego *j, int arma_id) {
    if (arma_id < 0 || arma_id > 2) {
        snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Arma invalida");
        return false;
    }
    FuncArmas f = j->armas.fn[arma_id];
    if (!f) {
        snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Arma no configurada");
        return false;
    }
    return f(j);
}
