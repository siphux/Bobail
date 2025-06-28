#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "board.h"
#include "move_validation.h"
#include "victory.h"
#include "square2bitboard.h"
#include "mcts.h"



// Prompt and read a valid move from the player for the given piece
void read_valid_move(Board* board, int player, int turn, Bitboard* from_bit, Bitboard* to_bit) {
    char* prompt = (turn == 0) ? "plays the bobail" : "plays his pawn";
    char from_square[10], to_square[10];
    printf("Player %d %s : ", player + 1, prompt);             // Ask for move
    scanf("%s %s", from_square, to_square);                    // Read source and destination
    *from_bit = square2bitboard(from_square);                       // Convert to board indices
    *to_bit = square2bitboard(to_square);
    while (!is_legal_move(*board, *from_bit, *to_bit, player, turn)) {              // Repeat until the move is legal
        printf("Error : This move is illegal.\n");
        printf("Player %d %s : ", player + 1, prompt);
        scanf("%s %s", from_square, to_square);
        *from_bit = square2bitboard(from_square);
        *to_bit = square2bitboard(to_square);
    }
}

void print_AImove(Move move, int turn){
    char* piece = (turn == 0) ? "the bobail" : "a pawn";
    printf("AI plays %s: %c%d -> %c%d\n",
            piece,
            'A' + (get_lowest_bit_index(move.from_bit) % 5), 5 - (get_lowest_bit_index(move.from_bit) / 5),
            'A' + (get_lowest_bit_index(move.to_bit) % 5), 5 - (get_lowest_bit_index(move.to_bit) / 5));
}


#define ITERATIONS 1000000
#define EXPLORATION 1.5

int main() {
    srand(time(NULL));
    init_rays_bobail(rays_bobail);
    init_rays_pawns(rays_pawns);

    Board* board = malloc(sizeof(Board));
    init_board(board);
    Node* root=create_node(board, 0, 1, (Move){0b0U, 0b0U}, NULL);
    free(board);

    Bitboard from_bit, to_bit;
    bool vs_ai = false;
    bool ai_game = false;

    // Choose gamemode
    char choice1[10];
    char choice2[10];
    char choice3[10];
    printf("Do you want to make two AIs play against each other? (yes/no): ");
    scanf("%s", choice3);
    int iterations1;
    int iterations2;
    double exploration1;
    double exploration2;
    if (strcmp(choice3, "yes") == 0 || strcmp(choice3, "y") == 0 || strcmp(choice3, "Y") == 0) {
        ai_game = true;
        printf("Number of iterations for AI which begins (int): ");
        scanf("%d", &iterations1);
        printf("Exploration constant for AI which begins (double): ");
        scanf("%lf", &exploration1);
        printf("Number of iterations for AI number 2 (int): ");
        scanf("%d", &iterations2);
        printf("Exploration constant for AI number 2 (double): ");
        scanf("%lf", &exploration2);
    } else {
        printf("Do you want to play against the AI? (yes/no): ");
        scanf("%s", choice1);
        if (strcmp(choice1, "yes") == 0 || strcmp(choice1, "y") == 0 || strcmp(choice1, "Y") == 0) {
            vs_ai = true;
        }
        if (vs_ai){
            printf("Do you want to start? (yes/no): ");
            scanf("%s", choice2);
            if (strcmp(choice2, "no") == 0 || strcmp(choice2, "n") == 0 || strcmp(choice2, "N") == 0) {
                root->player = 1;
            }
        }
    }
    if (ai_game){
        while (true) {
            print_board(*root->board);
            if (root->is_terminal){
                int winner = victory(*root->board,root->player);
                free_tree(root);
                if (winner == 0){
                    printf("AI1 wins !\n");
                } else{
                    printf("AI2 wins !\n");
                }
                return 0;
            }
            if (root->player == 0) {
                Move ai_move = mcts(&root, exploration1, iterations1);
                print_AImove((Move){ai_move.from_bit,ai_move.to_bit}, 1-root->turn);
            } else {
                Move ai_move = mcts(&root, exploration2, iterations2);
                print_AImove((Move){ai_move.from_bit,ai_move.to_bit}, 1-root->turn);
            }
            if (root->turn == 0){
                Board* board = malloc(sizeof(Board));
                int player = root->player;
                *board = *root->board;
                free_tree(root);
                root = create_node(board, player, 0, (Move){0b0U, 0b0U}, NULL);
                free(board);
            }
        }
    } else{
        while (true) {
            print_board(*root->board);
            if (root->is_terminal){
                int winner = victory(*root->board,root->player);
                free_tree(root);
                if (vs_ai && winner == 1){
                    printf("AI wins !\n");
                } else{
                    printf("Player %d wins !\n", winner + 1);
                }
                return 0;
            }
            if (vs_ai && root->player == 1) {
                if (root->turn == 0){
                    Board* board = malloc(sizeof(Board));
                    *board = *root->board;
                    free_tree(root);
                    root = create_node(board, 1, 0, (Move){0b0U, 0b0U}, NULL);
                    free(board);
                }
                Move ai_move = mcts(&root, EXPLORATION, ITERATIONS);
                print_AImove((Move){ai_move.from_bit,ai_move.to_bit}, 1-root->turn);
            } else {
                read_valid_move(root->board, root->player, root->turn, &from_bit, &to_bit);
                play(root->board, from_bit, to_bit);
                root->turn = 1 - root->turn;
                if (root->turn == 0){
                    root->player = 1;
                }
                root->is_terminal = (victory(*root->board, 0) >= 0);
            }
        }
    }
}
