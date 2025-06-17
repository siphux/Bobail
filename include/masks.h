#ifndef MASKS_H
#define MASKS_H

#include <stdint.h>
#include "board.h"

#define MASK_LEFT  ((Bitboard)0b0000100001000010000100001U)
#define MASK_RIGHT ((Bitboard)0b1000010000100001000010000U)
#define MASK_UP    ((Bitboard)0b0000000000000000000011111U)
#define MASK_DOWN  ((Bitboard)0b1111100000000000000000000U)

#endif
