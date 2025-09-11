#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "main.h"
#include "tablero.h"
#include "entidades.h"
#include "armas.h"
#include "spawn.h"

static int total_inventario(PoolAliens *p) { return p->drone + p->skater + p->especial; }

static int hay_alien_en_base(Juego *j) {
    for (int x = 0; x < j->t->W; ++x) {
        Celda *c = (Celda *)j->t->celdas[0][x];
        if (c->alien) return 1;
    }
    return 0;
}

bool juego_iniciar(Juego *j, int dificultad) {
    memset(j, 0, sizeof(*j));
    j->dificultad = dificultad;
    j->turno = 0;
    j->vivos = 0;
    j->msg[0] = '\0';

    int W = (dificultad == 0) ? 5 : 7;
    int H = 15;
    j->t = tablero_crear(W, H);
    if (!j->t) return false;
    j->jugador_x = W / 2;

    // armas
    j->armas.fn[0] = arma_normal;
    j->armas.fn[1] = arma_perforador;
    j->armas.fn[2] = arma_especial;
    j->armas.ammo_perforador = 7;
    j->armas.ammo_especial = 1; // al menos 1 inicial

    // pool/spawn
    spawn_inicio(j);

    return true;
}

void juego_cerrar(Juego *j) {
    // liberar aliens restantes
    for (int y = 0; y < j->t->H; ++y) {
        for (int x = 0; x < j->t->W; ++x) {
            Celda *c = (Celda *)j->t->celdas[y][x];
            if (c->alien) free(c->alien);
            c->alien = NULL;
        }
    }
    tablero_cerrar(j->t);
}

static int leer_accion(void) {
    printf("Acción [A/D/1/2/3/Q]: ");
    fflush(stdout);
    char buf[64];
    if (!fgets(buf, sizeof(buf), stdin)) return 'Q';
    char c = 0;
    for (int i = 0; buf[i]; ++i) if (!isspace((unsigned char)buf[i])) { c = buf[i]; break; }
    if (c == 0) return -1;
    if (c >= 'a' && c <= 'z') c = (char)toupper(c);
    return c;
}

static void intentar_mover(Juego *j, int dir) {
    int nx = j->jugador_x + dir;
    if (nx < 0 || nx >= j->t->W) {
        snprintf(j->msg, sizeof(j->msg), "No puedes salir del tablero");
        return; // no consume turno
    }
    j->jugador_x = nx;
    j->turno++;
}

void juego_correr(Juego *j) {
    while (1) {
        tablero_imprimir(j);
        if (total_inventario(&j->pool) == 0 && j->vivos == 0) {
            printf("Victoria: inventario agotado y sin aliens vivos.\n");
            break;
        }
        if (hay_alien_en_base(j)) {
            printf("Derrota: un alien alcanzó la base.\n");
            break;
        }

        int act = leer_accion();
        bool procedio = false; // HUD persistente: no borramos msg aquí
        if (act == 'Q') break;
        switch (act) {
            case 'A': intentar_mover(j, -1); procedio = true; break;
            case 'D': intentar_mover(j, +1); procedio = true; break;
            case '1': procedio = disparar_armas(j, 0); if (procedio) { resolver_danos(j); j->turno++; } break;
            case '2': procedio = disparar_armas(j, 1); if (procedio) { resolver_danos(j); j->turno++; } break;
            case '3': procedio = disparar_armas(j, 2); if (procedio) { resolver_danos(j); j->turno++; } break;
            default:
                snprintf(j->msg, sizeof(j->msg), "Entrada inválida");
                break; // no consume turno
        }

        // Al cierre del turno válido: movimiento y spawn
        if (procedio) {
            if (j->turno % 2 == 0) {
                mover_aliens(j);
            }
            spawn_turno(j);
        }
    }
}

int main(void) {
    srand((unsigned)time(NULL));
    int dificultad = 0;
    printf("Seleccione dificultad [0=Fácil, 1=Difícil]: ");
    fflush(stdout);
    if (scanf("%d", &dificultad) != 1) dificultad = 0;
    int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

    Juego j;
    if (!juego_iniciar(&j, dificultad)) {
        fprintf(stderr, "No se pudo iniciar el juego\n");
        return 1;
    }
    juego_correr(&j);
    juego_cerrar(&j);
    return 0;
}
