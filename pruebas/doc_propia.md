fordward-declaration: declaracion anticipada que introduce el nombre y tipo en un archivo sin definirla. Permite que partes del codigo ocupen ese nombre sin que antes el compilador haya llegado a su definicion. 

stdbool.h: permite operar con booleanos y retornar booleanos en funciones

stddef.h: size_t, ptrdiff_t, wchar_t, NULL(puntero nulo), offsetof(struct_type, member)

en caso de que no se utilice una libreria dentro de un archivo.h, se define igual, ya sea por dependencia (ya sea, por que lo utilizan otros archivos incluidos(#include "... .h") o se utiliza en el archivo.c)

el static hace que esas funciones queden remitidas y funcionales al mismo archivo, no pudiendo ser invocadas por otros 

Sobre la funcion hay_alien_en_base, sobre la linea:
Celda *c = (Celda *)j->t->celdas[0][x];
el () hace un casteo y el c es un puntero a un tipo celda, esto nos permite en la sgte linea acceder al campo aliend de cada celda, verificando si 

*j = (Juego){0}: pone en 0 todos los bytes de juego, implica que punteros quedan en NULL, enteros en 0 y asi con el resto de tipo de archivos...

Sobre los typedef struct en los .h
