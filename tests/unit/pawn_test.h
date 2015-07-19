#include "pawn.h"

/* w_single_push_targets */
void w_single_push_targets_test(void **state) {
    assert_int_equal(0x0000001022810000ul,
            w_single_push_targets(0x000000001422C100ul, 0x0000001222810000ul));
}

//bboard w_double_push_targets(bboard, bboard);
void w_double_push_targets_test(void **state) {
    /* Pawns can't jump over pieces */
    assert_int_equal(0x0000000000000000ul,
            w_double_push_targets(0x000000001422C100ul, 0x0000142281000000ul));
    assert_int_equal(0x0000000080000000ul,
            w_double_push_targets(0x000000001422C100ul, 0x0000142281800000ul));
}

//bboard b_single_push_targets(bboard, bboard);
void b_single_push_targets_test(void **state) {
    assert_int_equal(0x0000631408000000ul,
            b_single_push_targets(0x00E3140800000000ul, 0x0000631408000000ul));
}

//bboard b_double_push_targets(bboard, bboard);
void b_double_push_targets_test(void **state) {
    /* Pawns can't jump over pieces */
    assert_int_equal(0, b_double_push_targets(0x00E3140800000000ul,
                0x000000E314080000ul));
    assert_int_equal(0x000000C000000000ul,
            b_double_push_targets(0x00E3140800000000ul, 0x0000C0E314080000ul));
}

//bboard single_push_targets(bboard, bboard, int);
void single_push_targets_test(void **state) {
    /* white */
    assert_int_equal(0x0000001022810000ul,
            single_push_targets(0x000000001422C100ul, 0x0000001222810000ul,
                WHITE));
    /* black */
    assert_int_equal(0x0000631408000000ul,
            single_push_targets(0x00E3140800000000ul, 0x0000631408000000ul,
                BLACK));
}

