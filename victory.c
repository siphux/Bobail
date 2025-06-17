#include "victory.h"
#include "masks.h"    // Pour MASK_DOWN, MASK_UP, MASK_LEFT, MASK_RIGHT
#include <stdbool.h> // Pour le type bool

// Rappel des règles de victoire/défaite :
// 1. Un joueur gagne s'il parvient à amener le Bobail sur l'une des cases de départ de ses pions.
// 2. Un joueur perd si le Bobail est bloqué et ne peut plus être déplacé au début de son tour.

int victory(Board board, int player_to_move) { // Renommé 'player' en 'player_to_move' pour la clarté
    Bitboard bobail_pos = board.bobail;
    Bitboard occupied_squares = board.player1 | board.player2 | bobail_pos;

    // Condition de Victoire 1: Bobail sur une case de départ
    //  - Si le Bobail est sur la ligne de départ du joueur 1 (MASK_DOWN), le joueur 1 gagne.
    //    On assume que le joueur 1 correspond à l'index 0.
    if (bobail_pos & MASK_DOWN) { // MASK_DOWN est la ligne de départ de player1
        return 0; // Joueur 1 (index 0) gagne
    }
    //  - Si le Bobail est sur la ligne de départ du joueur 2 (MASK_UP), le joueur 2 gagne.
    //    On assume que le joueur 2 correspond à l'index 1.
    if (bobail_pos & MASK_UP) {   // MASK_UP est la ligne de départ de player2
        return 1; // Joueur 2 (index 1) gagne
    }

    // Condition de Défaite 2: Bobail bloqué au début du tour du joueur 'player_to_move'
    // Si cette fonction est appelée au début du tour de 'player_to_move',
    // et que le Bobail ne peut pas bouger, alors 'player_to_move' perd.
    // Ce qui signifie que l'autre joueur (1 - player_to_move) gagne.

    // Vérifier si le Bobail est entouré (bloqué)
    bool bobail_is_surrounded = true; // Supposons qu'il est entouré initialement

    Bitboard adjacent_squares = 0b0U; // Bitboard pour les cases adjacentes au Bobail

    // Générer les cases adjacentes en tenant compte des bords du plateau (5x5)
    // Les décalages sont :
    // Haut-Gauche: >> 6  (valeur - 5 - 1)
    // Haut:        >> 5  (valeur - 5)
    // Haut-Droite: >> 4  (valeur - 5 + 1)
    // Gauche:      >> 1  (valeur - 1)
    // Droite:      << 1  (valeur + 1)
    // Bas-Gauche:  << 4  (valeur + 5 - 1)
    // Bas:         << 5  (valeur + 5)
    // Bas-Droite:  << 6  (valeur + 5 + 1)

    // Case 1: Bobail dans la colonne de gauche (A)
    if (bobail_pos & MASK_LEFT) { // MASK_LEFT = 0b00001...00001
        adjacent_squares |= (bobail_pos >> 5); // Haut
        adjacent_squares |= (bobail_pos >> 4); // Haut-Droite
        adjacent_squares |= (bobail_pos << 1); // Droite
        adjacent_squares |= (bobail_pos << 5); // Bas
        adjacent_squares |= (bobail_pos << 6); // Bas-Droite
    }
    // Case 2: Bobail dans la colonne de droite (E)
    else if (bobail_pos & MASK_RIGHT) { // MASK_RIGHT = 0b10000...10000
        adjacent_squares |= (bobail_pos >> 6); // Haut-Gauche
        adjacent_squares |= (bobail_pos >> 5); // Haut
        adjacent_squares |= (bobail_pos >> 1); // Gauche
        adjacent_squares |= (bobail_pos << 4); // Bas-Gauche
        adjacent_squares |= (bobail_pos << 5); // Bas
    }
    // Case 3: Bobail dans une colonne du milieu (B, C, D)
    else {
        adjacent_squares |= (bobail_pos >> 6); // Haut-Gauche
        adjacent_squares |= (bobail_pos >> 5); // Haut
        adjacent_squares |= (bobail_pos >> 4); // Haut-Droite
        adjacent_squares |= (bobail_pos >> 1); // Gauche
        adjacent_squares |= (bobail_pos << 1); // Droite
        adjacent_squares |= (bobail_pos << 4); // Bas-Gauche
        adjacent_squares |= (bobail_pos << 5); // Bas
        adjacent_squares |= (bobail_pos << 6); // Bas-Droite
    }
    
    // Le Bobail n'est pas techniquement sur les lignes MASK_UP ou MASK_DOWN ici, sinon
    // une condition de victoire aurait déjà été rencontrée.
    // Donc, les décalages >>5 et <<5 sont sûrs (ne sortiront pas du bitboard uint32_t
    // de manière à créer des faux positifs avec des bits non utilisés, mais peuvent devenir 0).
    // Le FULL_BOARD_MASK serait utile pour s'assurer que adjacent_squares ne contient
    // que des bits dans les 25 cases valides.
    Bitboard full_board_mask = (1U << 25) - 1; // Masque pour les 25 cases 0x1FFFFFF
    adjacent_squares &= full_board_mask;


    // Si une case adjacente est vide, le Bobail n'est pas entouré
    // Une case vide est une case qui n'est PAS dans occupied_squares.
    // (~occupied_squares) donne les cases vides.
    if (adjacent_squares & (~occupied_squares & full_board_mask)) {
        bobail_is_surrounded = false;
    }

    if (bobail_is_surrounded) {
        // Le Bobail est bloqué. Le joueur 'player_to_move' (celui dont c'est le tour) perd.
        // L'autre joueur gagne.
        return (1 - player_to_move); // Si J0 doit jouer, J1 gagne. Si J1 doit jouer, J0 gagne.
    }

    return -1; // Pas de vainqueur encore, Bobail pas sur ligne de fond, et pas bloqué.
}
