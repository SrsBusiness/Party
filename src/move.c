#include <stdlib.h>
#include <stdio.h>
#include "move.h"
#include "hash.h"
#include "pawn.h"
#include "bishop.h"
#include "rook.h"
#include "knight.h"
#include "king.h"
#include "bboard_utils.h"

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
    ZOBRIST_initial_hash,
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
    board->hash = ZOBRIST_incremental_update(board, m);
    bb_make(board, m);
    board->turn = !board->turn;
    board->flags = m->flags;
}

/* unused */
void unmake(struct board_state *board, struct move *m, struct board_flags *old_flags, uint64_t old_hash) {
    board->turn = !board->turn;
    bb_make(board, m);
    board->flags = *old_flags;
    board->hash = old_hash;
}

uint64_t all_attacks(struct board_state *board, int color) {
    uint64_t attacks;
    switch (color) {
        case WHITE:
            attacks = w_pawn_any_attacks(board->bb[WHITE][PAWN]);
            break;
        case BLACK:
            attacks = b_pawn_any_attacks(board->bb[BLACK][PAWN]);
            break;
    }
    /* Diagonal Attacks */
    uint64_t diags = board->bb[color][BISHOP] | board->bb[color][QUEEN];
    while (diags) {
        uint64_t diag = diags & -diags;
        attacks |= bishop_attacks(diag, board->bb[WHITE][ALL] | board->bb[BLACK][ALL]);
        diags ^= diag;
    }
    /* Lateral Attacks */
    uint64_t laterals = board->bb[color][ROOK] | board->bb[color][QUEEN];
    while (laterals) {
        uint64_t lateral = laterals & -laterals;
        attacks |= rook_attacks(lateral, board->bb[WHITE][ALL] | board->bb[BLACK][ALL]);
        laterals ^= lateral;
    }
    attacks |= (knight_attacks(board->bb[color][KNIGHT]) |
            king_attacks(board->bb[color][KING]));
    return attacks;
}

/* Checks to see if color is in check */
int in_check(struct board_state *board, int color) {
    return !!(all_attacks(board, !color) & board->bb[color][KING]);
}

int in_checkmate(struct board_state *board, int color) {
    int move_count;
    switch(color) {
        case WHITE:
            move_count = generate_moves_white(board, NULL);
            break;
        case BLACK:
            move_count = generate_moves_black(board, NULL);
            break;
    }
    /* If in check and no legal moves */
    return in_check(board, color) && move_count == 0;
}

int in_stalemate(struct board_state *board, int color) {
    int move_count;
    switch(color) {
        case WHITE:
            move_count = generate_moves_white(board, NULL);
            break;
        case BLACK:
            move_count = generate_moves_black(board, NULL);
            break;
    }
    /* If not in check and no legal moves */
    return !in_check(board, color) && move_count == 0;
}

int no_legal_moves(struct board_state *board, int color) {
    int move_count;
    switch(color) {
        case WHITE:
            move_count = generate_moves_white(board, NULL);
            break;
        case BLACK:
            move_count = generate_moves_black(board, NULL);
            break;
    }
    return move_count == 0;
}

