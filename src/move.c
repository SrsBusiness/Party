#include <stdlib.h>
#include <stdio.h>
#include "move.h"
#include "hash.h"
#include "pawn.h"
#include "bishop.h"
#include "rook.h"
#include "knight.h"
#include "king.h"

#ifdef UNIT_TEST
#include "unit_tests.h"
#endif

/*              ******** Incremental Move Updates ********
 *
 *  There are several different kinds of moves, and our incremental update
 *  scheme must account for all of them.
 *
 *  The majority of moves will just update 1 piece:
 *      Normal Moves
 *
 *  There are a few types of moves that update 2 pieces:
 *      Captures (including en passant)
 *      Castling
 *      Promotion
 *
 *  Finally there is the rare case of updating 3 pieces:
 *      Promotion + Capture
 *
 *  Thus, each move will have an primary, a secondary, and a tertiary mover
 *  _________________________________________________________________________
 * | Move Type            | Primary   | Secondary        | Tertiary          |
 * |______________________|___________|__________________|___________________|
 * | Normal Move          | Mover     | -None-           | -None-            |
 * | Captures             | Captor    | -None-           | Captive           |
 * | Castling             | King      | Rook             | -None-            |
 * | En passant           | Captor    | -None            | Captive Pawn      |
 * | Promotion            | Pawn      | Promotion Piece  | -None-            |
 * | Promotion + Capture  | Pawn      | Promotion Piece  | Captive           |
 * |______________________|___________|__________________|___________________|
 *
 * For each mover, we will have an index and a bitboard.
 * 
 * Incremental make and unmake looks something like this:
 * bb[primary]      ^= primary_move;
 * bb[secondary]    ^= secondary_move;
 * bb[tertiary]     ^= tertiary_move;
 *
 * Local copies of irreversible state such as ep_flags, castling rights, etc.
 * will be created during make to be restored in unmake
 *
 * Challenges/Potential Pitfalls:
 *
 *      1.  Captures - When a capture is performed, we need to know what piece
 *          resides on a given square (namely the capture square). However, 
 *          bitboards do not naturally lend themselves to answering this
 *          question. We could iterate through all the pieces and see if they
 *          occupy said square, or we could keep a mailbox representation of
 *          the board that must also be updated incrementally, but we would
 *          then only need to index into an array.
 *
 *      2.  En Passant - Capture square != Captive Square. This is an
 *          important consideration when updating redundant union sets.
 *
 *      3.  State Flags - Obviously these flags need to be modified and
 *          restored during make and unmake. Restoring the flags is merely
 *          a matter of saving a local copy of the flags before make is
 *          performed. However, a considerable amount of logic is required to
 *          properly maintain these flags. This block of logic can either be
 *          implemented in move generation functions or it can be in make.
 *          There are advantages and disadvantages to both. If it is in make,
 *          the logic is more centralized (ease of maintenance), and it is
 *          a more intuitive design. However there may be a duplication
 *          of logic. Consider a function that is capable of generating en
 *          passant captures. The function must have logic to see that an en
 *          passant is possible, which may mean that it parses the state flags,
 *          in which case it may as well compute the new state flags.
 *
 *
 * In conjuction with the incremental bitboard updates, we will need to keep
 * track of incremental updates to the Zobrist hash key as well.
 * 
 **/

const struct board_state BOARD_INITIAL_STATE = {
    WHITE,
    {{
        BITBOARD_KING   & BITBOARD_WHITE,
        BITBOARD_QUEEN  & BITBOARD_WHITE,
        BITBOARD_BISHOP & BITBOARD_WHITE,
        BITBOARD_KNIGHT & BITBOARD_WHITE,
        BITBOARD_ROOK   & BITBOARD_WHITE,
        BITBOARD_PAWN   & BITBOARD_WHITE,
        BITBOARD_WHITE
    }, {
        BITBOARD_KING   & BITBOARD_BLACK,
        BITBOARD_QUEEN  & BITBOARD_BLACK,
        BITBOARD_BISHOP & BITBOARD_BLACK,
        BITBOARD_KNIGHT & BITBOARD_BLACK,
        BITBOARD_ROOK   & BITBOARD_BLACK,
        BITBOARD_PAWN   & BITBOARD_BLACK,
        BITBOARD_BLACK
    }},
    {{1, 1}, {1, 1}, {0, 0}}
};

void board_init(struct board_state *board) {
    *board = BOARD_INITIAL_STATE;
}

int piece_on_square(struct board_state *board, int color, uint64_t square) {
    for (int i = 0; i < ALL; i++) {
        if (board->bb[color][i] & square) {
            return i;
        }
    }
    return NO_PIECE;
}

