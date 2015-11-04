#include <string.h>
#include "move.h"
#include "bboard_utils.h"

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
*   TODO: Test static castle flag modification in capture generation functions
**/

void assert_board_flags_equal(struct board_flags *a, struct board_flags *b) {
    assert_int_equal(a->castle_q[0], b->castle_q[0]);
    assert_int_equal(a->castle_q[1], b->castle_q[1]);
    assert_int_equal(a->castle_k[0], b->castle_k[0]);
    assert_int_equal(a->castle_k[1], b->castle_k[1]);
    assert_int_equal(a->en_passant[0], b->en_passant[0]);
    assert_int_equal(a->en_passant[1], b->en_passant[1]);
}

void assert_board_state_equal(struct board_state *a, struct board_state *b) {
    assert_int_equal(a->turn, b->turn);
    // assert_true(!memcmp(a->bb, b->bb, sizeof(a->bb)));
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 7; j++) {
            assert_int_equal(a->bb[i][j], b->bb[i][j]);
        }
    }
    assert_board_flags_equal(&a->flags, &b->flags);
}

void assert_move_equal(struct move *a, struct move *b) {
    assert_int_equal(a->p_mover, b->p_mover);
    assert_int_equal(a->s_mover, b->s_mover);
    assert_int_equal(a->t_mover, b->t_mover);
    assert_int_equal(a->primary, b->primary);
    assert_int_equal(a->secondary, b->secondary);
    assert_int_equal(a->tertiary, b->tertiary);
    assert_int_equal(a->primary_src, b->primary_src);
    assert_board_flags_equal(&a->flags, &b->flags);
}

/* 1.e2-e3 */
void normal_move_test(void **state) {
    struct board_state pos = BOARD_INITIAL_STATE;
    struct board_state expected = {
        BLACK,
        0,
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
        BITBOARD_E3 | BITBOARD_E2, 0, 0, BITBOARD_E2,
        {{1, 1}, {1, 1}, {0, 0}}
    };
    make(&pos, &e2e3);
    assert_board_state_equal(&expected, &pos);
}

/* 1. e2-e4 d7-d5 2. e4-d5 */
void capture_move_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
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
        0,
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
        BITBOARD_E4 | BITBOARD_D5, 0, BITBOARD_D5, BITBOARD_E4,
        {{1, 1}, {1, 1}, {0, 0}}
    };
    make(&pos, &e4d5);
    assert_board_state_equal(&expected, &pos);
}

void promotion_capture_move_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_G7,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_G7
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            BITBOARD_H8,
            0ul,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_H8
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };

    struct board_state expected = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            BITBOARD_H8,
            0ul,
            0ul,
            0ul,
            0ul,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_H8
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_BLACK,
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move g7h8 = {
        PAWN, QUEEN, ROOK,
        BITBOARD_G7, BITBOARD_H8, BITBOARD_H8, BITBOARD_G7,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    make(&pos, &g7h8);
    assert_board_state_equal(&expected, &pos);
}

void in_check_test(void **state) {
    struct board_state check = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            BITBOARD_H8,
            0ul,
            0ul,
            0ul,
            0ul,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_H8
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_BLACK,
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct board_state safe = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            BITBOARD_H7,
            0ul,
            0ul,
            0ul,
            0ul,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_H7
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_BLACK,
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    assert_int_equal(1, in_check(&check, BLACK));
    assert_int_equal(0, in_check(&safe, BLACK));
}

