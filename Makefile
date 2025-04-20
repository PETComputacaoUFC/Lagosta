CC = g++
SOURCES = ./src/main.cpp ./src/filters.cpp ./src/reader.cpp

# Detectar sistema e terminal
UNAME_S := $(shell uname)
SHELLTYPE := $(shell echo $$SHELL)

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
	IS_WINDOWS := 1
endif
ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
	IS_WINDOWS := 1
endif

ifeq ($(IS_WINDOWS),1)
	CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-variable \
	         -std=c++23 -lraylib -lgdi32 -lwinmm -Iinclude -Llibs -O2
	OUTPUT = ./bin/main.exe

	# Se for bash no Windows
	ifneq (,$(findstring /bin/bash,$(SHELLTYPE)))
		CREATE_DIR = mkdir -p bin
	else
		CREATE_DIR = if not exist bin mkdir bin
	endif
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

clean:
	@if exist $(OUTPUT) del $(OUTPUT)