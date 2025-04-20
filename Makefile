CC = g++
SOURCES = ./src/main.cpp ./src/filters.cpp ./src/reader.cpp ./src/align.cpp
OBJECTS = $(patsubst ./src/%.cpp, ./bin/%.o, $(SOURCES))

ifeq ($(OS),Windows_NT)
	CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-variable \
			 -std=c++23 -Iinclude -O2
	LDFLAGS = -lraylib -lgdi32 -lwinmm -Llibs
	OUTPUT = ./bin/main.exe
	CREATE_DIR = if not exist bin mkdir bin
else
	CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-variable \
			-std=c++23 -Iinclude -O2
	LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Llibs
	OUTPUT = ./bin/main
	CREATE_DIR = mkdir -p bin
endif

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS) | dir
	$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT)

./bin/%.o: ./src/%.cpp | dir
	$(CC) $(CFLAGS) -c $< -o $@

dir:
	@$(CREATE_DIR)

run: all
	@./$(OUTPUT)

clean:
	rm -f $(OBJECTS) $(OUTPUT)

