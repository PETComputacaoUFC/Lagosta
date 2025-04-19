CC = g++
CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-variable \
		 -std=c++23 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O2
OUTPUT = ./bin/main
SOURCES = ./src/main.cpp ./src/filters.cpp ./src/reader.cpp

all: $(OUTPUT)

$(OUTPUT): $(SOURCES) | dir
	$(CC) $(SOURCES) $(CFLAGS) -o $(OUTPUT)

dir:
	@mkdir -p bin

run: all
	@./$(OUTPUT)