/* move generation */
/* Generate all single pawn pushes excluding promotions */
int generate_pawn_single_pushes_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t pawns = w_pawns_able_to_push(board->bb[WHITE][PAWN],
            ~all_pieces(board)) & ~BITBOARD_RANK7;
    uint64_t pawn, move_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        move_mask = pawn | w_single_push_targets(pawn,
                ~all_pieces(board));
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = NO_PIECE;
        m->primary = move_mask;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_push_promotions_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
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
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = NO_PIECE;
            m->primary = pawn;
            m->secondary = promote_mask;
            m->primary_src = pawn;
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
                move_count++;
                /* TODO: Priority Computation */
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_double_pushes_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t pawns = w_pawns_able_to_double_push(board->bb[WHITE][PAWN],
            ~all_pieces(board));
    uint64_t pawn, move_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        move_mask = pawn | w_double_push_targets(pawn,
                ~all_pieces(board));
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = NO_PIECE;
        m->primary = move_mask;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_east_captures_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t pawns = w_pawns_able_to_capture_east(board->bb[WHITE][PAWN],
            board->bb[BLACK][ALL] ^ board->bb[BLACK][KING]) & ~BITBOARD_RANK7;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = w_pawn_east_attacks(pawn);
        primary_mask = pawn | capture_square;
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = piece_on_square(board, BLACK, capture_square);
        m->primary = primary_mask;
        m->tertiary = capture_square;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_west_captures_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t pawns = w_pawns_able_to_capture_west(board->bb[WHITE][PAWN],
            board->bb[BLACK][ALL] ^ board->bb[BLACK][KING]) & ~BITBOARD_RANK7;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = w_pawn_west_attacks(pawn);
        primary_mask = pawn | capture_square;
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = piece_on_square(board, BLACK, capture_square);
        m->primary = primary_mask;
        m->tertiary = capture_square;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int
generate_pawn_east_captures_promotions_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
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
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = captive;
            m->primary = primary_mask;
            m->secondary = capture_square;
            m->tertiary = capture_square;
            m->primary_src = pawn;
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
                move_count++;
                /* TODO: Priority Computation */
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
        }
        pawns ^= pawn;
    }
    return move_count;
}

int
generate_pawn_west_captures_promotions_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
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
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = captive;
            m->primary = primary_mask;
            m->secondary = capture_square;
            m->tertiary = capture_square;
            m->primary_src = pawn;
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
                move_count++;
                /* TODO: Priority Computation */
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_en_passant_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    if (!board->flags.en_passant[BLACK])
        return move_count;
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
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = PAWN;
        m->primary = pawn | en_passant_capture_square;
        m->tertiary = en_passant_square;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                //int32_t score = get_transposition_score(tmp.hash);
                int32_t score = 0;
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    return generate_pawn_single_pushes_white(board, moves) +
        generate_pawn_push_promotions_white(board, moves) +
        generate_pawn_double_pushes_white(board, moves) +
        generate_pawn_east_captures_white(board, moves) +
        generate_pawn_west_captures_white(board, moves) +
        generate_pawn_east_captures_promotions_white(board, moves) +
        generate_pawn_west_captures_promotions_white(board, moves) +
        generate_pawn_en_passant_white(board, moves);
}

/* Black */
int generate_pawn_single_pushes_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t pawns = b_pawns_able_to_push(board->bb[BLACK][PAWN],
            ~all_pieces(board)) & ~BITBOARD_RANK2;
    uint64_t pawn, move_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        move_mask = pawn | b_single_push_targets(pawn,
                ~all_pieces(board));
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = NO_PIECE;
        m->primary = move_mask;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, -score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_push_promotions_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
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
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = NO_PIECE;
            m->primary = pawn;
            m->secondary = promote_mask;
            m->primary_src = pawn;
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
                move_count++;
                /* TODO: Priority Computation */
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, -score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_double_pushes_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t pawns = b_pawns_able_to_double_push(board->bb[BLACK][PAWN],
            ~all_pieces(board));
    uint64_t pawn, move_mask;
    while (pawns) {
        pawn = pawns & -pawns;
        move_mask = pawn | b_double_push_targets(pawn,
                ~all_pieces(board));
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = NO_PIECE;
        m->primary = move_mask;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, -score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_east_captures_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t pawns = b_pawns_able_to_capture_east(board->bb[BLACK][PAWN],
            board->bb[WHITE][ALL] ^ board->bb[WHITE][KING]) & ~BITBOARD_RANK2;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = b_pawn_east_attacks(pawn);
        primary_mask = pawn | capture_square;
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = piece_on_square(board, WHITE, capture_square);
        m->primary = primary_mask;
        m->tertiary = capture_square;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, -score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_west_captures_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t pawns = b_pawns_able_to_capture_west(board->bb[BLACK][PAWN],
            board->bb[WHITE][ALL] ^ board->bb[WHITE][KING]) & ~BITBOARD_RANK2;
    uint64_t pawn, primary_mask, capture_square;
    while (pawns) {
        pawn = pawns & -pawns;
        capture_square = b_pawn_west_attacks(pawn);
        primary_mask = pawn | capture_square;
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = piece_on_square(board, WHITE, capture_square);
        m->primary = primary_mask;
        m->tertiary = capture_square;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, -score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int
generate_pawn_east_captures_promotions_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
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
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = captive;
            m->primary = primary_mask;
            m->secondary = capture_square;
            m->tertiary = capture_square;
            m->primary_src = pawn;
            /* Should we 0 the other masks? */
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            m->flags.castle_q[0] = board->flags.castle_q[0];
            /* In case the h8 rook was captured */
            m->flags.castle_k[0] = capture_square == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            /* Clear en-passant flags */
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                move_count++;
                /* TODO: Priority Computation */
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, -score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
        }
        pawns ^= pawn;
    }
    return move_count;
}

