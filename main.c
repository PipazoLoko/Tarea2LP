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

// Inventario o reserva total de aliens
static int total_inventario(PoolAliens *p) { 
    return p->drone + p->skater + p->especial; 
}

//itera sobre las celdas del eje x para revisar si existe algun alien o no en la base; retorna 1 si lo hay, de caso contrario retorna 0 
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
    j->fin = 0;

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

// UI persistente (pantalla alterna)
static void ui_enter(void) {
    // buffer alterno + ocultar cursor + posicionar al inicio
    printf("\033[?1049h\033[H\033[J\033[?25l");
    fflush(stdout);
}

static void ui_leave(void) {
    // restaurar cursor y buffer principal
    printf("\033[?25h\033[?1049l\n");
    fflush(stdout);
}

static int leer_accion(void) {
    char buf[256];
    if (!fgets(buf, sizeof(buf), stdin)) return 'Q';
    // aceptar exactamente un carácter no-espacio. si hay más de uno, inválido
    int count = 0;
    char c = 0;
    for (int i = 0; buf[i]; ++i) {
        unsigned char ch = (unsigned char)buf[i];
        if (!isspace(ch)) {
            if (count == 0) c = (char)ch;
            count++;
        }
    }
    if (count != 1) return -1; // inválido si vacío o más de uno
    if (c >= 'a' && c <= 'z') c = (char)toupper(c);
    return c;
}

static bool intentar_mover(Juego *j, int dir) {
    int nx = j->jugador_x + dir;
    if (nx < 0 || nx >= j->t->W) {
        snprintf(j->msg, sizeof(j->msg), "No puedes salir del tablero");
        return false; // no consume turno
    }
    j->jugador_x = nx;
    snprintf(j->msg, sizeof(j->msg), "Movido a x=%d", j->jugador_x);
    j->turno++;
    return true;
}

void juego_correr(Juego *j) {
    ui_enter();
    int salir = 0;
    while (!salir) {
        tablero_imprimir(j); // render + HUD + prompt persistente
        if (total_inventario(&j->pool) == 0 && j->vivos == 0) {
            // Pantalla final persistente con mensaje "VICTORIA"
            j->fin = 1; snprintf(j->msg, sizeof(j->msg), "VICTORIA");
            tablero_imprimir(j);
            char tmp[8]; fgets(tmp, sizeof(tmp), stdin); // esperar Enter
            ui_leave();
            return;
        }
        if (hay_alien_en_base(j)) {
            j->fin = 1; snprintf(j->msg, sizeof(j->msg), "DERROTA");
            tablero_imprimir(j);
            char tmp[8]; fgets(tmp, sizeof(tmp), stdin);
            ui_leave();
            return;
        }

        int act = leer_accion();
        bool procedio = false; // HUD persistente: no borramos msg aquí
        if (act == 'Q') { 
            salir = 1; 
            continue; 
        }
        switch (act) {
            case 'A': procedio = intentar_mover(j, -1); break;
            case 'D': procedio = intentar_mover(j, +1); break;
            case '1': procedio = disparar_armas(j, 0); if (procedio) { resolver_danos(j); j->turno++; } break;
            case '2': procedio = disparar_armas(j, 1); if (procedio) { resolver_danos(j); j->turno++; } break;
            case '3': procedio = disparar_armas(j, 2); if (procedio) { resolver_danos(j); j->turno++; } break;
            default:
                snprintf(j->msg, sizeof(j->msg), "Entrada invalida");
                break; // no consume turno
        }

        // al cierre del turno válido: movimiento y spawn
        if (procedio) {
            if (j->turno % 2 == 0) {
                mover_aliens(j);
            }
            spawn_turno(j);
        }
    }
    ui_leave();
}

int main(void) {
    srand((unsigned)time(NULL));
    int dificultad = 0;
    printf("Seleccione dificultad [0=Facil, 1=Dificil]: ");
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
