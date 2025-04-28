############### PROJECT CONFIG ###############
CXX := g++
CXXFLAGS += -Wall -Wextra -Wno-missing-field-initializers \
		 	-Iinclude -std=c++20
ifeq ($(DEBUG),TRUE)
	CXXFLAGS += -g
else
	CXXFLAGS += -O2
endif

LDFLAGS := -Llib -lraylib -lZXing
OS_LDFLAGS := -lGL -lm -lpthread -ldl -lrt -lX11 -lsane

SOURCES := ./src/main.cpp ./src/reader.cpp ./src/imgtools/filters.cpp \
		   ./src/imgtools/imgtools.cpp ./src/scanner.cpp ./src/gui/gui.cpp \
		   ./src/gui/draw.cpp ./src/gui/widgets.cpp
DEPS_SOURCES := ./deps/imgui/imgui.cpp ./deps/imgui/imgui_draw.cpp \
		   ./deps/imgui/imgui_tables.cpp ./deps/imgui/imgui_widgets.cpp \
		   ./deps/imgui/imgui_demo.cpp ./deps/rlImGui.cpp ./deps/ImGuiFileDialog/ImGuiFileDialog.cpp

OBJECTS := $(patsubst ./src/%.cpp, ./build/src/%.o, $(SOURCES))
DEPS_OBJECTS := $(patsubst ./deps/%.cpp, ./build/deps/%.o, $(DEPS_SOURCES))

OUTPUT := ./build/lagosta/lagosta
BUILD_DIRS := $(sort $(dir $(OBJECTS))) $(sort $(dir $(DEPS_OBJECTS))) $(dir $(OUTPUT))

CREATE_DIR = mkdir -p $(BUILD_DIRS)
CLEAN = rm -rf $(OBJECTS) $(OUTPUT) ./build/lagosta

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
	cp -r ./resources ./build/lagosta

dir:
	@$(CREATE_DIR)

run: all
	@cd $(dir $(OUTPUT)) && ./$(notdir $(OUTPUT))

clean:
	@$(CLEAN)

$(OUTPUT): dir | $(OBJECTS) $(DEPS_OBJECTS)
	$(CXX) $(OBJECTS) $(DEPS_OBJECTS) $(CXXFLAGS) $(LDFLAGS) -o $(OUTPUT)

./build/src/%.o: ./src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

./build/deps/%.o: ./deps/%.cpp
	$(CXX) $(CXXFLAGS) -w -c $< -o $@
