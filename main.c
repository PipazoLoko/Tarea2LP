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

/*
 * Nombre: total_inventario
 * Parametros: PoolAliens *p / reserva de enemigos
 * Retorno: int / cantidad total de aliens restantes
 * Descripcion: devuelve la suma del stock restante de aliens (drone+skater+especial), se usa en HUD y en condiciones de fin
 */
static int total_inventario(PoolAliens *p) {
    int stock_restante = p->drone + p->skater + p->especial;
    return stock_restante;
}

/*
 * Nombre: hay_alien_en_base
 * Parametros: Juego *j
 * Retorno: int / 1 si hay alien en y=0, de lo contrario 0 
 * Descripcion: recorre la fila-base (y=0) para verificar si algun alien alcanzo la base, la derota lo ve otra funcion
 */
static int hay_alien_en_base(Juego *j) {
    for (int x = 0; x < j->t->W; ++x) {
        Celda *c = celda_at(j->t, x, 0);
        if (c->alien) return 1;
    }
    return 0;
}

/*
 * Nombre: juego_iniciar
 * Parametros: Juego *j, int dificultad / 0 facil, 1 dificil
 * Retorno: bool / true si inicia correctamente, false de lo contrario
 * Descripcion: crea el tablero segun dificultad, configura armas y sus punteros a funcion, inicializa contadores y ejecuta el sembrado inicial de aliens
 */
bool juego_iniciar(Juego *j, int dificultad) {
    *j = (Juego){0}; // inicializa en 0 todos los bytes de juego => punteros en NULL, int en 0...
    j->dificultad = dificultad;
    j->turno = 0;
    j->vivos = 0;
    j->mensaje_hud[0] = '\0';

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

/*
 * Nombre: juego_cerrar
 * Parametros: Juego *j
 * Retorno: void
 * Descripcion: libera todos los recursos del juego, elimina aliens restantes, y libera la memoria asociada al tablero (void***).
 */
void juego_cerrar(Juego *j) {
    // liberar aliens restantes
    for (int y = 0; y < j->t->H; ++y) {
        for (int x = 0; x < j->t->W; ++x) {
            Celda *c = celda_at(j->t, x, y);
            if (c->alien) free(c->alien);
            c->alien = NULL;
        }
    }
    tablero_cerrar(j->t);
}

/*
 * Nombre: ui_enter
 * Parametros: (ninguno)
 * Retorno: void
 * Descripcion: Limpia la consola usando el comando 'clear' para la persistentencia del frame
 */
static void ui_enter(void) {
    system("clear");
}

/*
 * Nombre: ui_leave
 * Parametros: (ninguno)
 * Retorno: void
 * Descripcion: imprime  un salto de línea al terminar el juego 
 */
static void ui_leave(void) {
    putchar('\n');
}

/*
 * Nombre: leer_accion
 * Parametros: ninguno
 * Retorno: int // caracter de accion en mayuscula; -1 si invalida; 'Q' para salir)
 * Descripcion: lee una linea de la entrada estandar y la valida. acepta exactamente un caracter no-espacio, cualquier otra entrada se considera invalida.
 */
static int leer_accion(void) {
    char buf[256];
    if (!fgets(buf, sizeof(buf), stdin)) return 'Q';
    // aceptar exactamente un caracter no-espacio. si hay mas de uno, invalido
    int count = 0;
    char c = 0;
    for (int i = 0; buf[i]; ++i) {
        unsigned char ch = (unsigned char)buf[i];
        if (!isspace(ch)) {
            if (count == 0) c = (char)ch;
            count++;
        }
    }
    if (count != 1) return -1; // invalido si vacio o mas de uno
    if (c >= 'a' && c <= 'z') c = (char)toupper(c);
    return c;
}

/*
 * Nombre: intentar_mover
 * Parametros: Juego *j, int dir (-1 izquierda, +1 derecha)
 * Retorno: bool (true si el movimiento ocurrio; false si fue invalido)
 * Descripcion: intenta mover al jugador dentro de los limites, en caso valido, actualiza mensaje HUD y consume turno. si es invalido, informa y no consume turno
 */
static bool intentar_mover(Juego *j, int dir) {
    int x_destino = j->jugador_x + dir;
    if (x_destino < 0 || x_destino >= j->t->W) {
        snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "No puedes salir del tablero");
        return false; // no consume turno
    }
    j->jugador_x = x_destino;
    snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Movido a x=%d", j->jugador_x);
    j->turno++;
    return true;
}

/*
 * Nombre: juego_correr
 * Parametros: Juego *j (estado principal)
 * Retorno: void
 * Descripcion: Ejecuta el ciclo de turnos del juego con HUD persistente. Por
 *              cada accion valida, resuelve disparos, mueve aliens cada 2
 *              turnos, intenta spawns y re-renderiza. Gestiona condiciones de
 *              victoria/derrota mostrando el tablero final.
 */
void juego_correr(Juego *j) {
    ui_enter();
    int salir = 0;
    while (!salir) {
        tablero_imprimir(j); // render + HUD + prompt persistente
        if (total_inventario(&j->pool) == 0 && j->vivos == 0) {
            // Pantalla final persistente con mensaje "VICTORIA"
            j->fin = 1; snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "VICTORIA");
            tablero_imprimir(j);
            char tmp[8]; fgets(tmp, sizeof(tmp), stdin); // esperar Enter
            ui_leave();
            return;
        }
        if (hay_alien_en_base(j)) {
            j->fin = 1; snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "DERROTA");
            tablero_imprimir(j);
            char tmp[8]; fgets(tmp, sizeof(tmp), stdin);
            ui_leave();
            return;
        }

        int act = leer_accion();
        bool procedio = false; // HUD persistente: no borramos mensaje_hud aqui
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
                snprintf(j->mensaje_hud, sizeof(j->mensaje_hud), "Entrada invalida");
                break; // no consume turno
        }

        // al cierre del turno valido: movimiento y spawn
        if (procedio) {
            if (j->turno % 2 == 0) {
                mover_aliens(j);
            }
            spawn_turno(j);
        }
    }
    ui_leave();
}

/*
 * Nombre: main
 * Parametros: (ninguno)
 * Retorno: int (0 en exito, distinto de 0 en error)
 * Descripcion: Punto de entrada. Pide dificultad, inicializa el juego, corre el
 *              bucle principal y libera recursos al finalizar.
 */
int main(void) {
    srand((unsigned)time(NULL));
    int dificultad = 0;
    printf("Seleccione dificultad [0=Facil, 1=Dificil]: ");
    fflush(stdout);
    if (scanf("%d", &dificultad) != 1) dificultad = 0;
    int ch; 
    while ((ch = getchar()) != '\n' && ch != EOF) {}

    Juego j;
    if (!juego_iniciar(&j, dificultad)) {
        fprintf(stderr, "No se pudo iniciar el juego\n");
        return 1;
    }
    juego_correr(&j);
    juego_cerrar(&j);
    return 0;
}