void rook_check_test(void **state) {
    struct board_state check = {
        BLACK,
        0,
        {{
            BITBOARD_G1,
            0ul,
            0ul,
            BITBOARD_E2,
            BITBOARD_F7,
            BITBOARD_H3 | BITBOARD_G4 | BITBOARD_D5 | BITBOARD_C4 | BITBOARD_A4,
            BITBOARD_G1 | BITBOARD_E2 | BITBOARD_F7 | BITBOARD_H3 | BITBOARD_G4 | BITBOARD_D5 | BITBOARD_C4 | BITBOARD_A4,
        }, {
            BITBOARD_D7,
            0ul,
            0ul,
            0ul,
            BITBOARD_H8 | BITBOARD_B8,
            BITBOARD_H7 | BITBOARD_A7,
            BITBOARD_D7 | BITBOARD_H8 | BITBOARD_B8 | BITBOARD_H7 | BITBOARD_A7,
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    assert_int_equal(1, in_check(&check, BLACK));
    //serialize_board(&check);
}

void multiple_check_test(void **state) {
    struct board_state check = {
        BLACK,
        0,
        {{
            BITBOARD_F1,
            BITBOARD_H5,
            BITBOARD_D3,
            0ul,
            0ul,
            BITBOARD_A2 | BITBOARD_D2 | BITBOARD_D4 | BITBOARD_F2 | BITBOARD_G3,
            BITBOARD_F1 | BITBOARD_H5 | BITBOARD_D3 | BITBOARD_A2 | BITBOARD_D2 | BITBOARD_D4 | BITBOARD_F2 | BITBOARD_G3,
        }, {
            BITBOARD_H3,
            BITBOARD_B4,
            BITBOARD_F6,
            0ul,
            BITBOARD_A8,
            BITBOARD_A7 | BITBOARD_B7 | BITBOARD_C7 | BITBOARD_E7,
            BITBOARD_H3 | BITBOARD_B4 | BITBOARD_F6 | BITBOARD_A8 | BITBOARD_A7 | BITBOARD_B7 | BITBOARD_C7 | BITBOARD_E7,
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    assert_int_equal(1, in_check(&check, BLACK));
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_moves_black(&check, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move *m = priority_queue_pop(&pq);
    test_free(m);
    priority_queue_destroy(&pq);
    //serialize_board(&check);
}

void generate_pawn_single_pushes_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E2,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E2
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_BLACK
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_single_pushes_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, NO_PIECE,
        BITBOARD_E2 | BITBOARD_E3, 0, 0, BITBOARD_E2,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

/* TODO: mock out zobrist hash function */
void generate_pawn_push_promotions_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_A7,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_A7
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_BLACK
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };

    struct move expected_moves[4] = {
        {
            PAWN, QUEEN, NO_PIECE,
            BITBOARD_A7, BITBOARD_A8, 0, BITBOARD_A7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, BISHOP, NO_PIECE,
            BITBOARD_A7, BITBOARD_A8, 0, BITBOARD_A7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, KNIGHT, NO_PIECE,
            BITBOARD_A7, BITBOARD_A8, 0, BITBOARD_A7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, ROOK, NO_PIECE,
            BITBOARD_A7, BITBOARD_A8, 0, BITBOARD_A7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
    };

    struct move *expected_moves_ptrs[4] = {
        &expected_moves[0], &expected_moves[1], &expected_moves[2], &expected_moves[3]
    };

    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_push_promotions_white(&pos, &pq);
    assert_int_equal(4, move_count);
    assert_int_equal(4, pq.length);

    /* Check each move for equality with expected moves */
    for (int i = 0; i < 4; i++) {
        struct move *single_push = priority_queue_pop(&pq);
        int piece = single_push->s_mover;
        struct move *expected_move = expected_moves_ptrs[piece - QUEEN];
        assert_move_equal(expected_move, single_push);
        test_free(single_push);
        expected_moves_ptrs[piece - QUEEN] = NULL;
    }

    /* Check that each possible promotion was generated */
    for (int i = 0; i < 4; i++) {
        assert_ptr_equal(NULL, expected_moves_ptrs[i]);
    }
    priority_queue_destroy(&pq);
}

void generate_pawn_double_pushes_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E2,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E2
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_BLACK
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_double_pushes_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, NO_PIECE,
        BITBOARD_E2 | BITBOARD_E4, 0, 0, BITBOARD_E2,
        {{0, 0}, {0, 0}, {FILE_SET_EFILE, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_pawn_east_captures_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E4,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E4
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_F5,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_F5
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_east_captures_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, PAWN,
        BITBOARD_E4 | BITBOARD_F5, 0, BITBOARD_F5, BITBOARD_E4,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_pawn_west_captures_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E4,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E4
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_D5,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_D5
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_west_captures_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, PAWN,
        BITBOARD_E4 | BITBOARD_D5, 0, BITBOARD_D5, BITBOARD_E4,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void
generate_pawn_east_captures_promotions_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_A7,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_A7
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            BITBOARD_B8,
            0ul,
            0ul,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_B8
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };

    struct move expected_moves[4] = {
        {
            PAWN, QUEEN, KNIGHT,
            BITBOARD_A7, BITBOARD_B8, BITBOARD_B8, BITBOARD_A7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, BISHOP, KNIGHT,
            BITBOARD_A7, BITBOARD_B8, BITBOARD_B8, BITBOARD_A7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, KNIGHT, KNIGHT,
            BITBOARD_A7, BITBOARD_B8, BITBOARD_B8, BITBOARD_A7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, ROOK, KNIGHT,
            BITBOARD_A7, BITBOARD_B8, BITBOARD_B8, BITBOARD_A7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
    };

    struct move *expected_moves_ptrs[4] = {
        &expected_moves[0], &expected_moves[1], &expected_moves[2], &expected_moves[3]
    };

    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_east_captures_promotions_white(&pos, &pq);
    assert_int_equal(4, move_count);
    assert_int_equal(4, pq.length);

    /* Check each move for equality with expected moves */
    for (int i = 0; i < 4; i++) {
        struct move *single_push = priority_queue_pop(&pq);
        int piece = single_push->s_mover;
        struct move *expected_move = expected_moves_ptrs[piece - QUEEN];
        assert_move_equal(expected_move, single_push);
        test_free(single_push);
        expected_moves_ptrs[piece - QUEEN] = NULL;
    }

    /* Check that each possible promotion was generated */
    for (int i = 0; i < 4; i++) {
        assert_ptr_equal(NULL, expected_moves_ptrs[i]);
    }
    priority_queue_destroy(&pq);
}

void
generate_pawn_west_captures_promotions_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_C7,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_C7
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            BITBOARD_B8,
            0ul,
            0ul,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_B8
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };

    struct move expected_moves[4] = {
        {
            PAWN, QUEEN, KNIGHT,
            BITBOARD_C7, BITBOARD_B8, BITBOARD_B8, BITBOARD_C7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, BISHOP, KNIGHT,
            BITBOARD_C7, BITBOARD_B8, BITBOARD_B8, BITBOARD_C7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, KNIGHT, KNIGHT,
            BITBOARD_C7, BITBOARD_B8, BITBOARD_B8, BITBOARD_C7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, ROOK, KNIGHT,
            BITBOARD_C7, BITBOARD_B8, BITBOARD_B8, BITBOARD_C7,
            {{0, 0}, {0, 0}, {0, 0}}
        },
    };

    struct move *expected_moves_ptrs[4] = {
        &expected_moves[0], &expected_moves[1], &expected_moves[2], &expected_moves[3]
    };

    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_west_captures_promotions_white(&pos, &pq);
    assert_int_equal(4, move_count);
    assert_int_equal(4, pq.length);

    /* Check each move for equality with expected moves */
    for (int i = 0; i < 4; i++) {
        struct move *single_push = priority_queue_pop(&pq);
        int piece = single_push->s_mover;
        struct move *expected_move = expected_moves_ptrs[piece - QUEEN];
        assert_move_equal(expected_move, single_push);
        test_free(single_push);
        expected_moves_ptrs[piece - QUEEN] = NULL;
    }

    /* Check that each possible promotion was generated */
    for (int i = 0; i < 4; i++) {
        assert_ptr_equal(NULL, expected_moves_ptrs[i]);
    }
    priority_queue_destroy(&pq);
}

void generate_pawn_en_passant_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E5,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E5
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_D5,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_D5
        }},
        {{0, 0}, {0, 0}, {0, FILE_SET_DFILE}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_en_passant_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, PAWN,
        BITBOARD_E5 | BITBOARD_D6, 0, BITBOARD_D5, BITBOARD_E5,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

/* Black */
void generate_pawn_single_pushes_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E2,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E2
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E7,
            BITBOARD_KING & BITBOARD_BLACK
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_single_pushes_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, NO_PIECE,
        BITBOARD_E7 | BITBOARD_E6, 0, 0, BITBOARD_E7,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_pawn_push_promotions_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_WHITE
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_A2,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_A2
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };

    struct move expected_moves[4] = {
        {
            PAWN, QUEEN, NO_PIECE,
            BITBOARD_A2, BITBOARD_A1, 0, BITBOARD_A2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, BISHOP, NO_PIECE,
            BITBOARD_A2, BITBOARD_A1, 0, BITBOARD_A2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, KNIGHT, NO_PIECE,
            BITBOARD_A2, BITBOARD_A1, 0, BITBOARD_A2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, ROOK, NO_PIECE,
            BITBOARD_A2, BITBOARD_A1, 0, BITBOARD_A2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
    };

    struct move *expected_moves_ptrs[4] = {
        &expected_moves[0], &expected_moves[1], &expected_moves[2], &expected_moves[3]
    };

    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_push_promotions_black(&pos, &pq);
    assert_int_equal(4, move_count);
    assert_int_equal(4, pq.length);

    /* Check each move for equality with expected moves */
    for (int i = 0; i < 4; i++) {
        struct move *single_push = priority_queue_pop(&pq);
        int piece = single_push->s_mover;
        struct move *expected_move = expected_moves_ptrs[piece - QUEEN];
        assert_move_equal(expected_move, single_push);
        test_free(single_push);
        expected_moves_ptrs[piece - QUEEN] = NULL;
    }

    /* Check that each possible promotion was generated */
    for (int i = 0; i < 4; i++) {
        assert_ptr_equal(NULL, expected_moves_ptrs[i]);
    }
    priority_queue_destroy(&pq);
}

void generate_pawn_double_pushes_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_WHITE
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E7,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_E7
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_double_pushes_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, NO_PIECE,
        BITBOARD_E7 | BITBOARD_E5, 0, 0, BITBOARD_E7,
        {{0, 0}, {0, 0}, {0, FILE_SET_EFILE}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_pawn_east_captures_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E4,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E4
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_D5,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_D5
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_east_captures_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, PAWN,
        BITBOARD_D5 | BITBOARD_E4, 0, BITBOARD_E4, BITBOARD_D5,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_pawn_west_captures_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E4,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E4
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_F5,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_F5
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_west_captures_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, PAWN,
        BITBOARD_F5 | BITBOARD_E4, 0, BITBOARD_E4, BITBOARD_F5,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void
generate_pawn_east_captures_promotions_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            BITBOARD_B1,
            0ul,
            0ul,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_B1
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_A2,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_A2
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };

    struct move expected_moves[4] = {
        {
            PAWN, QUEEN, KNIGHT,
            BITBOARD_A2, BITBOARD_B1, BITBOARD_B1, BITBOARD_A2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, BISHOP, KNIGHT,
            BITBOARD_A2, BITBOARD_B1, BITBOARD_B1, BITBOARD_A2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, KNIGHT, KNIGHT,
            BITBOARD_A2, BITBOARD_B1, BITBOARD_B1, BITBOARD_A2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, ROOK, KNIGHT,
            BITBOARD_A2, BITBOARD_B1, BITBOARD_B1, BITBOARD_A2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
    };

    struct move *expected_moves_ptrs[4] = {
        &expected_moves[0], &expected_moves[1], &expected_moves[2], &expected_moves[3]
    };

    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_east_captures_promotions_black(&pos, &pq);
    assert_int_equal(4, move_count);
    assert_int_equal(4, pq.length);

    /* Check each move for equality with expected moves */
    for (int i = 0; i < 4; i++) {
        struct move *single_push = priority_queue_pop(&pq);
        int piece = single_push->s_mover;
        struct move *expected_move = expected_moves_ptrs[piece - QUEEN];
        assert_move_equal(expected_move, single_push);
        test_free(single_push);
        expected_moves_ptrs[piece - QUEEN] = NULL;
    }

    /* Check that each possible promotion was generated */
    for (int i = 0; i < 4; i++) {
        assert_ptr_equal(NULL, expected_moves_ptrs[i]);
    }
    priority_queue_destroy(&pq);
}

void
generate_pawn_west_captures_promotions_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            BITBOARD_B1,
            0ul,
            0ul,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_B1
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_C2,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_C2
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };

    struct move expected_moves[4] = {
        {
            PAWN, QUEEN, KNIGHT,
            BITBOARD_C2, BITBOARD_B1, BITBOARD_B1, BITBOARD_C2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, BISHOP, KNIGHT,
            BITBOARD_C2, BITBOARD_B1, BITBOARD_B1, BITBOARD_C2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, KNIGHT, KNIGHT,
            BITBOARD_C2, BITBOARD_B1, BITBOARD_B1, BITBOARD_C2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
        {
            PAWN, ROOK, KNIGHT,
            BITBOARD_C2, BITBOARD_B1, BITBOARD_B1, BITBOARD_C2,
            {{0, 0}, {0, 0}, {0, 0}}
        },
    };

    struct move *expected_moves_ptrs[4] = {
        &expected_moves[0], &expected_moves[1], &expected_moves[2], &expected_moves[3]
    };

    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_west_captures_promotions_black(&pos, &pq);
    assert_int_equal(4, move_count);
    assert_int_equal(4, pq.length);

    /* Check each move for equality with expected moves */
    for (int i = 0; i < 4; i++) {
        struct move *single_push = priority_queue_pop(&pq);
        int piece = single_push->s_mover;
        struct move *expected_move = expected_moves_ptrs[piece - QUEEN];
        assert_move_equal(expected_move, single_push);
        test_free(single_push);
        expected_moves_ptrs[piece - QUEEN] = NULL;
    }

    /* Check that each possible promotion was generated */
    for (int i = 0; i < 4; i++) {
        assert_ptr_equal(NULL, expected_moves_ptrs[i]);
    }
    priority_queue_destroy(&pq);
}

void generate_pawn_en_passant_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_D4,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_D4
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_E4,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_E4
        }},
        {{0, 0}, {0, 0}, {FILE_SET_DFILE, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_pawn_en_passant_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        PAWN, NO_PIECE, PAWN,
        BITBOARD_E4 | BITBOARD_D3, 0, BITBOARD_D4, BITBOARD_E4,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_bishop_moves_obstruction_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            BITBOARD_E4,
            0ul,
            0ul,
            BITBOARD_D3 | BITBOARD_D5 | BITBOARD_F3 | BITBOARD_G6,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E4 | BITBOARD_D3 | 
                BITBOARD_D5 | BITBOARD_F3 | BITBOARD_G6
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_BLACK
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_bishop_moves_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        BISHOP, NO_PIECE, NO_PIECE,
        BITBOARD_E4 | BITBOARD_F5, 0, 0, BITBOARD_E4,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_bishop_moves_capture_white_test(void **state) {
     struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            BITBOARD_G7,
            BITBOARD_F8,
            0ul,
            BITBOARD_F6 | BITBOARD_H6,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_G7 | BITBOARD_F8 |
                BITBOARD_F6 | BITBOARD_H6
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            BITBOARD_H8,
            0ul,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_H8
        }},
        {{0, 0}, {0, 1}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_bishop_moves_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        BISHOP, NO_PIECE, ROOK,
        BITBOARD_G7 | BITBOARD_H8, 0, BITBOARD_H8, BITBOARD_G7,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_bishop_moves_obstruction_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_WHITE
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            BITBOARD_D4,
            0ul,
            0ul,
            BITBOARD_C3 | BITBOARD_C5 | BITBOARD_E3 | BITBOARD_F6,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_D4 | BITBOARD_C3 |
                BITBOARD_C5 | BITBOARD_E3 | BITBOARD_F6
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_bishop_moves_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        BISHOP, NO_PIECE, NO_PIECE,
        BITBOARD_D4 | BITBOARD_E5, 0, 0, BITBOARD_D4,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_bishop_moves_capture_black_test(void **state) {
     struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            BITBOARD_H1,
            0ul,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_H1
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            BITBOARD_G2,
            BITBOARD_F1,
            0ul,
            BITBOARD_F3 | BITBOARD_H3,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_G2 | BITBOARD_F1 |
                BITBOARD_F3 | BITBOARD_H3
        }},
        {{0, 0}, {1, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_bishop_moves_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        BISHOP, NO_PIECE, ROOK,
        BITBOARD_G2 | BITBOARD_H1, 0, BITBOARD_H1, BITBOARD_G2,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_rook_moves_obstruction_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            BITBOARD_E4,
            BITBOARD_D4 | BITBOARD_E3 | BITBOARD_F4 | BITBOARD_E6,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_E4 | BITBOARD_D4 |
                BITBOARD_E3 | BITBOARD_F4 | BITBOARD_E6
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_BLACK
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_rook_moves_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        ROOK, NO_PIECE, NO_PIECE,
        BITBOARD_E4 | BITBOARD_E5, 0, 0, BITBOARD_E4,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_rook_moves_capture_white_test(void **state) {
     struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            BITBOARD_H7,
            BITBOARD_G7 | BITBOARD_H6,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_H7 | BITBOARD_G7 |
                BITBOARD_H6
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            BITBOARD_H8,
            0ul,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_H8
        }},
        {{0, 0}, {0, 1}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_rook_moves_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        ROOK, NO_PIECE, ROOK,
        BITBOARD_H7 | BITBOARD_H8, 0, BITBOARD_H8, BITBOARD_H7,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

/* TODO: Modify below */

void generate_rook_moves_obstruction_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_WHITE
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            BITBOARD_D4,
            BITBOARD_C4 | BITBOARD_D3 | BITBOARD_E4 | BITBOARD_D6,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_D4 | BITBOARD_C4 |
                BITBOARD_D3 | BITBOARD_E4 | BITBOARD_D6
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_rook_moves_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        ROOK, NO_PIECE, NO_PIECE,
        BITBOARD_D4 | BITBOARD_D5, 0, 0, BITBOARD_D4,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_rook_moves_capture_black_test(void **state) {
     struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            BITBOARD_H1,
            0ul,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_H1
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            BITBOARD_H2,
            BITBOARD_G2 | BITBOARD_H3,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_H2 | BITBOARD_G2 |
                BITBOARD_H3
        }},
        {{0, 0}, {1, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_rook_moves_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        ROOK, NO_PIECE, ROOK,
        BITBOARD_H2 | BITBOARD_H1, 0, BITBOARD_H1, BITBOARD_H2,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

/* TODO: test queen move generation */

void generate_queen_moves_obstruction_white_test(void **state) {
    struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            BITBOARD_E4,
            0ul,
            0ul,
            0ul,
            BITBOARD_D3 | BITBOARD_D4 | BITBOARD_D5 | BITBOARD_E3 |
                BITBOARD_F3 | BITBOARD_F4 | BITBOARD_E5 | BITBOARD_G6,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_D3 | BITBOARD_D4 |
                BITBOARD_D5 | BITBOARD_E3 | BITBOARD_F3 | BITBOARD_F4 | 
                BITBOARD_E5 | BITBOARD_G6 | BITBOARD_E4
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_BLACK
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_queen_moves_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        QUEEN, NO_PIECE, NO_PIECE,
        BITBOARD_E4 | BITBOARD_F5, 0, 0, BITBOARD_E4,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_queen_moves_capture_white_test(void **state) {
     struct board_state pos = {
        WHITE,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            BITBOARD_G7,
            0ul,
            BITBOARD_F8 | BITBOARD_G8,
            0ul,
            BITBOARD_F6 | BITBOARD_F7 | BITBOARD_G6 | BITBOARD_H6 |
                BITBOARD_H7,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_F6 | BITBOARD_F7 |
                BITBOARD_G6 | BITBOARD_H6 | BITBOARD_H7 | BITBOARD_F8 |
                BITBOARD_G8 | BITBOARD_G7

        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            0ul,
            0ul,
            0ul,
            BITBOARD_H8,
            0ul,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_H8
        }},
        {{0, 0}, {0, 1}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_queen_moves_white(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        QUEEN, NO_PIECE, ROOK,
        BITBOARD_G7 | BITBOARD_H8, 0, BITBOARD_H8, BITBOARD_G7,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_queen_moves_obstruction_black_test(void **state) {
    struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            0ul,
            0ul,
            BITBOARD_KING & BITBOARD_WHITE
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            BITBOARD_D4,
            0ul,
            0ul,
            0ul,
            BITBOARD_C3 | BITBOARD_C4 | BITBOARD_C5 | BITBOARD_D3 |
                BITBOARD_E3 | BITBOARD_E4 | BITBOARD_D5 | BITBOARD_F6,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_C3 | BITBOARD_C4 |
                BITBOARD_C5 | BITBOARD_D3 | BITBOARD_E3 | BITBOARD_E4 |
                BITBOARD_D5 | BITBOARD_F6 | BITBOARD_D4
        }},
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_queen_moves_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        QUEEN, NO_PIECE, NO_PIECE,
        BITBOARD_D4 | BITBOARD_E5, 0, 0, BITBOARD_D4,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

void generate_queen_moves_capture_black_test(void **state) {
     struct board_state pos = {
        BLACK,
        0,
        {{
            BITBOARD_KING & BITBOARD_WHITE,
            0ul,
            0ul,
            0ul,
            BITBOARD_H1,
            0ul,
            (BITBOARD_KING & BITBOARD_WHITE) | BITBOARD_H1
        }, {
            BITBOARD_KING & BITBOARD_BLACK,
            BITBOARD_G2,
            0ul,
            BITBOARD_F1 | BITBOARD_G1,
            0ul,
            BITBOARD_F3 | BITBOARD_G3 | BITBOARD_H3 | BITBOARD_F2 |
                BITBOARD_H2,
            (BITBOARD_KING & BITBOARD_BLACK) | BITBOARD_G2 | BITBOARD_F1 |
                BITBOARD_G1 | BITBOARD_F3 | BITBOARD_G3 | BITBOARD_H3 |
                BITBOARD_F2 | BITBOARD_H2
        }},
        {{0, 0}, {1, 0}, {0, 0}}
    };
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_queen_moves_black(&pos, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move expected_move = {
        QUEEN, NO_PIECE, ROOK,
        BITBOARD_G2 | BITBOARD_H1, 0, BITBOARD_H1, BITBOARD_G2,
        {{0, 0}, {0, 0}, {0, 0}}
    };
    struct move *single_push = priority_queue_pop(&pq);
    assert_move_equal(&expected_move, single_push);
    test_free(single_push);
    priority_queue_destroy(&pq);
}

/* TODO: test knight move generation */
/* TODO: test king move generation */
