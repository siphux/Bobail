# Compiler and flags
CC = gcc
CFLAGS = -g -O0 -Wall -Werror -Wfatal-errors -Iinclude

# Linker flags (for math library)
LDFLAGS = -lm

# Source and object files
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,%.o,$(SRC))

# Executable name
TARGET = bobail

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation: place .o files in root, compile from src/
%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)