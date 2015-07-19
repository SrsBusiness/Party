#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "bitboard_test.h"
#include "pawn_test.h"

int main() {
    const struct CMUnitTest tests[] = {
        /* bitboard tests */
        cmocka_unit_test(lsb_test),
        cmocka_unit_test(msb_test),
        cmocka_unit_test(fliph_test),
        cmocka_unit_test(flipv_test),
        cmocka_unit_test(rolll_test),
        cmocka_unit_test(rorll_test),
        cmocka_unit_test(north_one_test),
        cmocka_unit_test(south_one_test),
        cmocka_unit_test(east_one_test),
        cmocka_unit_test(west_one_test),
        cmocka_unit_test(ne_one_test),
        cmocka_unit_test(nw_one_test),
        cmocka_unit_test(se_one_test),
        cmocka_unit_test(sw_one_test),
        cmocka_unit_test(flipld_test),
        cmocka_unit_test(flipdd_test),
        cmocka_unit_test(rotate_pi_test),
        cmocka_unit_test(rotate_clockwise_test),
        cmocka_unit_test(rotate_anticlockwise_test),
        cmocka_unit_test(north_span_test),
        cmocka_unit_test(south_span_test),
        cmocka_unit_test(east_span_test),
        cmocka_unit_test(west_span_test),
        cmocka_unit_test(ne_span_test),
        cmocka_unit_test(nw_span_test),
        cmocka_unit_test(se_span_test),
        cmocka_unit_test(sw_span_test),
        cmocka_unit_test(north_fill_test),
        cmocka_unit_test(south_fill_test),
        cmocka_unit_test(east_fill_test),
        cmocka_unit_test(west_fill_test),
        cmocka_unit_test(ne_fill_test),
        cmocka_unit_test(nw_fill_test),
        cmocka_unit_test(se_fill_test),
        cmocka_unit_test(sw_fill_test),
        cmocka_unit_test(file_fill_test),
        cmocka_unit_test(south_occluded_fill_test),
        cmocka_unit_test(north_occluded_fill_test),
        cmocka_unit_test(east_occluded_fill_test),
        cmocka_unit_test(ne_occluded_fill_test),
        cmocka_unit_test(se_occluded_fill_test),
        cmocka_unit_test(west_occluded_fill_test),
        cmocka_unit_test(sw_occluded_fill_test),
        cmocka_unit_test(nw_occluded_fill_test),

        /* pawn tests */
        cmocka_unit_test(w_single_push_targets_test),
        cmocka_unit_test(w_double_push_targets_test),
        cmocka_unit_test(b_single_push_targets_test),
        cmocka_unit_test(b_double_push_targets_test),
        cmocka_unit_test(single_push_targets_test),
        cmocka_unit_test(double_push_targets_test),
        cmocka_unit_test(w_pawns_able_to_push_test),
        cmocka_unit_test(w_pawns_able_to_double_push_test),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
