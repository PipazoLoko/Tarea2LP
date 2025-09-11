#include <stdio.h>
#include <stdlib.h>

#include "armas.h"
#include "entidades.h"
#include "tablero.h"
#include "main.h"

static Celda *celda(struct Juego *j, int x, int y) {
    return (Celda *)j->t->celdas[y][x];
}

static int buscar_primer_alien_en_col(struct Juego *j, int col) {
    // desde y=0 (base) hacia arriba; jugador está en y=-1
    for (int y = 0; y < j->t->H; ++y) {
        if (celda(j, col, y)->alien) return y;
    }
    return -1;
}

bool arma_normal(struct Juego *j) {
    int c = j->jugador_x;
    int y = buscar_primer_alien_en_col(j, c);
    if (y >= 0) {
        celda(j, c, y)->dano_pend += 1;
        snprintf(j->msg, sizeof(j->msg), "Normal impacto en (%d,%d)", c, y);
    } else {
        snprintf(j->msg, sizeof(j->msg), "Normal: sin objetivo");
    }
    return true; // siempre procede
}

bool arma_perforador(struct Juego *j) {
    if (j->armas.ammo_perforador <= 0) {
        snprintf(j->msg, sizeof(j->msg), "Sin municion Perforador");
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
    snprintf(j->msg, sizeof(j->msg), "Perforador: %d impacto(s)", impactos);
    return true;
}

bool arma_especial(struct Juego *j) {
    if (j->armas.ammo_especial <= 0) {
        snprintf(j->msg, sizeof(j->msg), "Sin municion Especial");
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
        snprintf(j->msg, sizeof(j->msg), "Especial: golpe y reboto en %d,%d", c, y);
    } else {
        snprintf(j->msg, sizeof(j->msg), "Especial: sin objetivo");
    }
    j->armas.ammo_especial--;
    return true;
}

bool disparar_armas(struct Juego *j, int arma_id) {
    if (arma_id < 0 || arma_id > 2) {
        snprintf(j->msg, sizeof(j->msg), "Arma invalida");
        return false;
    }
    FuncArmas f = j->armas.fn[arma_id];
    if (!f) {
        snprintf(j->msg, sizeof(j->msg), "Arma no configurada");
        return false;
    }
    return f(j);
}
