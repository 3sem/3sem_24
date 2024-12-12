CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRC_DIR = source
COMMON_DIR = $(SRC_DIR)/common
CLIENT_DIR = $(SRC_DIR)/client
SERVER_DIR = $(SRC_DIR)/server

# Source files
CLIENT_SRC = $(CLIENT_DIR)/client.c $(CLIENT_DIR)/main.c
SERVER_SRC = $(SERVER_DIR)/server.c $(SERVER_DIR)/main.c $(CLIENT_DIR)/client.c $(COMMON_DIR)/file_utils.c
COMMON_SRC = $(COMMON_DIR)/file_utils.c $(COMMON_DIR)/ipc_utils.c

# Header files
COMMON_HEADERS = $(COMMON_DIR)/file_utils.h $(COMMON_DIR)/ipc_utils.h $(COMMON_DIR)/log_utils.h
CLIENT_HEADERS = $(CLIENT_DIR)/client.h
SERVER_HEADERS = $(SERVER_DIR)/server.h

# Output binaries
CLIENT_BIN = client
SERVER_BIN = server

# Object files
CLIENT_OBJ = $(CLIENT_SRC:.c=.o) $(COMMON_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o) $(COMMON_SRC:.c=.o)

# Targets
all: $(CLIENT_BIN) $(SERVER_BIN)

# Compile client binary
$(CLIENT_BIN): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile server binary
$(SERVER_BIN): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile object files and include headers in the compilation
%.o: %.c $(COMMON_HEADERS) $(CLIENT_HEADERS) $(SERVER_HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up object files and binaries
clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_BIN) $(SERVER_BIN)

.PHONY: all clean

