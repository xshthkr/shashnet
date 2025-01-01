# Paths
BIN_DIR := bin
SRC_DIR := src
INCLUDE_DIR := include
TEST_DIR := test

# Compiler and flags
CC := gcc
CFLAGS := -g -Wall -I$(INCLUDE_DIR)

# Sources and objects
CLIENT_SRC := $(SRC_DIR)/client.c
SERVER_SRC := $(SRC_DIR)/server.c
PROTOCOL_SRC := $(SRC_DIR)/protocol.c
UTILS_SRC := $(SRC_DIR)/utils.c
TRANSPORT_SRC := $(SRC_DIR)/transport.c
TEST_CLIENT_SRC := $(TEST_DIR)/client.c
TEST_SERVER_SRC := $(TEST_DIR)/server.c

CLIENT_OBJ := $(BIN_DIR)/client.o
SERVER_OBJ := $(BIN_DIR)/server.o
PROTOCOL_OBJ := $(BIN_DIR)/protocol.o
UTILS_OBJ := $(BIN_DIR)/utils.o
TRANSPORT_OBJ := $(BIN_DIR)/transport.o

# Targets
CLIENT_BIN := $(BIN_DIR)/client
SERVER_BIN := $(BIN_DIR)/server
TEST_CLIENT_BIN := $(BIN_DIR)/test_client
TEST_SERVER_BIN := $(BIN_DIR)/test_server

# Default target
all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_OBJ) $(PROTOCOL_OBJ) $(UTILS_OBJ) $(TRANSPORT_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(SERVER_BIN): $(SERVER_OBJ) $(PROTOCOL_OBJ) $(UTILS_OBJ) $(TRANSPORT_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_OBJ): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_OBJ): $(SERVER_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(PROTOCOL_OBJ): $(PROTOCOL_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(UTILS_OBJ): $(UTILS_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(TRANSPORT_OBJ): $(TRANSPORT_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_CLIENT_BIN): $(TEST_CLIENT_SRC)
	$(CC) $(CFLAGS) $< -o $@

$(TEST_SERVER_BIN): $(TEST_SERVER_SRC)
	$(CC) $(CFLAGS) $< -o $@

test: $(TEST_CLIENT_BIN) $(TEST_SERVER_BIN)

clean:
	rm -rf $(BIN_DIR)/*.o $(CLIENT_BIN) $(SERVER_BIN) $(TEST_CLIENT_BIN) $(TEST_SERVER_BIN)

.PHONY: all test clean
