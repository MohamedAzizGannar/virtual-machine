CC = clang
CFLAGS = -Wall -Wextra -I./include 
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

# Automatically find all .c files in src/
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Automatically generate object file names
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Target executable name
TARGET = $(BIN_DIR)/asm-parser

# Default target
all: $(TARGET)

# Link all object files
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

# Compile each .c file to .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete"

# Rebuild from scratch
rebuild: clean all

# Run the program (optional)
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run


