all:
	mkdir -p bin
	gcc ./src/main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o ./bin/main

run: all
	./bin/main
