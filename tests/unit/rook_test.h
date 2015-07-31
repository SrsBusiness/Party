#include "rook.h"

#define expected_rook_attacks(x, y) ((north_occluded_fill((x), ~north_one(y)) | \
                south_occluded_fill((x), ~south_one(y)) | \
                east_occluded_fill((x), ~east_one(y)) | \
                west_occluded_fill((x), ~west_one(y))) ^ (x))

/* exhaustive magic bitboard test */
void exhaustive_rook_magic_test(void **state) {
    int i;
    for (i = 0; i < 64; i++) {
        const magic_entry *m = &rook_magic_table[i];
        bboard occupancy = m->mask, n = 0;
        bboard rook = 1ul << i;
        do {
            assert_int_equal(expected_rook_attacks(rook, n),
                    rook_attacks(rook, n));
            n = (n - occupancy) & occupancy;
        } while (n);
    }
}

/* rook attacks */
void rook_attacks_test(void **state) {
    /* Squares outside of relevant occupancy should be inconsequential */
    assert_int_equal(0x000010106C101000ul,
            rook_attacks(0x0000000010000000ul, 0x8181918144819181ul));
}
