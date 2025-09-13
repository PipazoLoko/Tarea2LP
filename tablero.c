#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tablero.h"
#include "entidades.h"

/*
 * Nombre: nueva_celda
 * Parametros: ninguno
 * Retorno: Celda* / puntero a celda inicializada
 * Descripcion: reserva memoria para crer una Celda vacia (alien=NULL, dano_pend=0)
 */
static Celda *nueva_celda(void) {
    Celda *c = malloc(sizeof *c);
    c->alien = NULL;
    c->dano_pend = 0;
    return c;
}

/*
 * Nombre: tablero_crear
 * Parametros: int ancho, int alto
 * Retorno: Tablero*
 * Descripcion: reserva memoria para el struct Tablero y su matriz void*** (alto//H x ancho//W). cada posicion contiene un (void*) apuntando a Celda* nueva.
 */
Tablero *tablero_crear(int ancho, int alto) {
    Tablero *t = malloc(sizeof *t);
    t->W = ancho;
    t->H = alto;

    // void ***: filas -> columnas -> (void*)
    t->celdas = malloc(sizeof *t->celdas * t->H);
    for (int y = 0; y < t->H; ++y) {
        t->celdas[y] = malloc(sizeof *t->celdas[y] * t->W);
        for (int x = 0; x < t->W; ++x) {
            Celda *cel = nueva_celda();
            t->celdas[y][x] = cel; // Celda* -> void*
        }
    }
    return t;
}

/*
 * Nombre: tablero_cerrar
 * Parametros: Tablero *t
 * Retorno: void
 * Descripcion: libera todas las Celdas, la matriz void*** y el Tablero
 */
void tablero_cerrar(Tablero *t) {
    if (t->celdas) {
        for (int y = 0; y < t->H; ++y) {
            for (int x = 0; x < t->W; ++x) {
                Celda *cel = celda_at(t, x, y);
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
 * Parametros: Juego *juego, int x, int y
 * Retorno: char (caracter representativo de la celda)
 * Descripcion: devuelve el caracter visible para la celda (alien, vacio o impacto 'X') segun su contenido y daño pendiente
 */
static char render_celda(struct Juego *juego, int x, int y) {
    Tablero *t = juego->t;
    Celda *cel = celda_at(t, x, y);
    if (cel->alien == NULL) return ' ';
    switch (cel->alien->tipo) {
        case AL_DRONE: return 'D';
        case AL_SKATER: return 'S';
        default: return 'E';
    }
}

/*
 * Nombre: tablero_imprimir
 * Parametros: Juego *juego
 * Retorno: void
 * Descripcion: Renderiza el tablero completo, la linea de base del jugador en
 *              y=-1 y el HUD persistente (incluye prompt si el juego no finalizo).
 */
void tablero_imprimir(struct Juego *juego) {
    Tablero *t = juego->t;
    // Limpiar pantalla (Linux/WSL) y reimprimir el frame en la misma posición
    system("clear");
    for (int y = t->H - 1; y >= 0; --y) {
        for (int x = 0; x < t->W; ++x) {
            char c = render_celda(juego, x, y);
            printf("[%c]", c);
        }
        printf("\n");
    }

    // Linea de base (jugador en y = -1)
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
    // Prompt integrado, excepto cuando el juego termino
    if (!juego->fin) {
        printf("Accion [A/D/1/2/3/Q]: ");
        fflush(stdout);
    } else {
        fflush(stdout);
    }

    // limpiar indicadores de daño para el proximo render
    for (int y = 0; y < t->H; ++y) {
        for (int x = 0; x < t->W; ++x) {
            Celda *cel = celda_at(t, x, y);
            cel->dano_pend = 0;
        }
    }
}
