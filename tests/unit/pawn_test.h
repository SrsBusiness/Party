#include "pawn.h"

/* w_single_push_targets */
void w_single_push_targets_test(void **state) {
    /* Should return squares e5, f4, b4, h3, and a3 */
    assert_int_equal(0x0000001022810000ul,
            w_single_push_targets(0x000000001422C100ul, 0x0000001222810000ul));
}

//bboard w_double_push_targets(bboard, bboard);
void w_double_push_targets_test(void **state) {
    /* Should not jump over pieces */
    assert_int_equal(0x0000000000000000ul,
            w_double_push_targets(0x000000001422C100ul, 0x0000142281000000ul));
    /* Should return squares h4 */
    assert_int_equal(0x0000000080000000ul,
            w_double_push_targets(0x000000001422C100ul, 0x0000142281800000ul));
}

//bboard b_single_push_targets(bboard, bboard);
void b_single_push_targets_test(void **state) {
    /* Should return squares g6, f7, b6, a6, e5, c5, d4 */
    assert_int_equal(0x0000631408000000ul,
            b_single_push_targets(0x00E3140800000000ul, 0x0000631408000000ul));
}

//bboard b_double_push_targets(bboard, bboard);
void b_double_push_targets_test(void **state) {
    /* Should not jump over pieces */
    assert_int_equal(0, b_double_push_targets(0x00E3140800000000ul,
                0x000000E314080000ul));
    /* Should return squares h5, g5 */
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
    /* Should return squares c4, f3, b3, h2, a2*/
    assert_int_equal(0x0000000004228100ul,
            w_pawns_able_to_push(0x000000001422C100ul, 0x0000000422810000ul));
}

//bboard w_pawns_able_to_double_push(bboard, bboard);
void w_pawns_able_to_double_push_test(void **state) {
    /* Should not jump over pieces */
    assert_int_equal(0, w_pawns_able_to_double_push(0x000000001422C100ul,
                0x00000000C1000000ul));
    /* Should return squares h2, a2 */
    assert_int_equal(0x0000000000008100ul,
            w_pawns_able_to_double_push(0x000000001422C100ul,
                0x00000000C1810000ul));
}

//bboard b_pawns_able_to_push(bboard, bboard);
void b_pawns_able_to_push_test(void **state) {
    /* Should return squares h7, f7, b7, a7, e6, c6 */
    assert_int_equal(0x00A3140000000000ul,
            b_pawns_able_to_push(0x00E3140800000000ul, 0x0000A31400000000ul));
}

//bboard b_pawns_able_to_double_push(bboard, bboard);
void b_pawns_able_to_double_push_test(void **state) {
    /* Should not jump over pieces*/
    assert_int_equal(0, b_pawns_able_to_double_push(0x00E3140800000000ul,
                0x000000E300000000ul));
    /* Should return squares h7, f7, b7, a7*/
    assert_int_equal(0x00A3000000000000ul,
            b_pawns_able_to_double_push(0x00E3140800000000ul,
                0x0000E3A300000000ul));
}

//bboard pawns_able_to_push(bboard, bboard, int);
void pawns_able_to_push_test(void **state) {
    /* white */ 
    assert_int_equal(0x0000000004228100ul,
            pawns_able_to_push(0x000000001422C100ul, 0x0000000422810000ul,
                WHITE));
    /* black */
    assert_int_equal(0x00A3140000000000ul,
            pawns_able_to_push(0x00E3140800000000ul, 0x0000A31400000000ul,
                BLACK));
}

//bboard pawns_able_to_double_push(bboard, bboard, int);
void pawns_able_to_double_push_test(void **state) {
    /* white */
    assert_int_equal(0, pawns_able_to_double_push(0x000000001422C100ul,
                0x00000000C1000000ul, WHITE));
    assert_int_equal(0x0000000000008100ul,
            pawns_able_to_double_push(0x000000001422C100ul,
                0x00000000C1810000ul, WHITE));
    /* black */
    assert_int_equal(0, pawns_able_to_double_push(0x00E3140800000000ul,
                0x000000E300000000ul, BLACK));
    assert_int_equal(0x00A3000000000000ul,
            pawns_able_to_double_push(0x00E3140800000000ul,
                0x0000E3A300000000ul, BLACK));
}

