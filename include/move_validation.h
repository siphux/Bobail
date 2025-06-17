#ifndef MOVE_VALIDATION_H
#define MOVE_VALIDATION_H

#include "board.h"
#include <stdbool.h>

bool is_legal_move(Board board, Bitboard from_bit, Bitboard to_bit, int player, int turn);

#endif