#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "entidades.h"
#include "tablero.h"
#include "main.h"

/*
 * Nombre: mover_un_alien
 * Parametros: Juego *juego, Alien *a
 * Retorno: void
 * Descripcion: Calcula y aplica el movimiento de un alien segun su tipo:
 *              baja una celda y puede desplazarse horizontalmente con rebote.
 */
static void mover_un_alien(struct Juego *juego, Alien *a) {
    Tablero *t = juego->t;
    int nx = a->x;
    int ny = a->y;

    // Todos descienden 1 celda (cada 2 turnos, controlado por main)
    ny -= 1; // y: H-1 arriba -> 0 base

    if (a->tipo == AL_SKATER) {
        // Zig-zag: desplazamiento horizontal y rebote en bordes
        nx += a->dx;
        if (nx < 0) { nx = 1; a->dx = +1; }
        else if (nx >= t->W) { nx = t->W - 2; a->dx = -1; }
    } else if (a->tipo == AL_ESPECIAL) {
        // Especial: se desplaza 2 celdas horizontales por descenso (rebota)
        nx += 2 * a->dx;
        if (nx < 0) { nx = 1; a->dx = +1; }
        else if (nx >= t->W) { nx = t->W - 2; a->dx = -1; }
    }

    // Respeta restriccion: maximo un alien por celda
    if (ny < 0) ny = 0; // clamp por seguridad
    Celda *dest = celda_at(t, nx, ny);
    if (dest->alien == NULL) {
        // Liberar celda origen
        Celda *orig = celda_at(t, a->x, a->y);
        orig->alien = NULL;
        // Mover
        dest->alien = a;
        a->x = nx; a->y = ny;
    } else {
        // Si esta ocupada, se queda en su lugar original (sin colisiones)
    }
}

/*
 * Nombre: mover_aliens
 * Parametros: Juego *juego
 * Retorno: void
 * Descripcion: Recorre el tablero y mueve todos los aliens vivos una vez.
 */
void mover_aliens(struct Juego *juego) {
    Tablero *t = juego->t;
    // Recorremos de abajo hacia arriba para no mover dos veces el mismo
    for (int y = 1; y < t->H; ++y) {
        for (int x = 0; x < t->W; ++x) {
            Celda *cel = celda_at(t, x, y);
            Alien *a = cel->alien;
            if (!a) continue;
            mover_un_alien(juego, a);
        }
    }
}

/*
 * Nombre: drop_municion
 * Parametros: Juego *juego
 * Retorno: void
 * Descripcion: Ejecuta tirada de drop al eliminar un alien y otorga municion
 *              extra al jugador segun probabilidades.
 */
static void drop_municion(struct Juego *juego) {
    // 25% perforador, 15% especial (ajustable)
    int r = rand() % 100;
    if (r < 25) {
        juego->armas.ammo_perforador += 1;
        snprintf(juego->mensaje_hud, sizeof(juego->mensaje_hud), "+1 municion Perforador");
    } else if (r < 40) { // 15% siguiente tramo
        juego->armas.ammo_especial += 1;
        snprintf(juego->mensaje_hud, sizeof(juego->mensaje_hud), "+1 municion Especial");
    }
}

/*
 * Nombre: resolver_danos
 * Parametros: Juego *juego
 * Retorno: void
 * Descripcion: Consolida el daño marcado en cada celda: reduce HP, elimina
 *              aliens muertos, decrementa 'vivos' y evalua drops.
 */
void resolver_danos(struct Juego *juego) {
    Tablero *t = juego->t;
    for (int y = 0; y < t->H; ++y) {
        for (int x = 0; x < t->W; ++x) {
            Celda *cel = celda_at(t, x, y);
            if (cel->dano_pend <= 0 || cel->alien == NULL) continue;
            Alien *a = cel->alien;
            a->hp -= cel->dano_pend;
            if (a->hp <= 0) {
                free(a);
                cel->alien = NULL;
                juego->vivos--;
                drop_municion(juego);
            }
            cel->dano_pend = 0;
        }
    }
}
