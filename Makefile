CC=gcc
CFLAGS=-Wall -O2 -std=c99
LDFLAGS=-L/usr/local/lib

build:
	gcc ./src/main.c -o ./bin/game `pkg-config --libs --cflags raylib`

build_linux:
	$(CC) ./src/main.c -o ./bin/game $(CFLAGS) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

run:
	./bin/game
