#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tablero.h"
#include "entidades.h"

static Celda *nueva_celda(void) {
    Celda *c = (Celda *)malloc(sizeof(Celda));
    if (c) {
        c->alien = NULL;
        c->dano_pend = 0;
    }
    return c;
}

Tablero *tablero_crear(int ancho, int alto) {
    Tablero *t = (Tablero *)malloc(sizeof(Tablero));
    if (!t) return NULL;
    t->W = ancho;
    t->H = alto;

    // void ***: filas -> columnas -> (void*)
    t->celdas = (void ***)malloc(sizeof(void **) * t->H);
    if (!t->celdas) { free(t); return NULL; }
    for (int y = 0; y < t->H; ++y) {
        t->celdas[y] = (void **)malloc(sizeof(void *) * t->W);
        if (!t->celdas[y]) {
            for (int k = 0; k < y; ++k) free(t->celdas[k]);
            free(t->celdas);
            free(t);
            return NULL;
        }
        for (int x = 0; x < t->W; ++x) {
            Celda *cel = nueva_celda();
            t->celdas[y][x] = (void *)cel; // (void*) -> Celda*
        }
    }
    return t;
}

void tablero_cerrar(Tablero *t) {
    if (!t) return;
    if (t->celdas) {
        for (int y = 0; y < t->H; ++y) {
            if (!t->celdas[y]) continue;
            for (int x = 0; x < t->W; ++x) {
                Celda *cel = (Celda *)t->celdas[y][x];
                free(cel);
            }
            free(t->celdas[y]);
        }
        free(t->celdas);
    }
    free(t);
}

static char render_celda(struct Juego *juego, int x, int y) {
    (void)juego; // sólo para player y HUD
    Tablero *t = juego->t;
    Celda *cel = (Celda *)t->celdas[y][x];
    if (y == 0 && x == juego->jugador_x) {
        if (cel->dano_pend > 0) return 'X';
        return '^'; // jugador
    }
    if (cel->dano_pend > 0) return 'X';
    if (cel->alien == NULL) return ' ';
    switch (cel->alien->tipo) {
        case AL_DRONE: return 'D';
        case AL_SKATER: return 'S';
        default: return 'E';
    }
}

void tablero_imprimir(struct Juego *juego) {
    Tablero *t = juego->t;
    // Limpieza simple de pantalla
    printf("\033[2J\033[H");
    for (int y = t->H - 1; y >= 0; --y) {
        for (int x = 0; x < t->W; ++x) {
            char c = render_celda(juego, x, y);
            printf("[%c]", c);
        }
        printf("\n");
    }

    // HUD
    int inventario_restante = juego->pool.drone + juego->pool.skater + juego->pool.especial;
    printf("Turno: %d | Inv: %d | Vivos: %d | Mun (1/2/3): INF/%d/%d\n",
           juego->turno, inventario_restante, juego->vivos,
           juego->armas.ammo_perforador, juego->armas.ammo_especial);
    if (juego->msg[0] != '\0') {
        printf("%s\n", juego->msg);
    }

    // limpiar indicadores de daño para el próximo render
    for (int y = 0; y < t->H; ++y) {
        for (int x = 0; x < t->W; ++x) {
            Celda *cel = (Celda *)t->celdas[y][x];
            cel->dano_pend = 0;
        }
    }
}

