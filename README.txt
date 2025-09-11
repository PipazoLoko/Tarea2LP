Tarea 2 — Space Invaders (terminal)

Compilación
-----------
Requiere un compilador C11. Para compilar y ejecutar:

    make
    ./juego

Luego seleccione la dificultad (0 Fácil, 1 Difícil) y use:
- A / D: mover la nave en la base (y = 0).
- 1: Disparo normal (munición infinita).
- 2: Disparo perforador (7 disparos al inicio).
- 3: Disparo especial (>= 1 al inicio).
- Q: Salir.

HUD
----
Debajo del tablero se muestra: Turno, aliens restantes del inventario, aliens
vivos, municiones (1/2/3) y el último mensaje de evento (impactos, drops,
entradas inválidas).

Armas
-----
1) Normal: Láser recto que impacta al primer alien de la columna del jugador,
   daña 1. Munición infinita. Implementación en `armas.c:arma_normal`.

2) Perforador: Láser que atraviesa y daña con 1 a todos los aliens en la
   columna actual. Requiere munición; comienza con 7. `armas.c:arma_perforador`.

3) Especial (creativo): "Golpe con rebote". Impacta al primer alien de la
   columna actual con daño 2 y, si existe, aplica daño 1 en diagonal superior
   inmediata (columnas vecinas x-1 y x+1 a una fila por encima del impacto).
   Es un recurso limitado que inicia con 1. `armas.c:arma_especial`.

Tipos de Alien
--------------
- Drone (HP=2): desciende recto cada 2 turnos.
- Skater (HP=1): desciende cada 2 turnos y se desplaza en zig-zag con rebote en
  bordes (dx = ±1).
- Especial (HP=2): desciende cada 2 turnos y avanza 2 celdas horizontales por
  descenso (con rebote). Implementación en `entidades.c`.

Spawns e Inventario
-------------------
- Fácil: tablero 5x15, tope simultáneos 6. Inventario: 6 Drones, 4 Skaters,
  5 Especiales (total 15). Sembrado inicial: 2 aliens.
- Difícil: tablero 7x15, tope simultáneos 8. Inventario: 8 Drones, 6 Skaters,
  6 Especiales (total 20). Sembrado inicial: 3 aliens.
- Por turno: si hay inventario y no se alcanza el tope, aparece 1 alien; con un
  30% se intenta un segundo. Si la fila superior está llena, se pospone.

Munición extra (drops)
----------------------
Al destruir un alien existe una tirada aleatoria inmediata:
- 25%: +1 munición Perforador.
- 15%: +1 munición Especial.

Supuestos y notas
-----------------
- Un alien por celda: si el destino del movimiento está ocupado, el alien
  permanece en su celda (no hay colisiones entre ellos).
- Orden del turno: acción válida -> (si disparo) resolución de daño -> mover
  aliens cada 2 turnos -> spawns del turno -> render.
- Derrota si algún alien alcanza y=0 (la base). Victoria si el inventario se
  agota y no quedan aliens vivos.