int
generate_pawn_west_captures_promotions_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
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
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = PAWN;
            m->s_mover = promote;
            m->t_mover = captive;
            m->primary = primary_mask;
            m->secondary = capture_square;
            m->tertiary = capture_square;
            m->primary_src = pawn;
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
                move_count++;
                /* TODO: Priority Computation */
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, -score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_en_passant_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    if (!board->flags.en_passant[WHITE])
        return move_count;
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
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->t_mover = PAWN;
        m->primary = pawn | en_passant_capture_square;
        m->tertiary = en_passant_square;
        m->primary_src = pawn;
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
            move_count++;
            /* TODO: Priority Computation */
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, -score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        pawns ^= pawn;
    }
    return move_count;
}

int generate_pawn_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    return generate_pawn_single_pushes_black(board, moves) +
        generate_pawn_push_promotions_black(board, moves) +
        generate_pawn_double_pushes_black(board, moves) +
        generate_pawn_east_captures_black(board, moves) +
        generate_pawn_west_captures_black(board, moves) +
        generate_pawn_east_captures_promotions_black(board, moves) +
        generate_pawn_west_captures_promotions_black(board, moves) +
        generate_pawn_en_passant_black(board, moves);
}

int generate_bishop_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t bishops = board->bb[WHITE][BISHOP];
    while (bishops) {
        uint64_t bishop = bishops & -bishops;
        uint64_t attacks = bishop_attacks(bishop, 
                board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) & 
            ~board->bb[WHITE][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = BISHOP;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, BLACK, attack);
            m->t_mover = captured_piece;
            m->primary = bishop | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->primary_src = bishop;
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_q[1] = attack == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            m->flags.castle_k[1] = attack == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                move_count++;
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
            attacks ^= attack;
        }
        bishops ^= bishop;
    }
    return move_count;
}

int generate_bishop_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t bishops = board->bb[BLACK][BISHOP];
    while (bishops) {
        uint64_t bishop = bishops & -bishops;
        uint64_t attacks = bishop_attacks(bishop, 
                board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) & 
            ~board->bb[BLACK][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = BISHOP;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, WHITE, attack);
            m->t_mover = captured_piece;
            m->primary = bishop | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->primary_src = bishop;
            m->flags.castle_q[0] = attack == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[0] = attack == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                move_count++;
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, -score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
            attacks ^= attack;
        }
        bishops ^= bishop;
    }
    return move_count;
}

int generate_rook_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t rooks = board->bb[WHITE][ROOK];
    while (rooks) {
        uint64_t rook = rooks & -rooks;
        uint64_t attacks = rook_attacks(rook, 
                board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) & 
            ~board->bb[WHITE][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = ROOK;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, BLACK, attack);
            m->t_mover = captured_piece;
            m->primary = rook | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->primary_src = rook;
            m->flags.castle_q[0] = rook == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = attack == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = rook == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = attack == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                move_count++;
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
            attacks ^= attack;
        }
        rooks ^= rook;
    }
    return move_count;
}

