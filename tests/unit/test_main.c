#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "bitboard_test.h"
#include "pawn_test.h"
#include "bishop_test.h"
#include "rook_test.h"
#include "pqueue_test.h"
#include "move_test.h"
#include "hash_test.h"
#include "search_test.h"

int main() {
    ZOBRIST_transposition_init(1);
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
        cmocka_unit_test(b_pawns_able_to_push_test),
        cmocka_unit_test(b_pawns_able_to_double_push_test),
        cmocka_unit_test(pawns_able_to_push_test),
        cmocka_unit_test(pawns_able_to_double_push_test),
        cmocka_unit_test(w_east_lever_test),
        cmocka_unit_test(w_west_lever_test),
        cmocka_unit_test(w_any_lever_test),
        cmocka_unit_test(b_east_lever_test),
        cmocka_unit_test(b_west_lever_test),
        cmocka_unit_test(b_any_lever_test),
        cmocka_unit_test(w_safe_pawn_squares_test),
        cmocka_unit_test(b_safe_pawn_squares_test),
        cmocka_unit_test(w_pawns_able_to_capture_east_test),
        cmocka_unit_test(w_pawns_able_to_capture_west_test),
        cmocka_unit_test(w_pawns_able_to_capture_any_test),
        cmocka_unit_test(b_pawns_able_to_capture_east_test),
        cmocka_unit_test(b_pawns_able_to_capture_west_test),
        cmocka_unit_test(b_pawns_able_to_capture_any_test),
        cmocka_unit_test(pawns_able_to_capture_east_test),
        cmocka_unit_test(pawns_able_to_capture_west_test),
        cmocka_unit_test(pawns_able_to_capture_any_test),
        cmocka_unit_test(pawn_capture_targets_test),
        cmocka_unit_test(w_pawn_defended_from_west_test),
        cmocka_unit_test(w_pawn_defended_from_east_test),
        cmocka_unit_test(b_pawn_defended_from_west_test),
        cmocka_unit_test(b_pawn_defended_from_east_test),
        cmocka_unit_test(w_pawn_defenders_from_west_test),
        cmocka_unit_test(w_pawn_defenders_from_east_test),
        cmocka_unit_test(b_pawn_defenders_from_west_test),
        cmocka_unit_test(b_pawn_defenders_from_east_test),
        cmocka_unit_test(pawns_with_east_adj_neighbors_test),
        cmocka_unit_test(pawns_with_west_adj_neighbors_test),
        cmocka_unit_test(pawns_with_west_adj_neighbors_test),
        cmocka_unit_test(closed_files_test),
        cmocka_unit_test(open_files_test),
        cmocka_unit_test(w_half_open_files_test),
        cmocka_unit_test(b_half_open_files_test),
        cmocka_unit_test(bboard_to_fset_test),
        cmocka_unit_test(fset_to_file_fill_test),
        cmocka_unit_test(islands_east_files_test),
        cmocka_unit_test(islands_west_files_test),
        cmocka_unit_test(no_east_neighbors_test),
        cmocka_unit_test(no_west_neighbors_test),
        cmocka_unit_test(isolanis_test),
        cmocka_unit_test(half_isolanis_test),
        /* cmocka_unit_test(hanging_pawns_test),*/
        cmocka_unit_test(w_unfree_pawns_test),
        cmocka_unit_test(b_unfree_pawns_test),
        cmocka_unit_test(w_open_pawns_test),
        cmocka_unit_test(b_open_pawns_test),
        cmocka_unit_test(w_passed_pawns_test),
        cmocka_unit_test(b_passed_pawns_test),
        cmocka_unit_test(w_backward_test),
        cmocka_unit_test(b_backward_test),

        /* bishop tests */
        cmocka_unit_test(exhaustive_bishop_magic_test),
        cmocka_unit_test(bishop_attacks_test),

        /* rook tests */
        cmocka_unit_test(exhaustive_rook_magic_test),
        cmocka_unit_test(rook_attacks_test),

        /* priority queue tests */
        cmocka_unit_test(priority_queue_push_test),
        cmocka_unit_test(priority_queue_pop_test),

        /* move tests*/
        cmocka_unit_test(normal_move_test),
        cmocka_unit_test(capture_move_test),
        cmocka_unit_test(promotion_capture_move_test),
        cmocka_unit_test(in_check_test),
        cmocka_unit_test(rook_check_test),
        cmocka_unit_test(multiple_check_test),
        cmocka_unit_test(generate_pawn_single_pushes_white_test),
        cmocka_unit_test(generate_pawn_push_promotions_white_test),
        cmocka_unit_test(generate_pawn_double_pushes_white_test),
        cmocka_unit_test(generate_pawn_east_captures_white_test),
        cmocka_unit_test(generate_pawn_west_captures_white_test),
        cmocka_unit_test(generate_pawn_east_captures_promotions_white_test),
        cmocka_unit_test(generate_pawn_west_captures_promotions_white_test),
        cmocka_unit_test(generate_pawn_en_passant_white_test),
        cmocka_unit_test(generate_pawn_single_pushes_black_test),
        cmocka_unit_test(generate_pawn_push_promotions_black_test),
        cmocka_unit_test(generate_pawn_double_pushes_black_test),
        cmocka_unit_test(generate_pawn_east_captures_black_test),
        cmocka_unit_test(generate_pawn_west_captures_black_test),
        cmocka_unit_test(generate_pawn_east_captures_promotions_black_test),
        cmocka_unit_test(generate_pawn_west_captures_promotions_black_test),
        cmocka_unit_test(generate_pawn_en_passant_black_test),
        cmocka_unit_test(generate_bishop_moves_obstruction_white_test),
        cmocka_unit_test(generate_bishop_moves_capture_white_test),
        cmocka_unit_test(generate_bishop_moves_obstruction_black_test),
        cmocka_unit_test(generate_bishop_moves_capture_black_test),
        cmocka_unit_test(generate_rook_moves_obstruction_white_test),
        cmocka_unit_test(generate_rook_moves_capture_white_test),
        cmocka_unit_test(generate_rook_moves_obstruction_black_test),
        cmocka_unit_test(generate_rook_moves_capture_black_test),
        cmocka_unit_test(generate_queen_moves_obstruction_white_test),
        cmocka_unit_test(generate_queen_moves_capture_white_test),
        cmocka_unit_test(generate_queen_moves_obstruction_black_test),
        cmocka_unit_test(generate_queen_moves_capture_black_test),
        /* hash tests */
        cmocka_unit_test(ZOBRIST_incremental_update_test),

        /* search tests */
        cmocka_unit_test(search_check_test),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