void bb_make(struct board_state *board, struct move *m) {
    /* primary mover */
    board->bb[board->turn][m->p_mover] ^= m->primary;
    board->bb[board->turn][ALL] ^= m->primary;

    /* secondary mover*/
    if (m->s_mover != NO_PIECE) {
        board->bb[board->turn][m->s_mover] ^= m->secondary;
        board->bb[board->turn][ALL] ^= m->secondary;
    }
    
    /* tertiary mover */
    if (m->t_mover != NO_PIECE) {
        board->bb[!board->turn][m->t_mover] ^= m->tertiary;
        board->bb[!board->turn][ALL] ^= m->tertiary;
    }
}

void make(struct board_state *board, struct move *m) {
    /* modify bitboards */
    bb_make(board, m);
    board->turn = !board->turn;
    board->flags = m->flags;
}

/* unused */
void unmake(struct board_state *board, struct move *m, struct board_flags *restore) {
    board->turn = !board->turn;
    bb_make(board, m);
    board->flags = *restore;
}

/* Checks to see if color is in check */
int in_check(struct board_state *board, int color) {
    uint64_t attacks;
    switch (color) {
        case WHITE:
            attacks = b_pawn_any_attacks(board->bb[BLACK][PAWN]);
            break;
        case BLACK:
            attacks = w_pawn_any_attacks(board->bb[WHITE][PAWN]);
            break;
    }
    attacks |= knight_attacks(board->bb[!color][KNIGHT]) |
        bishop_attacks(board->bb[!color][BISHOP], board->bb[WHITE][ALL] |
                board->bb[BLACK][ALL]) |
        bishop_attacks(board->bb[!color][QUEEN], board->bb[WHITE][ALL] |
                board->bb[BLACK][ALL]) |
        rook_attacks(board->bb[!color][ROOK], board->bb[WHITE][ALL] |
                board->bb[BLACK][ALL]) |
        rook_attacks(board->bb[!color][QUEEN], board->bb[WHITE][ALL] |
                board->bb[BLACK][ALL]) |
        king_attacks(board->bb[!color][KING]);
    return !!(attacks & board->bb[color][KING]);
}

/* move generation */
/* Generate all single pawn pushes excluding promotions */
void generate_pawn_single_pushes_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = w_pawns_able_to_push(board->bb[WHITE][PAWN],
            ~all_pieces(board)) & ~BITBOARD_RANK7;
    uint64_t pawn, move_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        move_mask = pawn | w_single_push_targets(pawn,
                ~all_pieces(board));
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = NO_PIECE;
        m->primary = move_mask;
        /* Should we 0 the other masks? */
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Clear the en passant flag */
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, WHITE)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

void generate_pawn_push_promotions_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = w_pawns_able_to_push(board->bb[WHITE][PAWN],
            ~all_pieces(board)) & BITBOARD_RANK7;
    uint64_t pawn, promote_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        promote_mask = w_single_push_targets(pawn,
                ~all_pieces(board));
        /* Iterate through all promotions pieces */
        for (int promote = QUEEN; promote <= ROOK; promote++) {
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = NO_PIECE;
            m->primary = pawn;
            m->secondary = promote_mask;
            /* Should we 0 the other masks? */
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            /* Clear the en passant flag */
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                /* TODO: Priority Computation */
                priority_queue_push(moves, m, 0);
            }
        }
        pawns ^= pawn;
    }
}

void generate_pawn_double_pushes_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = w_pawns_able_to_double_push(board->bb[WHITE][PAWN],
            ~all_pieces(board));
    uint64_t pawn, move_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        move_mask = pawn | w_double_push_targets(pawn,
                ~all_pieces(board));
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = NO_PIECE;
        m->primary = move_mask;
        /* Should we 0 the other masks? */
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Set white en passant flag, clear black en passant flag */
        m->flags.en_passant[0] = bboard_to_fset(pawn);
        m->flags.en_passant[1] = 0;
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, WHITE)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

void generate_pawn_east_captures_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = w_pawns_able_to_capture_east(board->bb[WHITE][PAWN],
            board->bb[BLACK][ALL] ^ board->bb[BLACK][KING]) & ~BITBOARD_RANK7;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = w_pawn_east_attacks(pawn);
        primary_mask = pawn | capture_square;
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = piece_on_square(board, BLACK, capture_square);
        m->primary = primary_mask;
        m->tertiary = capture_square;
        /* Should we 0 the other masks? */
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Clear en-passant flags */
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, WHITE)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

