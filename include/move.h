#include "bitboard.h"

extern const board_state initial_state;

void board_init(board_state *);
void make(board_state *, move *);
void unmake(board_state *, move *);

void generate_pawn_moves(board_state *board, move *candidates,
        int *move_index);
void generate_single_pushes(board_state *board, move *candidates, 
        int *move_index);
void generate_double_pushes(board_state *board, move *candidates, 
        int *move_index);
void generate_pawn_captures(board_state *board, move *candidates, 
        int *move_index);
void generate_en_passant(board_state *board, move *candidates, 
        int *move_index);
