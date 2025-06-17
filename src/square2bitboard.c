#include <string.h>
#include <ctype.h>
#include "square2bitboard.h"

// Converts a square name (e.g., "A5", "C2") into a Bitboard with a single bit set
// Returns 0 if the input is invalid
Bitboard square2bitboard(char* square) {
    if (strlen(square) != 2) return 0;

    char col = toupper(square[0]);
    char row = square[1];

    if (col < 'A' || col > 'E') return 0;
    if (row < '1' || row > '5') return 0;

    int column = col - 'A';
    int line = '5' - row;
    int index = line * 5 + column;

    return 0b1U << index;
}


