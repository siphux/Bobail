#ifndef MCTS_H
#define MCTS_H

#include <stdbool.h>
#include "board.h"

typedef struct Move {
    Bitboard from_bit;
    Bitboard to_bit;
} Move;

typedef struct Node {
    struct Node* parent;
    struct Node** children;
    int num_children;

    Board* board;
    Move move;
    int player;
    int turn; // 0 = Bobail à jouer, 1 = pion à jouer

    int visits;
    int wins; // wins of his parent when passing by this node

    bool is_terminal;
} Node;

Node* create_node(Board* board, int player, int turn, Move move, Node* parent);
void free_tree(Node* node);
Move mcts(Node** p_root, double exploration, int iterations);

#endif
