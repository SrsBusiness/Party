#include "pawn.h"

/* w_single_push_targets */
void w_single_push_targets_test(void **state) {
    assert_int_equal(0x0000001022810000ul,
            w_single_push_targets(0x000000001422C100ul,
                0x0000001222810000ul));
}
//bboard w_dbl_push_targets(bboard, bboard);
void w_dbl_push_targets_test(void **state) {
    /* Pawns can't jump over pieces */
    assert_int_equal(0x0000000000000000ul,
            w_dbl_push_targets(0x000000001422C100ul,
                0x0000142281000000ul));
    assert_int_equal(0x0000000080000000ul,
            w_dbl_push_targets(0x000000001422C100ul,
                0x0000142281800000ul));
}

//bboard b_single_push_targets(bboard, bboard);
void b_single_push_targets_test(void **state) {
    //assert_int_equal();
}
//bboard b_dbl_push_targets(bboard, bboard);
//bboard single_push_targets(bboard, bboard, int);
//bboard double_push_targets(bboard, bboard, int);
//
//bboard w_pawns_able_2_push(bboard, bboard);
//bboard w_pawns_able_2_dbl_push(bboard, bboard);
//bboard b_pawns_able_2_push(bboard, bboard);
//bboard b_pawns_able_2_dbl_push(bboard, bboard);
//bboard pawns_able_2_push(bboard, bboard, int);
//bboard pawns_able_2_dbl_push(bboard, bboard, int);
//
//bboard w_east_lever(bboard, bboard);
//bboard w_west_lever(bboard, bboard);
//bboard w_any_lever(bboard, bboard);
//bboard b_east_lever(bboard, bboard);
//bboard b_west_lever(bboard, bboard);
//bboard b_any_lever(bboard, bboard);
//
//bboard w_safe_pawn_squares(bboard, bboard);
//bboard b_safe_pawn_squares(bboard, bboard);
//
//bboard w_pawns_able_2_capture_east(bboard, bboard);
//bboard w_pawns_able_2_capture_west(bboard, bboard);
//bboard w_pawns_able_2_capture_any(bboard, bboard);
//bboard b_pawns_able_2_capture_east(bboard, bboard);
//bboard b_pawns_able_2_capture_west(bboard, bboard);
//bboard b_pawns_able_2_capture_any(bboard, bboard);
//bboard pawns_able_2_capture_east(bboard, bboard, int);
//bboard pawns_able_2_capture_west(bboard, bboard, int);
//bboard pawns_able_2_capture_any(bboard, bboard, int);
//
//bboard pawn_capture_targets(bboard, bboard, int);
//
//bboard w_pawn_defended_from_west(bboard);
//bboard w_pawn_defended_from_east(bboard);
//bboard b_pawn_defended_from_west(bboard);
//bboard b_pawn_defended_from_east(bboard);
//
//bboard w_pawn_defenders_from_west(bboard);
//bboard w_pawn_defenders_from_east(bboard);
//bboard b_pawn_defenders_from_west(bboard);
//bboard b_pawn_defenders_from_east(bboard);
//
//bboard pawns_with_east_adj_neighbors(bboard);
//bboard pawns_with_west_adj_neighbors(bboard);
//
//bboard closed_files(bboard, bboard);
//
//bboard open_files(bboard, bboard);
//
//bboard w_half_open_files(bboard, bboard);
//bboard b_half_open_files(bboard, bboard);
//
//fset bboard_2_fset(bboard);
//bboard fset_2_file_fill(fset f);
//
//fset islands_east_files(fset pawns);
//fset islands_west_files(fset pawns);
//
///*
// *  Pawn Isolanis and Hanging Pawns
// */
//
//bboard no_east_neighbors(bboard);
//bboard no_west_neighbors(bboard);
//bboard isolanis(bboard);
//bboard half_isolanis(bboard);
//
///* TODO: hanging pawns */
//bboard hanging_pawns();
///*
// *  Unfree pawns
// */
//
//bboard w_unfree_pawns(bboard, bboard);
//bboard b_unfree_pawns(bboard, bboard);
//
///*
// *  Open pawns (opposite of unfree pawns)
// */
//
//bboard w_open_pawns(bboard, bboard);
//bboard b_open_pawns(bboard, bboard);
///*
// *  Passed Pawns
// */
//
//bboard w_passed_pawns(bboard, bboard);
//bboard b_passed_pawns(bboard, bboard);
//
///*
// *  TODO: Candidate Pawns
// */
//
//
///*
// *   Backward Pawns
// */
//
//bboard w_backward(bboard, bboard);
//bboard b_backward(bboard, bboard);

