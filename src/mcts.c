#include "mcts.h"
#include "move_validation.h"
#include "victory.h"
#include "masks.h"
#include "directions_masks.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// Recursively frees the MCTS tree from memory
void free_tree(Node* node) {
    for (int i = 0; i < node->num_children; i++) {
        free_tree(node->children[i]);
    }
    free(node->children);
    free(node->board);
    free(node);
}


bool is_terminal(Board board){
    if ((victory(board, 0)>=0)) return true;
    else return false;
}


/*  TREE TRAVERSAL  */


// Selects the best child node of a node using the UCB1 formula (Upper Confidence Bound)
Node* select_best_ucb(Node* node, double exploration) {
    Node* best = NULL;
    double best_score = -1.0;
    for (int i = 0; i < node->num_children; i++) {
        Node* child = node->children[i];
        if (child->visits==0){
            return child;
        }
        double win_rate = (double)child->wins / (child->visits);
        double ucb = win_rate + exploration * sqrt(log(node->visits) / (child->visits));
        if (ucb > best_score) {
            best = child;
            best_score = ucb;
        }
    }
    return best;
}

//Return the node to expand or rollout (works recursively)
Node* tree_traversal(Node* node, double exploration){
    if (node->num_children == 0) return node;
    else return tree_traversal(select_best_ucb(node, exploration), exploration);
}


/*  NODE EXPANSION  */


// Creates a new MCTS node with a copy of the board, current player, phase of turn, move played to reach it, and parent node
Node* create_node(Board* board, int player, int turn, Move move, Node* parent) {
    Node* node = malloc(sizeof(Node));

    node->board = malloc(sizeof(Board));
    *node->board = *board;

    node->player = player;
    node->turn = turn;
    node->move = move;
    node->parent = parent;

    node->children = NULL;

    node->num_children = 0;
    node->visits = 0;
    node->wins = 0;
    node->is_terminal = is_terminal(*node->board);

    return node;
}

/* générer les mouvements légaux de maière optimale */

void safe_shift_unaire(Bitboard* b, int shift){
    if (*b == 0) return;
    for (int i = 0; i < 8; i++){
        if ((shift == directions_masks[i].shift) && (*b & directions_masks[i].mask)){
            *b = 0;
            return;
        }
    }
    int bit = get_lowest_bit_index(*b);
    int dest = bit + shift;
    if (dest < 0 || dest > 24) {
        *b = 0;
        return;
    };
    *b = 1U << dest;
}

Bitboard rays_bobail[25][8];

void init_rays_bobail(Bitboard rays_bobail[25][8]) {
    for (int from = 0; from < 25; from++) {
        for (int dir = 0; dir < 8; dir++) {
            Bitboard current = 1U << from;
            safe_shift_unaire(&current, directions_masks[dir].shift);
            rays_bobail[from][dir] = current;
        }
    }
}

Move* generate_legal_moves_bobail(Node* node, int* count){
    *count = 0;
    Move* moves = malloc(sizeof(Move) * 40);
    Bitboard occupied = node->board->player1 | node->board->player2 | node->board->bobail;
    Bitboard bobail = node->board->bobail;
    int i = get_lowest_bit_index(bobail);
    for (int dir = 0; dir < 8; dir++) {
        Bitboard target = rays_bobail[i][dir];
            // Si un obstacle ou case hors plateau
            if (!(target & occupied) && target != 0) {
                moves[*count].from_bit = bobail;
                moves[*count].to_bit = target;
                (*count)++;
            }
    }
    return moves;
}

Bitboard rays_pawns[25][8][4];

void init_rays_pawns(Bitboard rays_pawns[25][8][4]) {
    for (int from = 0; from < 25; from++) {
        for (int dir = 0; dir < 8; dir++) {
            Bitboard current = 1U << from;
            for (int step = 0; step < 4; step ++){
                safe_shift_unaire(&current, directions_masks[dir].shift);
                rays_pawns[from][dir][step] = current;
            }
        }
    }
}

Move* generate_legal_moves_pawns(Node* node, int* count){
    *count = 0;
    Move* moves = malloc(sizeof(Move) * 40);
    Bitboard occupied = node->board->player1 | node->board->player2 | node->board->bobail;
    Bitboard pawns = (node->player == 0) ? node->board->player1 : node->board->player2;
    for (int i = 0; i < 25; i++) {
        if (pawns & (0b1U << i)) {
            Bitboard pawn = 0b1U << i;
            for (int dir = 0; dir < 8; dir++) {
                for (int step = 0; step < 4; step++) {
                    Bitboard target = rays_pawns[i][dir][step];
                    // Si un obstacle ou case hors plateau
                    if ((target & occupied) || target == 0) {
                        if (step != 0) {
                            moves[*count].from_bit = pawn;
                            moves[*count].to_bit = rays_pawns[i][dir][step-1];
                            (*count)++;
                        }
                        break; // Sort de la boucle "step", essaie une autre direction
                    }
                    // Si on atteint le bout du rayon sans collision
                    if (step == 3) {
                        moves[*count].from_bit = pawn;
                        moves[*count].to_bit = target;
                        (*count)++;
                    }
                }
            }
        }
    }
    return moves;
}

