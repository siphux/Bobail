#ifndef DIRECTIONS_MASKS_H
#define DIRECTIONS_MASKS_H

#include "board.h"
#include "masks.h"

// Directions : décalages et masques associés
typedef struct {
    int shift;
    Bitboard mask;
} Direction;

extern Direction directions_masks[8];

#endif