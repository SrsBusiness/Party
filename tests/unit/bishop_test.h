#include "bishop.h"

#define expected_bishop_attacks(x, y) ((ne_occluded_fill((x), ~ne_one(y)) | \
                se_occluded_fill((x), ~se_one(y)) | \
                sw_occluded_fill((x), ~sw_one(y)) | \
                nw_occluded_fill((x), ~nw_one(y))) ^ (x))

/* exhaustive magic bitboard test */
void exhaustive_bishop_magic_test(void **state) {
    int i;
    for (i = 0; i < 64; i++) {
        const magic_entry *m = &bishop_magic_table[i];
        bboard occupancy = m->mask, n = 0;
        bboard bishop = 1ul << i;
        do {
            assert_int_equal(expected_bishop_attacks(bishop, n),
                    bishop_attacks(bishop, n));
            n = (n - occupancy) & occupancy;
        } while (n);
    }
}

/* bishop attacks */
void bishop_attacks_test(void **state) {
    /* Squares outside of relevant occupancy should be inconsequential */
    assert_int_equal(0x0000442800284400ul,
            bishop_attacks(0x0000000010000000ul, 0x0000441111114400ul));
}

