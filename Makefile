CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

OBJS = main.o tablero.o entidades.o armas.o spawn.o

all: juego

juego: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

main.o: main.c main.h tablero.h entidades.h armas.h spawn.h
tablero.o: tablero.c tablero.h entidades.h main.h
entidades.o: entidades.c entidades.h tablero.h main.h
armas.o: armas.c armas.h entidades.h tablero.h main.h
spawn.o: spawn.c spawn.h entidades.h tablero.h main.h

clean:
	rm -f $(OBJS) juego

.PHONY: all clean