void generate_pawn_west_captures_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = w_pawns_able_to_capture_west(board->bb[WHITE][PAWN],
            board->bb[BLACK][ALL] ^ board->bb[BLACK][KING]) & ~BITBOARD_RANK7;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = w_pawn_west_attacks(pawn);
        primary_mask = pawn | capture_square;
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = piece_on_square(board, BLACK, capture_square);
        m->primary = primary_mask;
        m->tertiary = capture_square;
        /* Should we 0 the other masks? */
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Clear en-passant flags */
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, WHITE)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

void
generate_pawn_east_captures_promotions_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = w_pawns_able_to_capture_east(board->bb[WHITE][PAWN],
            board->bb[BLACK][ALL] ^ board->bb[BLACK][KING]) & BITBOARD_RANK7;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = w_pawn_east_attacks(pawn);
        primary_mask = pawn;
        int captive = piece_on_square(board, BLACK, capture_square);
        for (int promote = QUEEN; promote <= ROOK; promote++) {
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = captive;
            m->primary = primary_mask;
            m->secondary = capture_square;
            m->tertiary = capture_square;
            /* Should we 0 the other masks? */
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            m->flags.castle_q[1] = board->flags.castle_q[1];
            /* In case the h8 rook was captured */
            m->flags.castle_k[1] = capture_square == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            /* Clear en-passant flags */
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                /* TODO: Priority Computation */
                priority_queue_push(moves, m, 0);
            }
        }
        pawns ^= pawn;
    }
}

void
generate_pawn_west_captures_promotions_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = w_pawns_able_to_capture_west(board->bb[WHITE][PAWN],
            board->bb[BLACK][ALL] ^ board->bb[BLACK][KING]) & BITBOARD_RANK7;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = w_pawn_west_attacks(pawn);
        primary_mask = pawn;
        int captive = piece_on_square(board, BLACK, capture_square);
        for (int promote = QUEEN; promote <= ROOK; promote++) {
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = captive;
            m->primary = primary_mask;
            m->secondary = capture_square;
            m->tertiary = capture_square;
            /* Should we 0 the other masks? */
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            /* In case the a8 rook was captured */
            m->flags.castle_q[1] = capture_square == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            /* Clear en-passant flags */
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                /* TODO: Priority Computation */
                priority_queue_push(moves, m, 0);
            }
        }
        pawns ^= pawn;
    }
}

void generate_pawn_en_passant_white(struct board_state *board,
        struct priority_queue *moves) {
    if (!board->flags.en_passant[BLACK])
        return;
    /* Find en passant squares */
    uint64_t en_passant_square = fset_to_file_fill(board->flags.
            en_passant[BLACK]) & BITBOARD_RANK5;
    uint64_t en_passant_capture_square = north_one(en_passant_square);
    uint64_t pawn, pawns = b_pawn_any_attacks(en_passant_capture_square) &
        board->bb[WHITE][PAWN];
    /* Maximum of 2 possible en passants */
    while (pawns) {
        pawn = pawns & -pawns;
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = PAWN;
        m->primary = pawn | en_passant_capture_square;
        m->tertiary = en_passant_square;
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Clear en-passant flags */
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, WHITE)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

/* Black */
void generate_pawn_single_pushes_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = b_pawns_able_to_push(board->bb[BLACK][PAWN],
            ~all_pieces(board)) & ~BITBOARD_RANK2;
    uint64_t pawn, move_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        move_mask = pawn | b_single_push_targets(pawn,
                ~all_pieces(board));
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = NO_PIECE;
        m->primary = move_mask;
        /* Should we 0 the other masks? */
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Clear the en passant flag */
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, BLACK)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

void generate_pawn_push_promotions_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = b_pawns_able_to_push(board->bb[BLACK][PAWN],
            ~all_pieces(board)) & BITBOARD_RANK2;
    uint64_t pawn, promote_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        promote_mask = b_single_push_targets(pawn,
                ~all_pieces(board));
        /* Iterate through all promotions pieces */
        for (int promote = QUEEN; promote <= ROOK; promote++) {
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = NO_PIECE;
            m->primary = pawn;
            m->secondary = promote_mask;
            /* Should we 0 the other masks? */
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            /* Clear the en passant flag */
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                /* TODO: Priority Computation */
                priority_queue_push(moves, m, 0);
            }
        }
        pawns ^= pawn;
    }
}

