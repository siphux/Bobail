#include <stdio.h>
#include "board.h"
#include "masks.h"


void init_board(Board* board) {
    board->bobail = 0b1U << 12;
    board->player1 = MASK_DOWN;
    board->player2 = MASK_UP;
}

// ANSI color codes for terminal output
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"


void print_board(Board board) {
    printf("  A B C D E\n");
    for (int row = 0; row < 5; row++) {
        printf("%d ", 5 - row);
        for (int col = 0; col < 5; col++) {
            Bitboard mask = 0b1U << (5*row+col);
            if (board.bobail & mask) {
                printf(YELLOW "o " RESET);
            } else if (board.player1 & mask) {
                printf(RED "o " RESET);
            } else if (board.player2 & mask) {
                printf(GREEN "o " RESET);
            } else {
                printf(". ");
            }
        }
        printf("%d\n", 5 - row);
    }
    printf("  A B C D E\n");
}


int get_lowest_bit_index(Bitboard x) {
    if (x == 0) return -1;
    return __builtin_ctz(x);
}

void play(Board* board, Bitboard from_bit, Bitboard to_bit) {
    // Vérifier si la pièce à déplacer est le Bobail
    if (board->bobail & from_bit) {
        // Déplacer le Bobail:
        // 1. Enlever le Bobail de sa position d'origine (board->bobail &= ~from_bit;)
        // 2. Placer le Bobail sur sa nouvelle position (board->bobail |= to_bit;)
        // Ceci peut être combiné avec XOR :
        board->bobail ^= (from_bit | to_bit);
    }
    // Vérifier si la pièce à déplacer appartient au joueur 1
    else if (board->player1 & from_bit) {
        // Déplacer le pion du joueur 1
        board->player1 ^= (from_bit | to_bit);
    }
    // Vérifier si la pièce à déplacer appartient au joueur 2
    else if (board->player2 & from_bit) {
        // Déplacer le pion du joueur 2
        board->player2 ^= (from_bit | to_bit);
    }
    // Optionnel: Gérer le cas où il n'y a aucune pièce sur from_bit.
    // Pour l'instant, on fait confiance à l'appelant.
    // else {
    //     fprintf(stderr, "Erreur dans play(): Aucune pièce trouvée sur from_bit 0x%X\n", from_bit);
    // }
}

/*
// Donne la position (0...24) du seul bit à 1 dans bobail
int bitboard_log2(Bitboard b) {
    int pos = 0;
    while (b >>= 1) ++pos;
    return pos;
}

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