# Compiler
CXX = g++
CXXFLAGS = -Wall \
            -Wextra \
            -Wconversion \
            -Wsign-conversion \
            -Wpedantic \
            -Wshadow \
            -Wnull-dereference \
            -Wold-style-cast \
            -std=c++17 \
            -Isrc \
            -g \
            -fsanitize=address \
            -fsanitize=undefined

LDFLAGS = -fsanitize=address -fsanitize=undefined

# Generate dependency files
DEPFLAGS = -MMD -MP

# Folders
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Output binary
TARGET = $(BIN_DIR)/nexa

# Source files
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")

# Object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Dependency files
DEPS := $(OBJS:.o=.d)

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(TARGET)

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

# Include generated dependencies
-include $(DEPS)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Rebuild
re: clean all

.PHONY: all clean re