Move* generate_legal_moves(Node* node, int* count) {
    Move* moves = (node->turn == 0) ? generate_legal_moves_bobail(node, count) : generate_legal_moves_pawns(node, count);
    return moves;
}

// Expands a node by generating all children from legal moves
void expand(Node* node) {
    if (node->is_terminal) return;
    int move_count;
    Move* moves = generate_legal_moves(node, &move_count);
    node->children = malloc(sizeof(Node*) * move_count);
    for (int i = 0; i < move_count; i++) {
        Board* new_board = malloc(sizeof(Board));
        *new_board = *node->board;
        play(new_board, moves[i].from_bit, moves[i].to_bit);

        int next_turn = (node->turn == 0) ? 1 : 0;
        int next_player = (next_turn == 0) ? 1 - node->player : node->player;

        Node* child = create_node(new_board, next_player, next_turn, moves[i], node);
        node->children[node->num_children] = child;
        node->num_children++;
        free(new_board);
    }
    free(moves); 
}


/*  ROLLOUT  */


// Plays random moves until the game ends, and returns the winning player
int rollout(Node* node) {
    if (node->is_terminal){
        return victory(*node->board, node->player);
    }
    int v;
    Node* temp_node = create_node(node->board, node->player, node->turn, node->move, NULL);
    while (true) {
        int move_count;
        Move* moves = generate_legal_moves(temp_node, &move_count);
        if (move_count==0) {
            v = 1 - temp_node->player;
            free(temp_node->board);
            free(temp_node);
            return v;
        }

        Move move = moves[rand() % move_count]; // Choose random move
        play(temp_node->board, move.from_bit, move.to_bit);
        free(moves);

        // Switch player after pawn move (turn 1)
        if (temp_node->turn == 1) temp_node->player = 1 - temp_node->player;
        temp_node->turn = 1 - temp_node->turn; // Toggle phase (0 -> 1 or 1 -> 0)

        // Check for a winner
        v = victory(*temp_node->board, temp_node->player);
        if (v >= 0) {
            free(temp_node->board);
            free(temp_node);
            return v;
        }
    }
}


/*  BACKPROPAGATION  */


// Backpropagates the simulation result up the tree, updating visit and wins counts
void backpropagate(Node* node, int winner) {
    while (node != NULL) {
        node->visits++;
        if (node->parent != NULL && node->parent->player == winner){
            node->wins++;
        }
        node = node->parent;
    }
}


/*  MAIN MCTS  */

int fast_cut(Node* root, int iterations){
    int result = -1;
    int best_idx = -1;
    int most_visits = -1;
    int second_most_visits = -1;
    for (int i = 0; i < root->num_children; i++) {
        if (root->children[i]->visits > second_most_visits) {
            if (root->children[i]->visits > most_visits) {
                second_most_visits = most_visits;
                best_idx = i;
                most_visits = root->children[i]->visits;
            } else {
                second_most_visits = root->children[i]->visits;
            }
        }
    }
    if ((most_visits > second_most_visits + iterations - root->visits) || (root->num_children == 1)){
        result = best_idx;
    }
    return result;
}

// Main MCTS function: runs the search for a given number of iterations and returns the best move found
Move mcts(Node** p_root, double exploration, int iterations) {
    Node* root = *p_root;
    int previous_visits = root->visits;
    int winner;
    for (int i = 0; i < iterations - previous_visits; i++) {
        int best_idx = fast_cut(root, iterations);
        if (best_idx >= 0){
            Node* best = root->children[best_idx];
            best->parent = NULL;
            for (int i = 0; i != best_idx && i < root->num_children; i++) {
                free_tree(root->children[i]);
            }
            free(root->children);
            free(root->board);
            free(root);
            *p_root = best;

            return best->move;
        }
        Node* node = tree_traversal(root, exploration);
        if (node->visits == 0 && node->parent){
            winner = rollout(node);
            backpropagate(node, winner);
        } else{
            expand(node);
            if (node->num_children > 0){
                node = node->children[0];
            }
            winner = rollout(node);
            backpropagate(node, winner);
        }
    }
    int best_idx = -1;
    int most_visits = -1;
    for (int i = 0; i < root->num_children; i++) {
        if (root->children[i]->visits > most_visits) {
            best_idx = i;
            most_visits = root->children[i]->visits;
        }
    }
    Node* best = root->children[best_idx];
    best->parent = NULL;
    for (int i = 0; i != best_idx && i < root->num_children; i++) {
        free_tree(root->children[i]);
    }
    free(root->children);
    free(root->board);
    free(root);
    *p_root = best;

    return best->move;
}