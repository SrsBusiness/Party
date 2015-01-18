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
};

void single_arg_test(bboard (*f)(bboard), char *desc) {
    int i;
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

    /* bboard w_pawn_defended_from_west(bboard); */
    single_arg_test(w_pawn_defended_from_west,
            "White pawns defended from the west");

    /* bboard w_pawn_defended_from_east(bboard); */
    single_arg_test(w_pawn_defended_from_east,
            "White pawns defended from the east");

    /* bboard b_pawn_defended_from_west(bboard); */
    single_arg_test(b_pawn_defended_from_west,
            "Black pawns defended from the west");

    /* bboard b_pawn_defended_from_east(bboard); */
    single_arg_test(b_pawn_defended_from_east,
            "Black pawns defended from the east");

    /* bboard w_pawn_defenders_from_west(bboard); */
    single_arg_test(w_pawn_defenders_from_west,
            "White pawn defenders from the west");

    /* bboard w_pawn_defenders_from_east(bboard); */
    single_arg_test(w_pawn_defenders_from_east,
            "White pawn defenders from the east");

    /* bboard b_pawn_defenders_from_west(bboard); */
    single_arg_test(b_pawn_defenders_from_west,
            "Black pawn defenders from the west");


    /* bboard b_pawn_defenders_from_east(bboard); */
    single_arg_test(b_pawn_defenders_from_east,
            "Black pawn defenders from the east");

    /* bboard pawns_with_east_neighbors(bboard); */
    single_arg_test(pawns_with_east_adj_neighbors,
            "Pawns with adjacent east neighbors");

    /* bboard pawns_with_west_neighbors(bboard); */
    single_arg_test(pawns_with_west_adj_neighbors,
            "Pawns with adjacent west neighbors");

    /* bboard no_east_neighbors(bboard); */
    single_arg_test(no_east_neighbors,
            "Pawns with no east neighbors");

    /* bboard no_west_neighbors(bboard); */
    single_arg_test(no_west_neighbors,
            "Pawns with no west neighbors");

    /* bboard isolanis(bboard); */
    single_arg_test(isolanis,
            "Isolanis");

    /* bboard half_isolanis(bboard); */
    single_arg_test(half_isolanis,
            "Half isolanis");

    show_cursor();
}

void double_args_test() {
}

int main() {
    clear_all();
    single_arg_tests();
}


