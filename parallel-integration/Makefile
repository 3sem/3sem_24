CC = gcc

BUILD_DIR = build
SOURCE_DIR = source
SCRIPTS_DIR = scripts

SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS = $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%, $(SOURCES))

all: $(OBJECTS)

$(BUILD_DIR)/%: $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	@$(CC) $< -o $@

$(BUILD_DIR):
	@echo "Creating build folder"
	@mkdir -p $(BUILD_DIR)

run:
	@python3 $(SCRIPTS_DIR)/run.py

clean:
	@echo "Deleting build folder"
	@rm -r $(BUILD_DIR) 
