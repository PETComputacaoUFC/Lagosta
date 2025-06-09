############### PROJECT CONFIG ###############
CXX := g++

CPP_SRC_DIR := cpp/src
# CPP_LIB_DIR := cpp/lib
CPP_INCLUDE_DIR := cpp/include
BUILD_DIR := build
TARGET := build/leitor.so

CXXFLAGS := -Wall -Wextra -Wno-missing-field-initializers \
			-I$(CPP_INCLUDE_DIR) -std=c++20 -fPIC
LDFLAGS := -lraylib -lZXing -lGL -lm -lpthread -ldl -lX11

SOURCES := $(shell find $(CPP_SRC_DIR) -name "*.cpp")
# Replace directory prefix and suffix .cpp with .o and prefix with $(BUILD_DIR)/.
OBJECTS := $(patsubst $(CPP_SRC_DIR)/%.cpp, $(BUILD_DIR)/src/%.o, $(SOURCES))

################ BUILD RULES #################
all: $(TARGET)

run: $(TARGET)
	@cd $(dir $(TARGET)) && ./$(notdir $(TARGET))

clean:
	rm -rf $(BUILD_DIR) $(TARGET)


$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $(TARGET))
	@cp -r resources $(dir $(TARGET))/resources
	$(CXX) -shared $(LDFLAGS) $^ -o $(TARGET)

# Build cpp source files
$(BUILD_DIR)/src/%.o: $(CPP_SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Make sure build directory exists etc.
$(BUILD_DIR):
	@mkdir -p $@

.PHONY: all clean run