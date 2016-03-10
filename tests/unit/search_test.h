#include "search.h"

void search_check_test(void **state) {
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
    struct move best_move;
    int status = search(&check, 4, &best_move);
    assert_int_equal(NORMAL_MOVE, status);
    struct priority_queue pq;
    priority_queue_init(&pq, 16);
    int move_count = generate_moves_black(&check, &pq);
    assert_int_equal(1, move_count);
    assert_int_equal(1, pq.length);
    struct move *m = priority_queue_pop(&pq);
    test_free(m);
    priority_queue_destroy(&pq);
}
