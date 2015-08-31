#include <string.h>
#include "move.h"

/*
*    _________________________________________________________________________
*   | Move Type            | Primary   | Secondary        | Tertiary          |
*   |______________________|___________|__________________|___________________|
*   | Normal Move          | Mover     | -None-           | -None-            |
*   | Captures             | Captor    | -None-           | Captive           |
*   | Castling             | King      | Rook             | -None-            |
*   | En passant           | Captor    | -None            | Captive Pawn      |
*   | Promotion            | Pawn      | Promotion Piece  | -None-            |
*   | Promotion + Capture  | Pawn      | Promotion Piece  | Captive           |
*   |______________________|___________|__________________|___________________|
*   
**/

void assert_board_flags_equals(struct board_flags *a, struct board_flags *b) {
    assert_int_equal(a->castle_q[0], b->castle_q[0]);
    assert_int_equal(a->castle_q[1], b->castle_q[1]);
    assert_int_equal(a->castle_k[0], b->castle_k[0]);
    assert_int_equal(a->castle_k[1], b->castle_k[1]);
    assert_int_equal(a->en_passant[0], b->en_passant[0]);
    assert_int_equal(a->en_passant[1], b->en_passant[1]);
}
void assert_board_state_equals(struct board_state *a, struct board_state *b) {
    assert_int_equal(a->turn, b->turn);
    // assert_true(!memcmp(a->bb, b->bb, sizeof(a->bb)));
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 7; j++) {
            assert_int_equal(a->bb[i][j], b->bb[i][j]);
        }
    }
    assert_board_flags_equals(&a->flags, &b->flags);
}

void normal_move_test(void **state) {
    struct board_state pos = BOARD_INITIAL_STATE;
    struct board_state expected = {
        BLACK,
        {{
            BITBOARD_KING   & BITBOARD_WHITE,
            BITBOARD_QUEEN  & BITBOARD_WHITE,
            BITBOARD_BISHOP & BITBOARD_WHITE,
            BITBOARD_KNIGHT & BITBOARD_WHITE,
            BITBOARD_ROOK   & BITBOARD_WHITE,
            0x000000000010EF00ul,
            0x000000000010EFFFul
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
    struct move e2e3 = {
        PAWN, NO_PIECE, NO_PIECE,
        BITBOARD_E3 | BITBOARD_E2, 0, 0,
        {{1, 1}, {1, 1}, {0, 0}}
    };
    make(&pos, &e2e3);
    assert_board_state_equals(&pos, &expected);
}

void capture_move_test(void **state) {
    struct board_state pos = {
        WHITE,
        {{
            BITBOARD_KING   & BITBOARD_WHITE,
            BITBOARD_QUEEN  & BITBOARD_WHITE,
            BITBOARD_BISHOP & BITBOARD_WHITE,
            BITBOARD_KNIGHT & BITBOARD_WHITE,
            BITBOARD_ROOK   & BITBOARD_WHITE,
            0x000000001000EF00ul,
            0x000000001000EFFFul
        }, {
            BITBOARD_KING   & BITBOARD_BLACK,
            BITBOARD_QUEEN  & BITBOARD_BLACK,
            BITBOARD_BISHOP & BITBOARD_BLACK,
            BITBOARD_KNIGHT & BITBOARD_BLACK,
            BITBOARD_ROOK   & BITBOARD_BLACK,
            0x00F7000800000000ul,
            0xFFF7000800000000ul,
        }},
        {{1, 1}, {1, 1}, {0, 0}}
    };

    struct board_state expected = {
        BLACK,
        {{
            BITBOARD_KING   & BITBOARD_WHITE,
            BITBOARD_QUEEN  & BITBOARD_WHITE,
            BITBOARD_BISHOP & BITBOARD_WHITE,
            BITBOARD_KNIGHT & BITBOARD_WHITE,
            BITBOARD_ROOK   & BITBOARD_WHITE,
            0x000000080000EF00ul,
            0x000000080000EFFFul
        }, {
            BITBOARD_KING   & BITBOARD_BLACK,
            BITBOARD_QUEEN  & BITBOARD_BLACK,
            BITBOARD_BISHOP & BITBOARD_BLACK,
            BITBOARD_KNIGHT & BITBOARD_BLACK,
            BITBOARD_ROOK   & BITBOARD_BLACK,
            0x00F7000000000000ul,
            0xFFF7000000000000ul,
        }},
        {{1, 1}, {1, 1}, {0, 0}}
    };

    struct move e4d5 = {
        PAWN, NO_PIECE, PAWN,
        BITBOARD_E4 | BITBOARD_D5, 0, BITBOARD_D5,
        {{1, 1}, {1, 1}, {0, 0}}
    };
    make(&pos, &e4d5);
    assert_board_state_equals(&pos, &expected);
}

void promotion_capture_move_test(void **state) {
    struct board_state pos = {
        WHITE,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_G7,
            0x0040000000000010ul
        }, {
            BITBOARD_KING   & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            BITBOARD_H8,
            0ul,
            0x9000000800000000ul,
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };

    struct board_state expected = {
        BLACK,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            BITBOARD_H8,
            0ul,
            0ul,
            0ul,
            0ul,
            0x8000000000000010ul
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            0x1000000800000000ul,
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move g7h8 = {
        PAWN, QUEEN, ROOK,
        BITBOARD_G7, BITBOARD_H8, BITBOARD_H8,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    make(&pos, &g7h8);
    assert_board_state_equals(&pos, &expected);
}
