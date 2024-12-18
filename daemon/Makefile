# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = -lpthread

# Directories
SRC_DIR = source
INCLUDE_DIR = include
BUILD_DIR = build

# Source and Object Files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Target Executable
TARGET = daemon

# Default Rule
all: $(TARGET)
# Linking
$(TARGET): $(OBJS)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Create Build Directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Clean Rule
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR) $(TARGET)

# Phony Targets
.PHONY: all clean