int generate_rook_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t rooks = board->bb[BLACK][ROOK];
    while (rooks) {
        uint64_t rook = rooks & -rooks;
        uint64_t attacks = rook_attacks(rook, 
                board->bb[WHITE][ALL] | board->bb[BLACK][ALL]) & 
            ~board->bb[BLACK][ALL]; /* Exclude allies */
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = ROOK;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, WHITE, attack);
            m->t_mover = captured_piece;
            m->primary = rook | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->primary_src = rook;
            m->flags.castle_q[0] = attack == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = rook == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = attack == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = rook == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                move_count++;
                if (moves) { 
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, -score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
            attacks ^= attack;
        }
        rooks ^= rook;
    }
    return move_count;
}

int generate_queen_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
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
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = QUEEN;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, BLACK, attack);
            m->t_mover = captured_piece;
            m->primary = queen | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->primary_src = queen;
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_q[1] = attack == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            m->flags.castle_k[1] = attack == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                move_count++;
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
            attacks ^= attack;
        }
        queens ^= queen;
    }
    return move_count;
}

int generate_queen_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
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
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = QUEEN;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, WHITE, attack);
            m->t_mover = captured_piece;
            m->primary = queen | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->primary_src = queen;
            m->flags.castle_q[0] = attack == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[0] = attack == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                move_count++;
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, -score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
            attacks ^= attack;
        }
        queens ^= queen;
    }
    return move_count;
}

int generate_knight_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t knights = board->bb[WHITE][KNIGHT];
    while (knights) {
        uint64_t knight = knights & -knights;
        uint64_t attacks = knight_attacks(knight) & ~board->bb[WHITE][ALL];
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = KNIGHT;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, BLACK, attack);
            m->t_mover = captured_piece;
            m->primary = knight | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->primary_src = knight;
            m->flags.castle_q[0] = board->flags.castle_q[0];
            m->flags.castle_q[1] = attack == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
            m->flags.castle_k[0] = board->flags.castle_k[0];
            m->flags.castle_k[1] = attack == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, WHITE)) {
                move_count++;
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
            attacks ^= attack;
        }
        knights ^= knight;
    }
    return move_count;
}

int generate_knight_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t knights = board->bb[BLACK][KNIGHT];
    while (knights) {
        uint64_t knight = knights & -knights;
        uint64_t attacks = knight_attacks(knight) & ~board->bb[BLACK][ALL];
        while (attacks) {
            uint64_t attack = attacks & -attacks;
            struct move *m = calloc(1, sizeof(struct move));
            if (m == NULL) {
                fprintf(stderr, "Out of memory in %s:%s:%d",
                        __FILE__, __FUNCTION__, __LINE__);
                exit(-1);
            }
            m->p_mover = KNIGHT;
            m->s_mover = NO_PIECE;
            int captured_piece = piece_on_square(board, WHITE, attack);
            m->t_mover = captured_piece;
            m->primary = knight | attack;
            m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
            m->primary_src = knight;
            m->flags.castle_q[0] = attack == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
            m->flags.castle_q[1] = board->flags.castle_q[1];
            m->flags.castle_k[0] = attack == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
            m->flags.castle_k[1] = board->flags.castle_k[1];
            m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
            /* Check legality */
            struct board_state tmp = *board;
            make(&tmp, m);
            if (!in_check(&tmp, BLACK)) {
                move_count++;
                if (moves) {
                    int32_t score = get_transposition_score(tmp.hash);
                    score = score == -1 ? 0 : score;
                    priority_queue_push(moves, m, -score);
                } else {
                    free(m);
                }
            } else {
                free(m);
            }
            attacks ^= attack;
        }
        knights ^= knight;
    }
    return move_count;
}

