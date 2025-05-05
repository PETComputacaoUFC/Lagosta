############### PROJECT CONFIG ###############
CXX := g++

CPP_SRC_DIR := cpp/src
CPP_DEPS_DIR := cpp/deps
CPP_LIBS_DIR := cpp/lib
CPP_INCLUDE_DIR := cpp/include
BUILD_DIR := build
TARGET := build/Lagosta/lagosta

CXXFLAGS := -Wall -Wextra -Wno-missing-field-initializers \
			-I$(CPP_INCLUDE_DIR) -std=c++20
LDFLAGS := -L$(CPP_LIBS_DIR) -lraylib -lZXing -lGL -lm -lpthread -ldl -lrt \
	       -lX11 -lsane

SOURCES := $(shell find $(CPP_SRC_DIR) -name "*.cpp")
DEPS_SOURCES := $(shell find $(CPP_DEPS_DIR) -name "*.cpp")
# Replace directory prefix and suffix .cpp with .o and prefix with $(BUILD_DIR)/.
OBJECTS := $(patsubst $(CPP_SRC_DIR)/%.cpp, $(BUILD_DIR)/src/%.o, $(SOURCES))
DEPS_OBJECTS := $(patsubst $(CPP_DEPS_DIR)/%.cpp, $(BUILD_DIR)/deps/%.o, $(DEPS_SOURCES))

################ BUILD RULES #################
all: $(TARGET)

run: $(TARGET)
	@cd $(dir $(TARGET)) && ./$(notdir $(TARGET))

clean:
	rm -rf $(BUILD_DIR) $(TARGET)


$(TARGET): $(OBJECTS) $(DEPS_OBJECTS)
	@mkdir -p $(dir $(TARGET))
	@cp -r resources $(dir $(TARGET))/resources
	$(CXX) $(LDFLAGS) $^ -o $(TARGET)

# Build cpp source files
$(BUILD_DIR)/src/%.o: $(CPP_SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build cpp dependencies
$(BUILD_DIR)/deps/%.o: $(CPP_DEPS_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -w -c $< -o $@

# Make sure build directory exists etc.
$(BUILD_DIR):
	@mkdir -p $@

.PHONY: all clean run