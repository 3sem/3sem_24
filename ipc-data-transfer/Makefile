TARGET = main
CC = gcc

BUILD_DIR   = build
SOURCE_DIR  = source
INCLUDE_DIR = include
SCRIPTS_DIR = scripts

INCLUDE = -I$(INCLUDE_DIR)
SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS = $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%, $(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@$(CC) $(INCLUDE) $^ -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	@$(CC) $(INCLUDE) -c $^ -o $@

$(BUILD_DIR):
	@echo "Creating build folder"
	@mkdir -p $(BUILD_DIR)

clean:
	@echo "Deleting build folder"
	@rm -r $(BUILD_DIR) 