int generate_king_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t king = board->bb[WHITE][KING];
    uint64_t attacks = king_attacks(king) & ~board->bb[WHITE][ALL];
    while (attacks) {
        uint64_t attack = attacks & -attacks;
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = KING;
        m->s_mover = NO_PIECE;
        int captured_piece = piece_on_square(board, BLACK, attack);
        m->t_mover = captured_piece;
        m->primary = king | attack;
        m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
        m->primary_src = king;
        m->flags.castle_q[0] = king == BITBOARD_E1 ? 0 : board->flags.castle_q[0];
        m->flags.castle_q[1] = attack == BITBOARD_A8 ? 0 : board->flags.castle_q[1];
        m->flags.castle_k[0] = king == BITBOARD_E1 ? 0 : board->flags.castle_k[0];
        m->flags.castle_k[1] = attack == BITBOARD_H8 ? 0 : board->flags.castle_k[1];
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, WHITE)) {
            move_count++;
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        attacks ^= attack;
    }
    return move_count;
}

int generate_king_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t king = board->bb[BLACK][KING];
    uint64_t attacks = king_attacks(king) & ~board->bb[BLACK][ALL];
    while (attacks) {
        uint64_t attack = attacks & -attacks;
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = KING;
        m->s_mover = NO_PIECE;
        int captured_piece = piece_on_square(board, WHITE, attack);
        m->t_mover = captured_piece;
        m->primary = king | attack;
        m->tertiary = captured_piece == NO_PIECE ? 0 : attack;
        m->primary_src = king;
        m->flags.castle_q[0] = attack == BITBOARD_A1 ? 0 : board->flags.castle_q[0];
        m->flags.castle_q[1] = king == BITBOARD_E8 ? 0 : board->flags.castle_q[1];
        m->flags.castle_k[0] = attack == BITBOARD_H1 ? 0 : board->flags.castle_k[0];
        m->flags.castle_k[1] = king == BITBOARD_E8 ? 0 : board->flags.castle_k[1];
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        struct board_state tmp = *board;
        make(&tmp, m);
        if (!in_check(&tmp, BLACK)) {
            move_count++;
            if (moves) {
                int32_t score = get_transposition_score(tmp.hash);
                score = score == -1 ? 0 : score;
                priority_queue_push(moves, m, -score);
            } else {
                free(m);
            }
        } else {
            free(m);
        }
        attacks ^= attack;
    }
    return move_count;
}

int generate_castle_white(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t enemy_attacks = all_attacks(board, BLACK);
    /* Squares between king and rook must be unobstructed, king cannot
     * castle through check
     **/
    if (board->flags.castle_q[WHITE] &
            !((BITBOARD_E1 | BITBOARD_D1 | BITBOARD_C1) & enemy_attacks) &
            !((BITBOARD_D1 | BITBOARD_C1 | BITBOARD_B1) & (board->bb[WHITE][ALL] | board->bb[BLACK][ALL]))) {
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = KING;
        m->s_mover = ROOK;
        m->t_mover = NO_PIECE;
        m->primary = BITBOARD_E1 | BITBOARD_C1;
        m->secondary = BITBOARD_A1 | BITBOARD_D1;
        m->primary_src = BITBOARD_E1;
        m->flags.castle_q[0] = 0;
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = 0;
        m->flags.castle_k[1] = board->flags.castle_k[1];
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        move_count++;
        if (moves) {
            struct board_state tmp = *board;
            make(&tmp, m);
            int32_t score = get_transposition_score(tmp.hash);
            score = score == -1 ? 0 : score;
            priority_queue_push(moves, m, score);
        } else {
            free(m);
        }
    }
    if (board->flags.castle_k[WHITE] &
            !((BITBOARD_E1 | BITBOARD_F1 | BITBOARD_G1) & enemy_attacks) &
            !((BITBOARD_F1 | BITBOARD_G1) & (board->bb[WHITE][ALL] | board->bb[BLACK][ALL]))) {
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = KING;
        m->s_mover = ROOK;
        m->t_mover = NO_PIECE;
        m->primary = BITBOARD_E1 | BITBOARD_G1;
        m->secondary = BITBOARD_H1 | BITBOARD_F1;
        m->primary_src = BITBOARD_E1;
        m->flags.castle_q[0] = 0;
        m->flags.castle_q[1] = board->flags.castle_q[1];
        m->flags.castle_k[0] = 0;
        m->flags.castle_k[1] = board->flags.castle_k[1];
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        move_count++;
        if (moves) {
            struct board_state tmp = *board;
            make(&tmp, m);
            int32_t score = get_transposition_score(tmp.hash);
            score = score == -1 ? 0 : score;
            priority_queue_push(moves, m, score);
        } else {
            free(m);
        }
    }
    return move_count;
}

