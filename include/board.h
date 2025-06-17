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
// Donne la position (0...24) du seul bit à 1 dans bobail
int bitboard_log2(Bitboard b) {
    int pos = 0;
    while (b >>= 1) ++pos;
    return pos;
}

typedef uint64_t Hash;

Hash board_to_uint64(const Board* board, int player, int turn) {
    Hash hash = 0;
    int bobail_pos = bitboard_log2(board->bobail) & 0x1F;         // 5 bits
    hash = bobail_pos;                                            // 5 bits pour la position du bobail
    hash = (hash << 25) | (board->player1 & 0x1FFFFFF);           // 25 bits pour player1
    hash = (hash << 25) | (board->player2 & 0x1FFFFFF);           // 25 bits pour player2
    hash = (hash << 1)  | (player & 1);                           // 1 bit: joueur à jouer (1 bit seulement !)
    hash = (hash << 1)  | (turn & 1);                             // 1 bit: pièce à jouer
    return hash;
}

*/


void init_board(Board* board);
void print_board(Board board);
int get_lowest_bit_index(Bitboard x);
void play(Board* board, Bitboard from_bit, Bitboard to_bit);


#endif