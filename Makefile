all:
	mkdir -p bin
	g++ ./src/main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o ./bin/main

run: all
	./bin/main
