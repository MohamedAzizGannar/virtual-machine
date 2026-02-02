CC = clang
CFLAGS = -Wall -Wextra -I./include 
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

SOURCES = $(wildcard $(SRC_DIR)/*.c)

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET = simulateur

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: all clean rebuild run


