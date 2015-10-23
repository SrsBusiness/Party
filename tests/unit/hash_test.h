#include "hash.h"

void ZOBRIST_incremental_update_test(void **state) {
    struct board_state pos = BOARD_INITIAL_STATE;
    struct move m = {
        PAWN, NO_PIECE, NO_PIECE,
        BITBOARD_E2 | BITBOARD_E4, 0ul, 0ul, BITBOARD_E2,
        {{1, 1}, {1, 1}, {0, 0}}
    };
    uint64_t expected_hash;
    make(&pos, &m);
    expected_hash = ZOBRIST_hash(&pos);
    assert_int_equal(expected_hash, pos.hash);
}
