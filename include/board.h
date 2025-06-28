#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>


typedef uint32_t Bitboard;

typedef struct {
    Bitboard bobail;
    Bitboard player1;
    Bitboard player2;
} Board;

/*
typedef uint64_t Hash;
*/

void init_board(Board* board);
void print_board(Board board);
int get_lowest_bit_index(Bitboard x);
void play(Board* board, Bitboard from_bit, Bitboard to_bit);

/*
Hash board_to_uint64(const Board* board, int player, int turn);
*/

#endif