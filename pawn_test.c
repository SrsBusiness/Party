#include "pawn.h"
#include "bitboard.h"
#include "minicurses.h"

bboard single_args_test_cases[] = {
    0x000000000000FF00, /*  white starting squares  */
    0x00FF000000000000, /*  black starting squares  */
    0x000000001422C100, /*  white sicilian maroczy  */
    0x000000001020C700, /*  white sicilian dragon   */
    0x00B3480000000000, /*  black sicilian dragon   */
    0x00E3180000000000, /*  black sicilian scheveningen */ 
    0x00E3140800000000, /*  black semi-slav */
    0x000000000C10E300, /*  white meran */
    0x000000081400E300, /*  white king's indian classical   */
    0x00A7481000000000, /*  black king's indian classical   */
    0x000000000800E300, /*  white isolated queen pawn   */
    0x000000000800A300
};

bboard double_args_gen[] = {
    0x0000000008502000,
    0x0020409800000000
};

bboard double_args_empty[] = {
    ~0x0020409800000000,
    ~0x0000000008502000
};

bboard double_args_white[] = {
    0x0000000008502000,
    0x0000004005800200,
    0x0000000004100A00
};

bboard double_args_black[] = {
    0x0020409800000000,
    0x0020400500000000,
    0x0000020011080000
};

void single_arg_test(bboard (*f)(bboard), char *desc) {
    int i;
    clear_all();
    display_text(desc, 2, 1);
    display_text("Args", 6, 1); 
    display_text("Returned", 20, 1);
    for (i = 0; i < sizeof(single_args_test_cases) / 
            sizeof(single_args_test_cases[0]); i++) {
        move_cursor(4,1);
        printf("%d", i);
        display_bboard(single_args_test_cases[i], 8, 1);
        display_bboard(f(single_args_test_cases[i]), 22, 1);
        getchar();
    }
}

void single_arg_tests() {
    hide_cursor();
    single_arg_test(w_pawn_defended_from_west,
            "White pawns defended from the west");
    single_arg_test(w_pawn_defended_from_east,
            "White pawns defended from the east");
    single_arg_test(b_pawn_defended_from_west,
            "Black pawns defended from the west");
    single_arg_test(b_pawn_defended_from_east,
            "Black pawns defended from the east");
    single_arg_test(w_pawn_defenders_from_west,
            "White pawn defenders from the west");
    single_arg_test(w_pawn_defenders_from_east,
            "White pawn defenders from the east");
    single_arg_test(b_pawn_defenders_from_west,
            "Black pawn defenders from the west");
    single_arg_test(b_pawn_defenders_from_east,
            "Black pawn defenders from the east");
    single_arg_test(pawns_with_east_adj_neighbors,
            "Pawns with adjacent east neighbors");
    single_arg_test(pawns_with_west_adj_neighbors,
            "Pawns with adjacent west neighbors");
    single_arg_test(no_east_neighbors,
            "Pawns with no east neighbors");
    single_arg_test(no_west_neighbors,
            "Pawns with no west neighbors");
    single_arg_test(isolanis,
            "Isolanis");
    single_arg_test(half_isolanis,
            "Half isolanis");
    show_cursor();
}

void double_args_test_empty(bboard (*f)(bboard, bboard), char *desc) {
    int i;
    clear_all();
    display_text(desc, 2, 1);
    display_text("Gen", 6, 1); 
    display_text("Empty", 6, 20);
    display_text("Returned", 20, 1);
    for (i = 0; i < sizeof(double_args_gen) / 
            sizeof(double_args_gen[0]); i++) {
        move_cursor(4,1);
        printf("%d", i);
        display_bboard(double_args_gen[i], 8, 1);
        display_bboard(double_args_empty[i], 8, 20);
        display_bboard(f(double_args_gen[i], double_args_empty[i]), 22, 1);
        getchar();
    }
}

void double_args_test(bboard (*f)(bboard, bboard), char *desc, int swap) {
    int i;
    clear_all();
    display_text(desc, 2, 1);
    display_text("White", 6, 1); 
    display_text("Black", 6, 20);
    display_text("Returned", 20, 1);
    for (i = 0; i < sizeof(double_args_white) / 
            sizeof(double_args_white[0]); i++) {
        move_cursor(4,1);
        printf("%d", i);
        display_bboard(double_args_white[i], 8, 1);
        display_bboard(double_args_black[i], 8, 20);
        if(!swap)
            display_bboard(f(double_args_white[i], double_args_black[i]), 22, 1);
        else
            display_bboard(f(double_args_black[i], double_args_white[i]), 22, 1);

        getchar();
    }
}

