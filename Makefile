build:
	gcc ./src/main.c -o ./bin/game `pkg-config --libs --cflags raylib`

run:
	./bin/game