void generate_pawn_double_pushes_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = b_pawns_able_to_double_push(board->bb[BLACK][PAWN],
            ~all_pieces(board));
    uint64_t pawn, move_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        move_mask = pawn | b_double_push_targets(pawn,
                ~all_pieces(board));
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = NO_PIECE;
        m->primary = move_mask;
        /* Should we 0 the other masks? */
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Set black en passant flag, clear black en passant flag */
        m->flags.en_passant[0] = 0;
        m->flags.en_passant[1] = bboard_to_fset(pawn);
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, BLACK)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

void generate_pawn_east_captures_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = b_pawns_able_to_capture_east(board->bb[BLACK][PAWN],
            board->bb[WHITE][ALL] ^ board->bb[WHITE][KING]) & ~BITBOARD_RANK2;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = b_pawn_east_attacks(pawn);
        primary_mask = pawn | capture_square;
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = piece_on_square(board, WHITE, capture_square);
        m->primary = primary_mask;
        m->tertiary = capture_square;
        /* Should we 0 the other masks? */
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Clear en-passant flags */
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, BLACK)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

void generate_pawn_west_captures_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = b_pawns_able_to_capture_west(board->bb[BLACK][PAWN],
            board->bb[WHITE][ALL] ^ board->bb[WHITE][KING]) & ~BITBOARD_RANK2;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = b_pawn_west_attacks(pawn);
        primary_mask = pawn | capture_square;
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = piece_on_square(board, WHITE, capture_square);
        m->primary = primary_mask;
        m->tertiary = capture_square;
        /* Should we 0 the other masks? */
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Clear en-passant flags */
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, BLACK)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

void
generate_pawn_east_captures_promotions_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = b_pawns_able_to_capture_east(board->bb[BLACK][PAWN],
            board->bb[WHITE][ALL] ^ board->bb[WHITE][KING]) & BITBOARD_RANK2;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = b_pawn_east_attacks(pawn);
        primary_mask = pawn;
        int captive = piece_on_square(board, WHITE, capture_square);
        for (int promote = QUEEN; promote <= ROOK; promote++) {
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = captive;
            m->primary = primary_mask;
            m->secondary = capture_square;
            m->tertiary = capture_square;
            /* Should we 0 the other masks? */
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            m->flags.castle_q[0] = board->flags.castle_q[0];
            /* In case the h8 rook was captured */
            m->flags.castle_k[0] = capture_square == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            /* Clear en-passant flags */
            m->flags.en_passant[0] = m->flags.en_passant[0] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                /* TODO: Priority Computation */
                priority_queue_push(moves, m, 0);
            }
        }
        pawns ^= pawn;
    }
}

void
generate_pawn_west_captures_promotions_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t pawns = b_pawns_able_to_capture_west(board->bb[BLACK][PAWN],
            board->bb[WHITE][ALL] ^ board->bb[WHITE][KING]) & BITBOARD_RANK2;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = b_pawn_west_attacks(pawn);
        primary_mask = pawn;
        int captive = piece_on_square(board, WHITE, capture_square);
        for (int promote = QUEEN; promote <= ROOK; promote++) {
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = captive;
            m->primary = primary_mask;
            m->secondary = capture_square;
            m->tertiary = capture_square;
            /* Should we 0 the other masks? */
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            /* In case the a1 rook was captured */
            m->flags.castle_q[0] = capture_square == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            /* Clear en-passant flags */
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                /* TODO: Priority Computation */
                priority_queue_push(moves, m, 0);
            }
        }
        pawns ^= pawn;
    }
}

void generate_pawn_en_passant_black(struct board_state *board,
        struct priority_queue *moves) {
    if (!board->flags.en_passant[WHITE])
        return;
    /* Find en passant squares */
    uint64_t en_passant_square =
        fset_to_file_fill(board->flags.en_passant[WHITE]) & BITBOARD_RANK4;
    uint64_t en_passant_capture_square = south_one(en_passant_square);
    uint64_t pawn, pawns = w_pawn_any_attacks(en_passant_capture_square) &
        board->bb[BLACK][PAWN];
    /* Maximum of 2 possible en passants */
    while (pawns) {
        pawn = pawns & -pawns;
        struct move *m = calloc(1, sizeof(struct move));
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = PAWN;
        m->primary = pawn | en_passant_capture_square;
        m->tertiary = en_passant_square;
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = board->flags.castle_k[1];
        /* Clear en-passant flags */
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        /* Check legality */
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, BLACK)) {
            /* TODO: Priority Computation */
            priority_queue_push(moves, m, 0);
        }
        pawns ^= pawn;
    }
}

