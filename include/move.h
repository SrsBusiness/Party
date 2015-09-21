#ifndef MOVE_H
#define MOVE_H

#include "bitboard.h"
#include "pqueue.h"

extern const struct board_state BOARD_INITIAL_STATE;

void board_init(struct board_state *);
void make(struct board_state *, struct move *);
void unmake(struct board_state *, struct move *, struct board_flags *);
int in_check(struct board_state *board, int color);

/* Move generation functions 
 * TODO: Check for legality e.g. checks, castling through/while in check, etc.
 *      Checks: Play the generated move and see if king is in check
 *      Castling: trivial
 **/

void generate_pawn_single_pushes_white(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_push_promotions_white(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_double_pushes_white(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_east_captures_white(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_west_captures_white(struct board_state *board,
        struct priority_queue *moves);

void
generate_pawn_east_captures_promotions_white(struct board_state *board,
        struct priority_queue *moves);

void
generate_pawn_west_captures_promotions_white(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_en_passant_white(struct board_state *board,
        struct priority_queue *moves);

/* Black */
void generate_pawn_single_pushes_black(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_push_promotions_black(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_double_pushes_black(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_east_captures_black(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_west_captures_black(struct board_state *board,
        struct priority_queue *moves);

void
generate_pawn_east_captures_promotions_black(struct board_state *board,
        struct priority_queue *moves);

void
generate_pawn_west_captures_promotions_black(struct board_state *board,
        struct priority_queue *moves);

void generate_pawn_en_passant_black(struct board_state *board,
        struct priority_queue *moves);

void generate_bishop_moves_white(struct board_state *board,
        struct priority_queue *moves);

void generate_bishop_moves_black(struct board_state *board,
        struct priority_queue *moves);

void generate_rook_moves_white(struct board_state *board,
        struct priority_queue *moves);

void generate_rook_moves_black(struct board_state *board,
        struct priority_queue *moves);

void generate_queen_moves_black(struct board_state *board,
        struct priority_queue *moves);

void generate_queen_moves_black(struct board_state *board,
        struct priority_queue *moves);

void generate_knight_moves_white(struct board_state *board,
        struct priority_queue *moves);

void generate_knight_moves_black(struct board_state *board,
        struct priority_queue *moves);

void generate_king_moves_white(struct board_state *board,
        struct priority_queue *moves);

void generate_king_moves_black(struct board_state *board,
        struct priority_queue *moves);

void generate_castle_white(struct board_state *board,
        struct priority_queue *moves);

void generate_castle_black(struct board_state *board,
        struct priority_queue *moves);
#endif
