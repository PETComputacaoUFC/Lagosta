CC = g++
CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-variable \
		 -std=c++23 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUTPUT = ./bin/main
SOURCES = ./src/*.cpp

all: $(OUTPUT)

$(OUTPUT): $(SOURCES) | dir
	$(CC) $(SOURCES) $(CFLAGS) -o $(OUTPUT)

dir:
	@mkdir -p bin

run: all
	@./$(OUTPUT)