int generate_castle_black(struct board_state *board,
        struct priority_queue *moves) {
    int move_count = 0;
    uint64_t enemy_attacks = all_attacks(board, WHITE);
    if (board->flags.castle_q[BLACK] &
            !((BITBOARD_E8 | BITBOARD_D8 | BITBOARD_C8) & enemy_attacks) &
            !((BITBOARD_D8 | BITBOARD_C8 | BITBOARD_B8) & (board->bb[WHITE][ALL] | board->bb[BLACK][ALL]))) {
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = KING;
        m->s_mover = ROOK;
        m->t_mover = NO_PIECE;
        m->primary = BITBOARD_E8 | BITBOARD_C8;
        m->secondary = BITBOARD_A8 | BITBOARD_D8;
        m->primary_src = BITBOARD_E8;
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = 0;
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = 0;
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        move_count++;
        if (moves) {
            struct board_state tmp = *board;
            make(&tmp, m);
            int32_t score = get_transposition_score(tmp.hash);
            score = score == -1 ? 0 : score;
            priority_queue_push(moves, m, -score);
        } else {
            free(m);
        }
    }
    if (board->flags.castle_k[BLACK] &
            !((BITBOARD_E8 | BITBOARD_F8 | BITBOARD_G8) & enemy_attacks) &
            !((BITBOARD_F8 | BITBOARD_G8) & (board->bb[WHITE][ALL] | board->bb[BLACK][ALL]))) {
        struct move *m = calloc(1, sizeof(struct move));
        if (m == NULL) {
            fprintf(stderr, "Out of memory in %s:%s:%d",
                    __FILE__, __FUNCTION__, __LINE__);
            exit(-1);
        }
        m->p_mover = KING;
        m->s_mover = ROOK;
        m->t_mover = NO_PIECE;
        m->primary = BITBOARD_E8 | BITBOARD_G8;
        m->secondary = BITBOARD_H8 | BITBOARD_F8;
        m->primary_src = BITBOARD_E8;
        m->flags.castle_q[0] = board->flags.castle_q[0];
        m->flags.castle_q[1] = 0;
        m->flags.castle_k[0] = board->flags.castle_k[0];
        m->flags.castle_k[1] = 0;
        m->flags.en_passant[0] = m->flags.en_passant[1] = 0;
        move_count++;
        if (moves) {
            struct board_state tmp = *board;
            make(&tmp, m);
            int32_t score = get_transposition_score(tmp.hash);
            score = score == -1 ? 0 : score;
            priority_queue_push(moves, m, -score);
        } else {
            free(m);
        }
    }
    return move_count;
}

int generate_moves_white(struct board_state *board,
        struct priority_queue *moves) {
    return generate_king_moves_white(board, moves) +
        generate_queen_moves_white(board, moves) +
        generate_bishop_moves_white(board, moves) +
        generate_knight_moves_white(board, moves) +
        generate_rook_moves_white(board, moves) +
        generate_pawn_moves_white(board, moves) +
        generate_castle_white(board, moves);
}

int generate_moves_black(struct board_state *board,
        struct priority_queue *moves) {
    return generate_king_moves_black(board, moves) +
        generate_queen_moves_black(board, moves) +
        generate_bishop_moves_black(board, moves) +
        generate_knight_moves_black(board, moves) +
        generate_rook_moves_black(board, moves) +
        generate_pawn_moves_black(board, moves) +
        generate_castle_black(board, moves);
}
