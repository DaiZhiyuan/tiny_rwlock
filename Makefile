CC = gcc 
CFLAGS = -Iinclude -Wall
LDFLAGS = -lpthread

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(SRC_DIR)/main.c
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXEC = $(BIN_DIR)/rwlock_test

all: $(EXEC)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJECTS) $(BIN_DIR)
	@$(CC) $(OBJECTS) -o $(EXEC) $(LDFLAGS)

clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

check:
	@objdump -d bin/rwlock_test | grep -i 'ldxr\|ldaxr\|stxr\|stlxr'
	@if [ $$# -eq 0 ]; then \
		echo "program use load/store exclusives instruction"; \
	fi

.PHONY: all clean check

