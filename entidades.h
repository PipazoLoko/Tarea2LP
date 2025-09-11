// entidades.h — Aliens, movimiento y resolución de daños
#ifndef ENTIDADES_H
#define ENTIDADES_H

#include <stdbool.h>

struct Juego; // forward-declaration para punteros en firmas

// Tipos de alien
enum { AL_DRONE = 0, AL_SKATER = 1, AL_ESPECIAL = 2 };

typedef struct {
    int tipo;  // tipo de alien
    int hp;    // vida actual
    int x, y;  // posición en el tablero
    int dx;    // dirección horizontal (zig-zag), p.ej. -1/+1
} Alien;

typedef struct Celda {
    Alien *alien; // NULL si la celda está vacía
    int dano_pend; // daño marcado a consolidar
} Celda;

void mover_aliens(struct Juego *juego);
void resolver_danos(struct Juego *juego);

#endif // ENTIDADES_H