void generate_bishop_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t bishops = board->bb[WHITE][BISHOP];
    while (bishops) {
        uint64_t bishop = bishops & -bishops;
        uint64_t attacks = bishop_attacks(bishop, 
                board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) & 
            ~board->bb[WHITE][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = BISHOP;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, BLACK, attack);
            m->t_mover = captured_piece;
            m->primary = bishop | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_q[1] = attack == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            m->flags.castle_k[1] = attack == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                priority_queue_push(moves, m, 0);
            }
            attacks ^= attack;
        }
        bishops ^= bishop;
    }
}

void generate_bishop_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t bishops = board->bb[BLACK][BISHOP];
    while (bishops) {
        uint64_t bishop = bishops & -bishops;
        uint64_t attacks = bishop_attacks(bishop, 
                board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) & 
            ~board->bb[BLACK][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = BISHOP;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, WHITE, attack);
            m->t_mover = captured_piece;
            m->primary = bishop | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->flags.castle_q[0] = attack == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[0] = attack == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                priority_queue_push(moves, m, 0);
            }
            attacks ^= attack;
        }
        bishops ^= bishop;
    }
}

void generate_rook_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t rooks = board->bb[WHITE][ROOK];
    while (rooks) {
        uint64_t rook = rooks & -rooks;
        uint64_t attacks = rook_attacks(rook, 
                board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) & 
            ~board->bb[WHITE][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = ROOK;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, BLACK, attack);
            m->t_mover = captured_piece;
            m->primary = rook | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->flags.castle_q[0] = rook == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = attack == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = rook == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = attack == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                priority_queue_push(moves, m, 0);
            }
            attacks ^= attack;
        }
        rooks ^= rook;
    }
}

void generate_rook_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t rooks = board->bb[BLACK][ROOK];
    while (rooks) {
        uint64_t rook = rooks & -rooks;
        uint64_t attacks = rook_attacks(rook, 
                board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) & 
            ~board->bb[BLACK][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = ROOK;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, WHITE, attack);
            m->t_mover = captured_piece;
            m->primary = rook | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->flags.castle_q[0] = attack == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = rook == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = attack == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = rook == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                priority_queue_push(moves, m, 0);
            }
            attacks ^= attack;
        }
        rooks ^= rook;
    }
}

void generate_queen_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t queens = board->bb[WHITE][QUEEN];
    while (queens) {
        uint64_t queen = queens & -queens;
        uint64_t attacks = 
            (bishop_attacks(queen, board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) |
            rook_attacks(queen, board->bb[WHITE][ALL] | board->bb[BLACK][ALL])) &
            ~board->bb[WHITE][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = QUEEN;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, BLACK, attack);
            m->t_mover = captured_piece;
            m->primary = queen | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_q[1] = attack == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            m->flags.castle_k[1] = attack == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                priority_queue_push(moves, m, 0);
            }
            attacks ^= attack;
        }
        queens ^= queen;
    }
}

void generate_queen_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t queens = board->bb[BLACK][QUEEN];
    while (queens) {
        uint64_t queen = queens & -queens;
        uint64_t attacks = 
            (bishop_attacks(queen, board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) |
            rook_attacks(queen, board->bb[WHITE][ALL] | board->bb[BLACK][ALL])) &
            ~board->bb[BLACK][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = QUEEN;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, WHITE, attack);
            m->t_mover = captured_piece;
            m->primary = queen | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->flags.castle_q[0] = attack == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[0] = attack == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                priority_queue_push(moves, m, 0);
            }
            attacks ^= attack;
        }
        queens ^= queen;
    }
}

void generate_knight_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t knights = board->bb[WHITE][KNIGHT];
    while (knights) {
        uint64_t knight = knights & -knights;
        uint64_t attacks = knight_attacks(knight) & ~board->bb[WHITE][ALL];
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = KNIGHT;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, BLACK, attack);
            m->t_mover = captured_piece;
            m->primary = knight | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_q[1] = attack == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            m->flags.castle_k[1] = attack == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                priority_queue_push(moves, m, 0);
            }
            attacks ^= attack;
        }
        knights ^= knight;
    }
}

void generate_knight_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    uint64_t knights = board->bb[BLACK][KNIGHT];
    while (knights) {
        uint64_t knight = knights & -knights;
        uint64_t attacks = knight_attacks(knight) & ~board->bb[BLACK][ALL];
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            m->p_mover = KNIGHT;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, WHITE, attack);
            m->t_mover = captured_piece;
            m->primary = knight | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->flags.castle_q[0] = attack == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[0] = attack == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                priority_queue_push(moves, m, 0);
            }
            attacks ^= attack;
        }
        knights ^= knight;
    }
}
