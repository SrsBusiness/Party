#include "bitboard.h"

/* lsb */
void lsb_test(void **state) {
    assert_int_equal(lsb(RANK5), 32);
}

/* msb */
void msb_test(void **state) {
    assert_int_equal(msb(RANK5), 39); 
}

/* fliph */
void fliph_test(void **state) {
    assert_int_equal(fliph(0x0123456789ABCDEFul),
            0x80C4A2E691D5B3F7ul);
}

/* flipv */
void flipv_test(void **state) {
    assert_int_equal(flipv(0x0123456789ABCDEFul),
            0xEFCDAB8967452301ul);
}

/* rolll */
void rolll_test(void **state) {
    assert_int_equal(rolll(0x0123456789ABCDEFul, 4),
            0x123456789ABCDEF0ul);
}

/* rorll */
void rorll_test(void **state) {
    assert_int_equal(rorll(0x0123456789ABCDEFul, 4),
            0xF0123456789ABCDEul);
}

/* north_one */
void north_one_test(void **state) {
    assert_int_equal(north_one(RANK1), RANK2);
    assert_int_equal(north_one(RANK8), 0ul);
}

/* south_one */
void south_one_test(void **state) {
    assert_int_equal(south_one(RANK2), RANK1);
    assert_int_equal(south_one(RANK1), 0ul);
}

/* east_one */
void east_one_test(void **state) {
    assert_int_equal(east_one(AFILE), BFILE);
    assert_int_equal(east_one(HFILE), 0ul);
}

/* west_one */
void west_one_test(void **state) {
    assert_int_equal(west_one(BFILE), AFILE);
    assert_int_equal(west_one(AFILE), 0ul);
}

/* ne_one */
void ne_one_test(void **state) {
    assert_int_equal(ne_one(1ul), 0x200ul);
    assert_int_equal(ne_one(A8H1), C8H3);
}

/* nw_one */
void nw_one_test(void **state) {
    assert_int_equal(nw_one(0x80ul), 0x4000ul);
    assert_int_equal(nw_one(A1H8), A3F8);
}

/* se_one */
void se_one_test(void **state) {
    assert_int_equal(se_one(0x4000ul), 0x80ul);
    assert_int_equal(se_one(A1H8), C1H6);
}

/* sw_one */
void sw_one_test(void **state) {
    assert_int_equal(sw_one(0x200ul), 1ul);
    assert_int_equal(sw_one(A8H1), A6F1);
}

/* flipld */
void flipld_test(void **state) {
    assert_int_equal(flipld(AFILE), RANK8);
    assert_int_equal(flipld(BFILE), RANK7);
}

/* flipdd */
void flipdd_test(void **state) {
    assert_int_equal(flipdd(AFILE), RANK1);
    assert_int_equal(flipdd(BFILE), RANK2);
}

/* rotate_pi */
void rotate_pi_test(void **state) {
    assert_int_equal(rotate_pi(BFILE | RANK6), GFILE | RANK3);
}

/* rotate_clockwise */
void rotate_clockwise_test(void **state) {
    assert_int_equal(rotate_clockwise(BFILE | RANK6), RANK7 | FFILE);
}

/* rotate_anticlockwise */
void rotate_anticlockwise_test(void **state) {
    assert_int_equal(rotate_anticlockwise(BFILE | RANK6), RANK2 | CFILE);
}

/* north_span */
void north_span_test(void **state) {
    assert_int_equal(north_span(1ul), AFILE & ~RANK1);
}

/* south_span */
void south_span_test(void **state) {
    assert_int_equal(south_span(0x0100000000000000), AFILE & ~RANK8);
}

/* east_span */
void east_span_test(void **state) {
    assert_int_equal(east_span(1ul), RANK1 & ~AFILE);
}

/* west_span */
void west_span_test(void **state) {
    assert_int_equal(west_span(0x80ul), RANK1 & ~HFILE);
}

/* ne_span*/
void ne_span_test(void **state) {
    assert_int_equal(ne_span(1ul), A1H8 ^ 1ul);
}

/* nw_span */
void nw_span_test(void **state) {
    assert_int_equal(nw_span(0x80ul), A8H1 ^ 0x80ul);
}

/* se_span */
void se_span_test(void **state) {
    assert_int_equal(se_span(0x0100000000000000ul), A8H1 & ~RANK8);
}

/* sw_span */
void sw_span_test(void **state) {
    assert_int_equal(sw_span(0x8000000000000000ul), A1H8 & ~RANK8);
}

/* north_fill */
void north_fill_test(void **state) {
    assert_int_equal(north_fill(1ul), AFILE);
}

/* south_fill */
void south_fill_test(void **state) {
    assert_int_equal(south_fill(0x0100000000000000ul), AFILE);
}

/* east_fill */
void east_fill_test(void **state) {
    assert_int_equal(east_fill(1ul), RANK1);
}

/* west_fill */
void west_fill_test(void **state) {
    assert_int_equal(west_fill(0x80ul), RANK1);
}

/* ne_fill */
void ne_fill_test(void **state) {
    assert_int_equal(ne_fill(1ul), A1H8);
}

/* nw_fill */
void nw_fill_test(void **state) {
    assert_int_equal(nw_fill(0x80ul), A8H1);
}

/* se_fill */
void se_fill_test(void **state) {
    assert_int_equal(se_fill(0x0100000000000000ul), A8H1);
}

/* sw_fill */
void sw_fill_test(void **state) {
    assert_int_equal(sw_fill(0x8000000000000000ul), A1H8);
}

/* file_fill */
void file_fill_test(void **state) {
    assert_int_equal(file_fill(2ul), BFILE);
}

/* south_occluded_fill */
void south_occluded_fill_test(void **state) {
    assert_int_equal(south_occluded_fill(0x0000002400000000ul,
                0x00FFFFFFFFFFFF00ul), 0x0000002424242400ul);
}

/* north_occluded_fill */
void north_occluded_fill_test(void **state) {
    assert_int_equal(north_occluded_fill(0x0000002400000000ul,
                0x00FFFFFFFFFFFF00ul), 0x0024242400000000ul);
}

/* east_occluded_fill */
void east_occluded_fill_test(void **state) {
    assert_int_equal(east_occluded_fill(0x0001020400000000ul,
                0x00FFFFFFFFFFFF00ul), 0x00FFFEFC00000000ul);
}

/* ne_occluded_fill */
void ne_occluded_fill_test(void **state) {
    assert_int_equal(ne_occluded_fill(0x0000000010040100ul,
                0x00FFFFFFFFFFFF00ul), 0x00E070381C060100ul);
}

/* se_occluded_fill */
void se_occluded_fill_test(void **state) {
    assert_int_equal(se_occluded_fill(0x0001041000000000ul,
                0x00FFFFFFFFFFFF00ul), 0x0001061C3870E000ul);
}

/* west_occluded_fill */
void west_occluded_fill_test(void **state) {
    assert_int_equal(west_occluded_fill(0x0000000080402000ul,
                0x00FEFEFEFEFEFE00ul), 0x00000000FE7E3E00ul);
}

/* sw_occluded_fill */
void sw_occluded_fill_test(void **state) {
    assert_int_equal(sw_occluded_fill(0x0080200800000000ul,
                0x00FFFFFFFFFFFF00ul), 0x008060381C0E0700ul);
}

/* nw_occluded_fill */
void nw_occluded_fill_test(void **state) {
    assert_int_equal(nw_occluded_fill(0x0000000008208000ul,
                0x00FFFFFFFFFFFF00ul), 0x00070E1C38608000ul);
}