void double_args_tests() {
    hide_cursor();
    /* bboard w_single_push_targets(bboard, bboard); */
    double_args_test_empty(w_single_push_targets,
            "White single push targets");
    /* bboard w_dbl_push_targets(bboard, bboard); */
    double_args_test_empty(w_dbl_push_targets,
            "White double push targets");
    /* bboard b_single_push_targets(bboard, bboard); */
    double_args_test_empty(b_single_push_targets,
            "Black single push targets");
    /* bboard b_double_push_targets(bboard, bboard); */
    double_args_test_empty(b_dbl_push_targets,
            "Black double push targets");
    /* bboard w_pawns_able_2_push(bboard, bboard); */
    double_args_test_empty(w_pawns_able_2_push,
            "White pawns able to push");
    /* bboard w_pawns_able_2_dbl_push(bboard, bboard); */
    double_args_test_empty(w_pawns_able_2_dbl_push,
            "White pawns able to double push");
    /* bboard b_pawns_able_2_push(bboard, bboard); */
    double_args_test_empty(b_pawns_able_2_push,
            "Black pawns able to push");
    /* bboard b_pawns_able_2_dbl_push(bboard, bboard); */
    double_args_test_empty(b_pawns_able_2_dbl_push,
            "Black pawns able to double push");
    /* bboard w_east_lever(bboard, bboard); */
    double_args_test(w_east_lever,
            "White east levers", 0);
    /* bboard w_west_lever(bboard, bboard); */
    double_args_test(w_west_lever,
            "White west levers", 0);
    /* bboard w_any_lever(bboard, bboard); */
    double_args_test(w_any_lever,
            "White any levers", 0);
    /* bboard b_east_lever(bboard, bboard); */
    double_args_test(b_east_lever,
            "Black east levers", 0);
    /* bboard b_west_lever(bboard, bboard); */
    double_args_test(b_west_lever,
            "Black west levers", 0);
    /* bboard b_any_lever(bboard, bboard); */
    double_args_test(b_any_lever,
            "Black any levers", 0);
    /* bboard w_safe_pawn_squares(bboard, bboard); */
    double_args_test(w_safe_pawn_squares,
            "White safe pawn squares", 0);
    /* bboard b_safe_pawn_squares(bboard, bboard); */
    double_args_test(b_safe_pawn_squares,
            "Black safe pawn squares", 0);
    /* bboard w_pawns_able_2_capture_east(bboard, bboard); */
    double_args_test(w_pawns_able_2_capture_east,
            "White pawns able to capture east", 0);
    /* bboard w_pawns_able_2_capture_west(bboard, bboard); */
    double_args_test(w_pawns_able_2_capture_west,
            "White pawns able to capture west", 0);
    /* bboard w_pawns_able_2_capture_any(bboard, bboard); */
    double_args_test(w_pawns_able_2_capture_any,
            "White pawns able to capture any", 0);
    /* bboard b_pawns_able_2_capture_east(bboard, bboard); */
    double_args_test(b_pawns_able_2_capture_east,
            "Black pawns able to capture east", 1);
    /* bboard b_pawns_able_2_capture_west(bboard, bboard); */
    double_args_test(b_pawns_able_2_capture_west,
            "Black pawns able to capture west", 1);
    /* bboard b_pawns_able_2_capture_any(bboard, bboard); */
    double_args_test(b_pawns_able_2_capture_any,
            "Black pawns able to capture any", 1);
    /* bboard closed_files(bboard, bboard); */
    double_args_test(closed_files,
            "Closed files", 0);
    /* bboard open_files(bboard, bboard); */
    double_args_test(open_files,
            "Open files", 0);
    /* bboard w_half_open_files(bboard, bboard); */
    double_args_test(w_half_open_files,
            "White half open files", 0);
    /* bboard b_half_open_files(bboard, bboard); */
    double_args_test(b_half_open_files,
            "Black half open files", 0);
    /* bboard w_unfree_pawns(bboard, bboard); */
    double_args_test(w_unfree_pawns,
            "White unfree pawns", 0);
    /* bboard b_unfree_pawns(bboard, bboard); */
    double_args_test(b_unfree_pawns,
            "Black unfree pawns", 0);
    /* bboard w_open_pawns(bboard, bboard); */
    double_args_test(w_open_pawns,
            "White open pawns", 0);
    /* bboard b_open_pawns(bboard, bboard); */
    double_args_test(b_open_pawns,
            "Black open pawns", 0);
    /* bboard w_passed_pawns(bboard, bboard); */
    double_args_test(w_passed_pawns,
            "White passed pawns", 0);
    /* bboard b_passed_pawns(bboard, bboard); */
    double_args_test(b_passed_pawns,
            "Black passed pawns", 0);
    /* bboard w_backward(bboard, bboard); */
    double_args_test(w_backward,
            "White backward pawns", 0);
    /* bboard b_backward(bboard, bboard); */
    double_args_test(b_backward,
            "Black backward pawns", 0);
    show_cursor();
}

int main() {
    //single_arg_tests();
    clear_all();
    //display_bboard(south_span(double_args_black[0]), 2, 2);
    //getchar();
    double_args_tests();
}