//bboard w_east_lever(bboard, bboard);
void w_east_lever_test(void **state) {
    assert_int_equal(0x0000000004000000ul,
            w_east_lever(0x000000000C000000ul, 0x0000000800000000ul)); 
}

//bboard w_west_lever(bboard, bboard);
void w_west_lever_test(void **state) {
    assert_int_equal(0x0000000008000000ul,
            w_west_lever(0x000000000C000000ul, 0x0000000400000000ul));
}

//bboard w_any_lever(bboard, bboard);
void w_any_lever_test(void **state) {
    assert_int_equal(0x0000000004000000ul,
            w_any_lever(0x000000000C000000ul, 0x0000000A00000000ul));
}

//bboard b_east_lever(bboard, bboard);
void b_east_lever_test(void **state) {
    assert_int_equal(0x0000000400000000ul,
            b_east_lever(0x0000000008000000ul, 0x0000000C00000000ul));
}

//bboard b_west_lever(bboard, bboard);
void b_west_lever_test(void **state) {
    assert_int_equal(0x0000000400000000ul,
            b_west_lever(0x0000000002000000ul, 0x0000000C00000000ul));
}

//bboard b_any_lever(bboard, bboard);
void b_any_lever_test(void **state) {
    assert_int_equal(0x0000000C00000000ul,
            b_any_lever(0x000000000C000000ul, 0x0000000C00000000ul));
}

//bboard w_safe_pawn_squares(bboard, bboard);
void w_safe_pawn_squares_test(void **state) {
    assert_int_equal(0xFFFF08D7EBFFFFFFul,
            w_safe_pawn_squares(0x000000000C10E300ul, 0x00E3140800000000ul));
}

//bboard b_safe_pawn_squares(bboard, bboard);
void b_safe_pawn_squares_test(void **state) {
    assert_int_equal(0xFFFFFFEBD708FFFFul,
            b_safe_pawn_squares(0x000000000C10E300ul, 0x00E3140800000000ul));
}

//bboard w_pawns_able_to_capture_east(bboard, bboard);
void w_pawns_able_to_capture_east_test(void **state) {
    assert_int_equal(0x0000000004000000ul,
            w_pawns_able_to_capture_east(0x000000000C10E300ul,
                0x00E3140800000000ul));
}

//bboard w_pawns_able_to_capture_west(bboard, bboard);
void w_pawns_able_to_capture_west_test(void **state) {
    assert_int_equal(0x0000000010000000ul,
            w_pawns_able_to_capture_west(0x0000000018000000ul,
                0x0000100800000000ul));
}

//bboard w_pawns_able_to_capture_any(bboard, bboard);
void w_pawns_able_to_capture_any_test(void **state) {
    assert_int_equal(0x0000000014000000ul,
            w_pawns_able_to_capture_any(0x000000001C000000ul,
                0x0000100800000000ul));
}

//bboard b_pawns_able_to_capture_east(bboard, bboard);
void b_pawns_able_to_capture_east_test(void **state) {
    assert_int_equal(0x0000000800000000ul,
            b_pawns_able_to_capture_east(0x0000100800000000ul,
                0x0000000018000000ul));
}

//bboard b_pawns_able_to_capture_west(bboard, bboard);
void b_pawns_able_to_capture_west_test(void **state) {
    assert_int_equal(0x0000000800000000ul,
            b_pawns_able_to_capture_west(0x00E3140800000000ul,
                0x000000000C10E300ul));
}

//bboard b_pawns_able_to_capture_any(bboard, bboard);
void b_pawns_able_to_capture_any_test(void **state) {
    assert_int_equal(0x0000001400000000ul,
            b_pawns_able_to_capture_any(0x0000001C00000000ul,
                0x0000000008000000ul));
}

//bboard pawns_able_to_capture_east(bboard, bboard, int);
void pawns_able_to_capture_east_test(void **state) {
    /* white */
    assert_int_equal(0x0000000004000000ul,
            pawns_able_to_capture_east(0x000000000C10E300ul,
                0x00E3140800000000ul, WHITE));
    /* black */
    assert_int_equal(0x0000000800000000ul,
            pawns_able_to_capture_east(0x0000100800000000ul,
                0x0000000018000000ul, BLACK));
}

