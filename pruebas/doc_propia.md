fordward-declaration: declaracion anticipada que introduce el nombre y tipo en un archivo sin definirla. Permite que partes del codigo ocupen ese nombre sin que antes el compilador haya llegado a su definicion. 

stdbool.h: permite operar con booleanos y retornar booleanos en funciones

stddef.h: size_t, ptrdiff_t, wchar_t, NULL(puntero nulo), offsetof(struct_type, member)

en caso de que no se utilice una libreria dentro de un archivo.h, es por dependencia (ya sea, por que lo utilizan otros archivos incluidos(#include "... .h") o se utiliza en el archivo.c)

el static hace que esas funciones queden remitidas y funcionales al mismo archivo, no pudiendo ser invocadas por otros archivos