//bboard double_push_targets(bboard, bboard, int);
void double_push_targets_test(void **state) {
    /* white */
    assert_int_equal(0x0000000000000000ul,
            double_push_targets(0x000000001422C100ul, 0x0000142281000000ul,
                WHITE));
    assert_int_equal(0x0000000080000000ul,
            double_push_targets(0x000000001422C100ul, 0x0000142281800000ul,
                WHITE));
    /* black */
    assert_int_equal(0, double_push_targets(0x00E3140800000000ul,
                0x000000E314080000ul, BLACK));
    assert_int_equal(0x000000C000000000ul,
            double_push_targets(0x00E3140800000000ul, 0x0000C0E314080000ul,
                BLACK));

}
//bboard w_pawns_able_to_push(bboard, bboard);
void w_pawns_able_to_push_test(void **state) {
    assert_int_equal(0x0000000004228100ul,
            w_pawns_able_to_push(0x000000001422C100ul, 0x0000000422810000ul));
}
//bboard w_pawns_able_to_double_push(bboard, bboard);
void w_pawns_able_to_double_push_test(void **state) {
    assert_int_equal(0, w_pawns_able_to_double_push(0x000000001422C100ul,
                0x00000000C1000000ul));
}
//bboard b_pawns_able_to_push(bboard, bboard);
void b_pawns_able_to_push_test(void **state) {
}
//bboard b_pawns_able_to_double_push(bboard, bboard);
void b_pawns_able_to_double_push_test(void **state) {
}
//bboard pawns_able_to_push(bboard, bboard, int);
void pawns_able_to_push_test(void **state) {
}
//bboard pawns_able_to_double_push(bboard, bboard, int);
void pawns_able_to_double_push_test(void **state) {
}
//bboard w_east_lever(bboard, bboard);
void w_east_lever_test(void **state) {
}
//bboard w_west_lever(bboard, bboard);
void w_west_lever_test(void **state) {
}
//bboard w_any_lever(bboard, bboard);
void w_any_lever_test(void **state) {
}
//bboard b_east_lever(bboard, bboard);
void b_east_lever_test(void **state) {
}
//bboard b_west_lever(bboard, bboard);
void b_west_lever_test(void **state) {
}
//bboard b_any_lever(bboard, bboard);
void b_any_lever_test(void **state) {
}
//bboard w_safe_pawn_squares(bboard, bboard);
void w_safe_pawn_squares_test(void **state) {
}
//bboard b_safe_pawn_squares(bboard, bboard);
void b_safe_pawn_squares_test(void **state) {
}
//bboard w_pawns_able_to_capture_east(bboard, bboard);
void w_pawns_able_to_capture_east_test(void **state) {
}
//bboard w_pawns_able_to_capture_west(bboard, bboard);
void w_pawns_able_to_capture_west_test(void **state) {
}
//bboard w_pawns_able_to_capture_any(bboard, bboard);
void w_pawns_able_to_capture_any_test(void **state) {
}
//bboard b_pawns_able_to_capture_east(bboard, bboard);
void b_pawns_able_to_capture_east_test(void **state) {
}
//bboard b_pawns_able_to_capture_west(bboard, bboard);
void b_pawns_able_to_capture_west_test(void **state) {
}
//bboard b_pawns_able_to_capture_any(bboard, bboard);
void b_pawns_able_to_capture_any_test(void **state) {
}
//bboard pawns_able_to_capture_east(bboard, bboard, int);
void pawns_able_to_capture_east_test(void **state) {
}
//bboard pawns_able_to_capture_west(bboard, bboard, int);
void pawns_able_to_capture_west_test(void **state) {
}
//bboard pawns_able_to_capture_any(bboard, bboard, int);
void pawns_able_to_capture_any_test(void **state) {
}
//bboard pawn_capture_targets(bboard, bboard, int);
void pawn_capture_targets_test(void **state) {
}
//bboard w_pawn_defended_from_west(bboard);
void w_pawn_defended_from_west_test(void **state) {
}
//bboard w_pawn_defended_from_east(bboard);
void w_pawn_defended_from_east_test(void **state) {
}
//bboard b_pawn_defended_from_west(bboard);
void b_pawn_defended_from_west_test(void **state) {
}
//bboard b_pawn_defended_from_east(bboard);
void b_pawn_defended_from_east_test(void **state) {
}
//bboard w_pawn_defenders_from_west(bboard);
void w_pawn_defenders_from_west_test(void **state) {
}
//bboard w_pawn_defenders_from_east(bboard);
void w_pawn_defenders_from_east_test(void **state) {
}
//bboard b_pawn_defenders_from_west(bboard);
void b_pawn_defenders_from_west_test(void **state) {
}
//bboard b_pawn_defenders_from_east(bboard);
void b_pawn_defenders_from_east_test(void **state) {
}
//bboard pawns_with_east_adj_neighbors(bboard);
void pawns_with_east_adj_neightbors_test(void **state) {
}
//bboard pawns_with_west_adj_neighbors(bboard);
void pawns_with_west_adj_neighbors_test(void **state) {
}
//bboard closed_files(bboard, bboard);
void closed_files_test(void **state) {
}
//bboard open_files(bboard, bboard);
void open_files_test(void **state) {
}
//bboard w_half_open_files(bboard, bboard);
void w_half_open_files_test(void **state) {
}
//bboard b_half_open_files(bboard, bboard);
void b_half_open_files_test(void **state) {
}
//fset bboard_to_fset(bboard);
void bboard_to_fset_test(void **state) {
}
//bboard fset_to_file_fill(fset f);
void fset_to_file_fill_test(void **state) {
}
//fset islands_east_files(fset pawns);
void islands_east_files_test(void **state) {
}
//fset islands_west_files(fset pawns);
void islands_west_files_test(void **state) {
}
///*
// *  Pawn Isolanis and Hanging Pawns
// */
//
//bboard no_east_neighbors(bboard);
void no_east_neighbors_test(void **state) {
}
//bboard no_west_neighbors(bboard);
void no_west_neighbors_test(void **state) {
}
//bboard isolanis(bboard);
void isolanis_test(void **state) {
}
//bboard half_isolanis(bboard);
void half_isolanis_test(void **state) {
}
///* TODO: hanging pawns */
//bboard hanging_pawns();
void hanging_pawns_test(void **state) {
}
///*
// *  Unfree pawns
// */
//
//bboard w_unfree_pawns(bboard, bboard);
void w_unfree_pawns_test(void **state) {
}
//bboard b_unfree_pawns(bboard, bboard);
void b_unfree_pawns_test(void **state) {
}
//
///*
// *  Open pawns (opposite of unfree pawns)
// */
//
//bboard w_open_pawns(bboard, bboard);
void w_open_pawns_test(void **state) {
}
//bboard b_open_pawns(bboard, bboard);
void b_open_pawns_test(void **state) {
}
///*
// *  Passed Pawns
// */
//
//bboard w_passed_pawns(bboard, bboard);
void w_passed_pawns_test(void **state) {
}
//bboard b_passed_pawns(bboard, bboard);
void b_passed_pawns_test(void **state) {
}
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
void w_backward_test(void **state) {
}
//bboard b_backward(bboard, bboard);
void b_backward_test(void **state) {
}

