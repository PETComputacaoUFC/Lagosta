############### PROJECT CONFIG ###############
CXX := g++
CXXFLAGS += -Wall -Wextra -Wno-missing-field-initializers \
		 	-Iinclude -std=c++20 -O2
LDFLAGS := -Llib -lraylib -lZXing
OS_LDFLAGS := -lGL -lm -lpthread -ldl -lrt -lX11 -lsane

SOURCES := ./src/main.cpp ./src/reader.cpp ./src/imgtools/filters.cpp \
		   ./src/imgtools/imgtools.cpp ./src/scanner.cpp
OBJECTS := $(patsubst ./src/%.cpp, ./build/%.o, $(SOURCES))
OUTPUT := ./build/lagosta
BUILD_DIRS := $(sort $(dir $(OBJECTS)))

CREATE_DIR = mkdir -p $(BUILD_DIRS)
CLEAN = rm -rf $(OBJECTS) $(OUTPUT)

############### WINDOWS FLAGS ###############
ifeq ($(OS),Windows_NT)
	IS_WINDOWS := 1
else
	UNAME_S := $(shell uname)
	ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
		IS_WINDOWS := 1
	else ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
		IS_WINDOWS := 1
	endif
endif

ifeq ($(IS_WINDOWS),1) # Flags de compilação no windows
	OS_CXXFLAGS := -DWINDOWS=true
	OS_LDFLAGS := -lgdi32 -lwinmm
	OUTPUT := ./build/lagosta.exe

	# Se NÃO for bash no Windows
	ifeq (,$(findstring /bin/bash,$(shell echo $$SHELL)))
		CREATE_DIR := @for %%d in ($(subst /,\, $(BUILD_DIRS))) do @if not exist "%%d" mkdir "%%d"
		CLEAN := @for %%d in ($(subst /,\, $(OBJECTS) $(OUTPUT))) do @if exist "%%d" del /F /Q "%%d"
	endif
endif

CXXFLAGS += $(OS_CXXFLAGS)
LDFLAGS += $(OS_LDFLAGS)
############### BUILDING RULES ###############
.PHONY = all dir run clean

all: $(OUTPUT)

dir:
	@$(CREATE_DIR)

run: all
	@./$(OUTPUT)

clean:
	@$(CLEAN)

$(OUTPUT): dir | $(OBJECTS)
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(LDFLAGS) -o $(OUTPUT)

./build/%.o: ./src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
