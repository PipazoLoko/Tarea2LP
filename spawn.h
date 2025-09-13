// spawn.h — Inventario unico por partida y aparicion
#ifndef SPAWN_H
#define SPAWN_H

struct Juego; // forward

typedef struct {
    int especial;
    int skater;
    int drone;
    int vivos_tope; // tope de aliens simultaneos en pantalla
} PoolAliens;

void spawn_inicio(struct Juego *juego);
void spawn_turno(struct Juego *juego);

#endif // SPAWN_H

