// spawn.h — Inventario único por partida y aparición
#ifndef SPAWN_H
#define SPAWN_H

struct Juego; // forward

typedef struct {
    int especial;
    int skater;
    int drone;
    int vivos_tope; // tope de aliens simultáneos en pantalla
} PoolAliens;

void spawn_inicio(struct Juego *juego);
void spawn_turno(struct Juego *juego);

#endif // SPAWN_H

