# Compiler and flags
CC = gcc
CFLAGS = -g -O0 -Wall -Werror -Wfatal-errors

# Linker flags (for math library)
LDFLAGS = -lm

# Source and object files
SRC = main.c board.c move_validation.c victory.c square2bitboard.c mcts.c directions_masks.c
OBJ = $(SRC:.c=.o)

# Executable name
TARGET = bobail

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
