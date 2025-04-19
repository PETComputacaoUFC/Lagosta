CC = g++
SOURCES = ./src/main.cpp ./src/filters.cpp ./src/reader.cpp

ifeq ($(OS),Windows_NT)
	CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-variable \
			 -std=c++23 -lraylib -lgdi32 -lwinmm -Iinclude -Llibs -O2
	OUTPUT = ./bin/main.exe
	CREATE_DIR = if not exist bin mkdir bin
else
	CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-variable \
			-std=c++23 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Iinclude -Llibs -O2
	OUTPUT = ./bin/main
	CREATE_DIR = mkdir -p bin
endif

all: $(OUTPUT)

$(OUTPUT): $(SOURCES) | dir
	$(CC) $(SOURCES) $(CFLAGS) -o $(OUTPUT)

dir:
	@$(CREATE_DIR)

run: all
	@./$(OUTPUT)