#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tablero.h"
#include "entidades.h"

/*
 * Nombre: nueva_celda
 * Parámetros: (ninguno)
 * Retorno: Celda* (puntero a celda inicializada)
 * Descripción: Reserva e inicializa una celda vacía con daño pendiente en 0.
 */
static Celda *nueva_celda(void) {
    Celda *c = (Celda *)malloc(sizeof(Celda));
    if (c) {
        c->alien = NULL;
        c->dano_pend = 0;
    }
    return c;
}

/*
 * Nombre: tablero_crear
 * Parámetros: int ancho, int alto
 * Retorno: Tablero* (puntero a tablero creado) o NULL en error
 * Descripción: Reserva el struct Tablero y su matriz void*** (H x W). Cada
 *              posición contiene un (void*) apuntando a Celda*.
 */
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

/*
 * Nombre: tablero_cerrar
 * Parámetros: Tablero *t
 * Retorno: void
 * Descripción: Libera todas las Celdas, la matriz void*** y el propio Tablero.
 */
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

/*
 * Nombre: render_celda
 * Parámetros: Juego *juego, int x, int y
 * Retorno: char (carácter representativo de la celda)
 * Descripción: Devuelve el carácter visible para la celda (alien, vacío o
 *              impacto 'X') según su contenido y daño pendiente.
 */
static char render_celda(struct Juego *juego, int x, int y) {
    Tablero *t = juego->t;
    Celda *cel = (Celda *)t->celdas[y][x];
    if (cel->dano_pend > 0) return 'X';
    if (cel->alien == NULL) return ' ';
    switch (cel->alien->tipo) {
        case AL_DRONE: return 'D';
        case AL_SKATER: return 'S';
        default: return 'E';
    }
}

/*
 * Nombre: tablero_imprimir
 * Parámetros: Juego *juego
 * Retorno: void
 * Descripción: Renderiza el tablero completo, la línea de base del jugador en
 *              y=-1 y el HUD persistente (incluye prompt si el juego no finalizó).
 */
void tablero_imprimir(struct Juego *juego) {
    Tablero *t = juego->t;
    // reposicionar cursor al inicio y limpiar hasta el final para
    // reescribir el frame sin desplazar la terminal
    printf("\033[H\033[J");
    for (int y = t->H - 1; y >= 0; --y) {
        for (int x = 0; x < t->W; ++x) {
            char c = render_celda(juego, x, y);
            printf("[%c]", c);
        }
        printf("\n");
    }

    // Línea de base (jugador en y = -1)
    for (int x = 0; x < t->W; ++x) {
        if (x == juego->jugador_x) printf(" ^ "); else printf("   ");
    }
    printf("\n");

    // HUD
    int inventario_restante = juego->pool.drone + juego->pool.skater + juego->pool.especial;
    printf("Turno: %d | Inv: %d | Vivos: %d | Mun (1/2/3): INF/%d/%d\n",
           juego->turno, inventario_restante, juego->vivos,
           juego->armas.ammo_perforador, juego->armas.ammo_especial);
    if (juego->mensaje_hud[0] != '\0') printf("%s\n", juego->mensaje_hud);
    // Prompt integrado, excepto cuando el juego terminó
    if (!juego->fin) {
        printf("Accion [A/D/1/2/3/Q]: ");
        fflush(stdout);
    } else {
        fflush(stdout);
    }

    // limpiar indicadores de daño para el próximo render
    for (int y = 0; y < t->H; ++y) {
        for (int x = 0; x < t->W; ++x) {
            Celda *cel = (Celda *)t->celdas[y][x];
            cel->dano_pend = 0;
        }
    }
}