//bboard pawns_able_to_capture_west(bboard, bboard, int);
void pawns_able_to_capture_west_test(void **state) {
    /* white */
    assert_int_equal(0x0000000010000000ul,
            pawns_able_to_capture_west(0x0000000018000000ul,
                0x0000100800000000ul, WHITE));
    /* black */
    assert_int_equal(0x0000000800000000ul,
            pawns_able_to_capture_west(0x00E3140800000000ul,
                0x000000000C10E300ul, BLACK));
}

//bboard pawns_able_to_capture_any(bboard, bboard, int);
void pawns_able_to_capture_any_test(void **state) {
    /* white */
    assert_int_equal(0x0000000014000000ul,
            pawns_able_to_capture_any(0x000000001C000000ul,
                0x0000100800000000ul, WHITE));
    /* black */
    assert_int_equal(0x0000001400000000ul,
            pawns_able_to_capture_any(0x0000001C00000000ul,
                0x0000000008000000ul, BLACK));
}

//bboard pawn_capture_targets(bboard, bboard, int);
void pawn_capture_targets_test(void **state) {
    /* white */
    assert_int_equal(0x0000000C00000000ul,
            pawn_capture_targets(0x000000001C000000ul,
                0x0000100C00000000ul, WHITE));
    /* black */
    assert_int_equal(0x000000001C000000ul,
            pawn_capture_targets(0x0000100C00000000ul,
                0x000000001C000000ul, BLACK));
}

//bboard w_pawn_defended_from_west(bboard);
void w_pawn_defended_from_west_test(void **state) {
    assert_int_equal(0x0000000010000000ul,
            w_pawn_defended_from_west(0x0000000010080000ul));
}

//bboard w_pawn_defended_from_east(bboard);
void w_pawn_defended_from_east_test(void **state) {
    assert_int_equal(0x0000000008000000ul,
            w_pawn_defended_from_east(0x0000000008100000ul));
}

//bboard b_pawn_defended_from_west(bboard);
void b_pawn_defended_from_west_test(void **state) {
    assert_int_equal(0x0000001000000000ul,
            b_pawn_defended_from_west(0x0000081000000000ul));
}

//bboard b_pawn_defended_from_east(bboard);
void b_pawn_defended_from_east_test(void **state) {
    assert_int_equal(0x0000000800000000ul,
            b_pawn_defended_from_east(0x0000100800000000ul));
}

//bboard w_pawn_defenders_from_west(bboard);
void w_pawn_defenders_from_west_test(void **state) {
    assert_int_equal(0x0000000000080000ul,
            w_pawn_defenders_from_west(0x0000000010080000ul));
}

//bboard w_pawn_defenders_from_east(bboard);
void w_pawn_defenders_from_east_test(void **state) {
    assert_int_equal(0x0000000000100000ul,
            w_pawn_defenders_from_east(0x0000000008100000ul));
}

//bboard b_pawn_defenders_from_west(bboard);
void b_pawn_defenders_from_west_test(void **state) {
    assert_int_equal(0x0000080000000000ul,
            b_pawn_defenders_from_west(0x0000081000000000ul));
}

//bboard b_pawn_defenders_from_east(bboard);
void b_pawn_defenders_from_east_test(void **state) {
    assert_int_equal(0x0000100000000000ul,
            b_pawn_defenders_from_east(0x0000100800000000ul));
}

//bboard pawns_with_east_adj_neighbors(bboard);
void pawns_with_east_adj_neighbors_test(void **state) {
    assert_int_equal(0x0000000008000000ul,
            pawns_with_east_adj_neighbors(0x0000000018000000ul));
}

//bboard pawns_with_west_adj_neighbors(bboard);
void pawns_with_west_adj_neighbors_test(void **state) {
    assert_int_equal(0x0000000010000000ul,
            pawns_with_west_adj_neighbors(0x0000000018000000ul));
}

//bboard closed_files(bboard, bboard);
void closed_files_test(void **state) {
    assert_int_equal(AFILE | BFILE | FFILE,
            closed_files(0x0000000000003300ul, 0x0023000000000000ul));
}

//bboard open_files(bboard, bboard);
void open_files_test(void **state) {
    assert_int_equal(CFILE | DFILE | GFILE | HFILE,
            open_files(0x0000000000003300ul, 0x0023000000000000ul));
}

