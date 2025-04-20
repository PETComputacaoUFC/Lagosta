CC = g++
SOURCES = ./src/main.cpp ./src/filters.cpp ./src/reader.cpp
OBJECTS = $(patsubst ./src/%.cpp, ./bin/%.o, $(SOURCES))

# Detectar sistema e terminal
ifneq ($(OS),Windows_NT) # previne erro chato rodando make no cmd ou powershell
	UNAME_S := $(shell uname)
endif
SHELLTYPE := $(shell echo $$SHELL)

# Detecta se o sistema é windows pelo mingw, msys, ou flag do OS
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
	IS_WINDOWS := 1
endif
ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
	IS_WINDOWS := 1
endif
ifeq ($(OS),Windows_NT)
	IS_WINDOWS := 1
endif

ifeq ($(IS_WINDOWS),1) # Flags de compilamento no windows
	CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-variable \
	         -Iinclude -std=c++23 -O2
	LDFLAGS = -lraylib -lgdi32 -lwinmm -Iinclude -Llibs
	OUTPUT = ./bin/main.exe

	# Se for bash no Windows
	ifneq (,$(findstring /bin/bash,$(SHELLTYPE)))
		CREATE_DIR = mkdir -p bin
		CLEAN = rm -rf ./bin/*
	else
		CREATE_DIR = if not exist bin mkdir bin
		CLEAN = del /f /q .\\bin\\*
	endif
else # Flags de compilamento no linux
	CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-variable \
	         -Iinclude -std=c++23 -O2
	LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Llibs
	OUTPUT = ./bin/main
	CREATE_DIR = mkdir -p bin
	CLEAN = rm -rf ./bin/*
endif

all: $(OUTPUT)

$(OUTPUT): dir | $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT)

./bin/%.o: ./src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

dir:
	@$(CREATE_DIR)

run: all
	@./$(OUTPUT)

clean:
	@$(CLEAN)

.PHONY = all dir run clean