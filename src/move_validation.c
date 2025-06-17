#include "move_validation.h"
#include "masks.h"
#include "directions_masks.h"
#include <stdlib.h>

bool is_legal_move(Board board, Bitboard from_bit, Bitboard to_bit, int player, int turn) {
    
    if (from_bit == 0 || to_bit == 0 || (from_bit & to_bit)) return false;

    if ((turn == 0 && !(board.bobail & from_bit)) ||
        ((turn == 1) && ((player == 0 && !(board.player1 & from_bit)) || (player == 1 && !(board.player2 & from_bit))))) return false;

    Bitboard occupied = board.player1 | board.player2 | board.bobail;
    if (to_bit & occupied) return false;

    if (turn == 0) {
        Bitboard adjacent = 0b0U;
        if (board.bobail & MASK_LEFT){
            adjacent |= (board.bobail >> 5);
            adjacent |= (board.bobail >> 4);
            adjacent |= (board.bobail << 1);
            adjacent |= (board.bobail << 5);
            adjacent |= (board.bobail << 6);
        } else if (board.bobail & MASK_RIGHT){
            adjacent |= (board.bobail >> 6);
            adjacent |= (board.bobail >> 5);
            adjacent |= (board.bobail >> 1);
            adjacent |= (board.bobail << 4);
            adjacent |= (board.bobail << 5);
        } else{
            adjacent |= (board.bobail >> 6);
            adjacent |= (board.bobail >> 5);
            adjacent |= (board.bobail >> 4);
            adjacent |= (board.bobail >> 1);
            adjacent |= (board.bobail << 1);
            adjacent |= (board.bobail << 4);
            adjacent |= (board.bobail << 5);
            adjacent |= (board.bobail << 6);
        }
        if (adjacent & to_bit) return true;
        else return false;
    }

    for (int i = 0; i < 8; i++) {
        Bitboard current = from_bit;
        int shift = directions_masks[i].shift;
        Bitboard mask = directions_masks[i].mask;
        while (true) {
            if (current & mask) break;

            if (shift > 0) {
                current <<= shift;
            } else {
                current >>= -shift;
            }
            if (current & occupied) break;

            if (current & to_bit) {
                Bitboard next = current;
                if (next & mask) return true;

                if (shift > 0) {
                    next <<= shift;
                } else {
                    next >>= -shift;
                }

                if (next & occupied) return true;
                else return false;
            }
        }
    }
    return false;
}