//bboard w_half_open_files(bboard, bboard);
void w_half_open_files_test(void **state) {
    assert_int_equal(EFILE | GFILE,
            w_half_open_files(0x000000000000AF00ul,
                0x00FF000000000000ul));
}

//bboard b_half_open_files(bboard, bboard);
void b_half_open_files_test(void **state) {
    assert_int_equal(EFILE | GFILE,
            b_half_open_files(0x000000000000FF00ul,
                0x00AF000000000000ul));
}

//fset bboard_to_fset(bboard);
void bboard_to_fset_test(void **state) {
    assert_int_equal(0x25,
            bboard_to_fset(0x0000000001200400ul));
}

//bboard fset_to_file_fill(fset f);
void fset_to_file_fill_test(void **state) {
    assert_int_equal(AFILE | CFILE | DFILE | FFILE,
            fset_to_file_fill(0x2D));
}

//fset islands_east_files(fset pawns);
void islands_east_files_test(void **state) {
    assert_int_equal(0x40,
            islands_east_files(0x7E));
}

//fset islands_west_files(fset pawns);
void islands_west_files_test(void **state) {
    assert_int_equal(0x02,
            islands_west_files(0x7E));
}

//bboard no_east_neighbors(bboard);
void no_east_neighbors_test(void **state) {
    assert_int_equal(0x0000000000001000ul,
            no_east_neighbors(0x0000000000001C00ul));
}

//bboard no_west_neighbors(bboard);
void no_west_neighbors_test(void **state) {
    assert_int_equal(0x0000000000000400ul,
            no_west_neighbors(0x0000000000001C00ul));
}

//bboard isolanis(bboard);
void isolanis_test(void **state) {
    assert_int_equal(0x0000000000000800ul, isolanis(0x000000000000C800ul));
}

//bboard half_isolanis(bboard);
void half_isolanis_test(void **state) {
    assert_int_equal(0x0000000000004400ul,
            half_isolanis(0x0000000000007C00ul));
}

//bboard hanging_pawns();
void hanging_pawns_test(void **state) {
}

//bboard w_unfree_pawns(bboard, bboard);
void w_unfree_pawns_test(void **state) {
    assert_int_equal(0x0000000010000000ul,
            w_unfree_pawns(0x0000000018000000ul, 0x0000001000000000ul));
}

//bboard b_unfree_pawns(bboard, bboard);
void b_unfree_pawns_test(void **state) {
    assert_int_equal(0x0000001000000000ul,
            b_unfree_pawns(0x0000000010000000ul, 0x0000001800000000ul));
}

//bboard w_open_pawns(bboard, bboard);
void w_open_pawns_test(void **state) {
    assert_int_equal(0x0000000008000000ul,
            w_open_pawns(0x0000000018000000ul, 0x0000001000000000ul));

}

//bboard b_open_pawns(bboard, bboard);
void b_open_pawns_test(void **state) {
    assert_int_equal(0x0000000800000000ul,
            b_open_pawns(0x0000000010000000ul, 0x0000001800000000ul));
}

//bboard w_passed_pawns(bboard, bboard);
void w_passed_pawns_test(void **state) {
    assert_int_equal(0x0000000000000800ul,
            w_passed_pawns(0x0000000000001800ul, 0x0020000000000000ul));
}

//bboard b_passed_pawns(bboard, bboard);
void b_passed_pawns_test(void **state) {
    assert_int_equal(0x0008000000000000ul,
            b_passed_pawns(0x0000000000002000ul, 0x0018000000000000ul));
}

//bboard w_backward(bboard, bboard);
void w_backward_test(void **state) {
    assert_int_equal(0x0000000000000200ul,
            w_backward(0x0000000004000200ul, 0x0000000400000000ul));
    assert_int_equal(0x0000000000000200ul,
            w_backward(0x0000000004000200ul, 0x0000000100000000ul));
}

//bboard b_backward(bboard, bboard);
void b_backward_test(void **state) {
    assert_int_equal(0x0002000000000000ul,
            b_backward(0x0000000004000000ul, 0x0002000400000000ul));
    assert_int_equal(0x0002000000000000ul,
            b_backward(0x0000000001000000ul, 0x0002000400000000ul));
}
