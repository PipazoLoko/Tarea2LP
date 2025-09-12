#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "spawn.h"
#include "main.h"
#include "entidades.h"
#include "tablero.h"

static Alien *nuevo_alien(int tipo, int x, int y) {
    Alien *a = (Alien *)malloc(sizeof(Alien));
    a->tipo = tipo;
    a->x = x; a->y = y; a->dx = (rand() % 2) ? +1 : -1;
    switch (tipo) {
        case AL_DRONE: a->hp = 2; break;
        case AL_SKATER: a->hp = 1; break;
        default: a->hp = 2; break; // especial
    }
    return a;
}

static int total_pool(PoolAliens *p) { return p->drone + p->skater + p->especial; }

static int elegir_tipo(PoolAliens *p) {
    int tot = total_pool(p);
    if (tot <= 0) return -1;
    int r = rand() % tot;
    if (r < p->drone) return AL_DRONE;
    r -= p->drone;
    if (r < p->skater) return AL_SKATER;
    return AL_ESPECIAL;
}

static int columna_libre_superior(struct Juego *j) {
    // Devuelve columna libre en y = H-1, o -1 si está llena
    int intentos = 16; // evitar bucles
    while (intentos--) {
        int x = rand() % j->t->W;
        Celda *c = (Celda *)j->t->celdas[j->t->H - 1][x];
        if (c->alien == NULL) return x;
    }
    // búsqueda lineal por si falló el azar
    for (int x = 0; x < j->t->W; ++x) {
        Celda *c = (Celda *)j->t->celdas[j->t->H - 1][x];
        if (c->alien == NULL) return x;
    }
    return -1;
}

static void instanciar_en_superior(struct Juego *j, int tipo) {
    if (tipo < 0) return;
    int x = columna_libre_superior(j);
    if (x < 0) return; // fila superior llena => posponer
    int y = j->t->H - 1;
    Alien *a = nuevo_alien(tipo, x, y);
    Celda *c = (Celda *)j->t->celdas[y][x];
    c->alien = a;
    j->vivos++;
    switch (tipo) {
        case AL_DRONE: j->pool.drone--; break;
        case AL_SKATER: j->pool.skater--; break;
        case AL_ESPECIAL: j->pool.especial--; break;
    }
}

void spawn_inicio(struct Juego *j) {
    // configuración por dificultad
    if (j->dificultad == 0) {
        j->pool.drone = 6; j->pool.skater = 4; j->pool.especial = 5; // total 15
        j->pool.vivos_tope = 6;
    } else {
        j->pool.drone = 8; j->pool.skater = 6; j->pool.especial = 6; // total 20
        j->pool.vivos_tope = 8;
    }

    // sembrado inicial: 2 (Fácil) / 3 (Difícil)
    int iniciales = (j->dificultad == 0) ? 2 : 3;
    for (int i = 0; i < iniciales; ++i) {
        int tipo = elegir_tipo(&j->pool);
        instanciar_en_superior(j, tipo);
    }
}

void spawn_turno(struct Juego *j) {
    if (total_pool(&j->pool) <= 0) return;
    if (j->vivos >= j->pool.vivos_tope) return;
    int tipo = elegir_tipo(&j->pool);
    instanciar_en_superior(j, tipo);
    // 30% intentar un segundo
    if (total_pool(&j->pool) > 0 && j->vivos < j->pool.vivos_tope) {
        if ((rand() % 100) < 30) {
            tipo = elegir_tipo(&j->pool);
            instanciar_en_superior(j, tipo);
        }
    }
}

