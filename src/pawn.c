#include <stdio.h>
#include <stdlib.h>
#include "bitboard.h"
#include "pawn.h"

const uint64_t square_pp[64] = {0, 0, 0, 0, 0, 0, 0, 0,
    0x7F7F7F7F7F7F7F00, //A2
    0xFFFFFFFFFFFFFF00, //b2
    0xFFFFFFFFFFFFFF00, //c2
    0xFFFFFFFFFFFFFF00, //d2
    0xFFFFFFFFFFFFFF00, //e2
    0xFFFFFFFFFFFFFF00, //f2
    0xFFFFFFFFFFFFFF00, //g2
    0xFEFEFEFEFEFEFE00, //h2
    0x3F3F3F3F3F3F0000, //a3
    0x7F7F7F7F7F7F0000, //b3
    0xFFFFFFFFFFFF0000, //c3
    0xFFFFFFFFFFFF0000, //d3
    0xFFFFFFFFFFFF0000, //e3
    0xFFFFFFFFFFFF0000, //f3
    0xFEFEFEFEFEFE0000, //g3
    0xFCFCFCFCFCFC0000, //h3
    0x1F1F1F1F1F000000, //a4
    0x3F3F3F3F3F000000, //b4
    0x7F7F7F7F7F000000, //c4
    0xFFFFFFFFFF000000, //d4
    0xFFFFFFFFFF000000, //e4
    0xFEFEFEFEFE000000, //f4
    0xFCFCFCFCFC000000, //g4
    0xF8F8F8F8F8000000, //h4
    0x0F0F0F0F00000000, //a5
    0x1F1F1F1F00000000, //b5
    0x3F3F3F3F00000000, //c5
    0x7F7F7F7F00000000, //d5
    0xFEFEFEFE00000000, //e5
    0xFCFCFCFC00000000, //f5
    0xF8F8F8F800000000, //g5
    0xF0F0F0F000000000, //h5
    0x0707070000000000, //a6
    0x0F0F0F0000000000, //b6
    0x1F1F1F0000000000, //c6
    0x3E3E3E0000000000, //d6
    0x7C7C7C0000000000, //e6
    0xF8F8F80000000000, //f6
    0xF0F0F00000000000, //g6
    0xE0E0E00000000000, //h6
    0x0303000000000000, //a7
    0x0707000000000000, //b7
    0x0E0E000000000000, //c7
    0x1C1C000000000000, //d7
    0x3838000000000000, //e7
    0x7070000000000000, //f7
    0xE0E0000000000000, //g7
    0xC0C0000000000000, //h7
    0x0100000000000000, //a8
    0x0200000000000000, //b8
    0x0400000000000000, //c8
    0x0800000000000000, //d8
    0x1000000000000000, //e8
    0x2000000000000000, //f8
    0x4000000000000000, //g8
    0x8000000000000000  //h8
};

/*
 *  returns destination squares of pawn pushes
 */

uint64_t w_single_push_targets(uint64_t wpawns, uint64_t empty) {
    return north_one(wpawns) & empty;
}

uint64_t w_double_push_targets(uint64_t wpawns, uint64_t empty) {
    uint64_t single_pushs = w_single_push_targets(wpawns, empty);
    return north_one(single_pushs) & empty & BITBOARD_RANK4;
}

uint64_t b_single_push_targets(uint64_t bpawns, uint64_t empty) {
    return south_one(bpawns) & empty;
}

uint64_t b_double_push_targets(uint64_t bpawns, uint64_t empty) {
    uint64_t single_pushs = b_single_push_targets(bpawns, empty);
    return south_one(single_pushs) & empty & BITBOARD_RANK5;
}

uint64_t single_push_targets(uint64_t pawns, uint64_t empty, int color) {
    switch(color) {
        case WHITE:
            return w_single_push_targets(pawns, empty);
        case BLACK:
            return b_single_push_targets(pawns, empty);
        default:
            fprintf(stderr, "Invalid color\n");
            exit(1);
    }
}

uint64_t double_push_targets(uint64_t pawns, uint64_t empty, int color) {
    switch(color) {
        case WHITE:
            return w_double_push_targets(pawns, empty);
        case BLACK:
            return b_double_push_targets(pawns, empty);
        default:
            fprintf(stderr, "Invalid color\n");
            exit(1);
    }
}

/*
 *  returns source squares of pawns able to be pushed
 */

uint64_t w_pawns_able_to_push(uint64_t wpawns, uint64_t empty) {
    return south_one(empty) & wpawns;
}

uint64_t w_pawns_able_to_double_push(uint64_t wpawns, uint64_t empty) {
    uint64_t emptyRank3 = south_one(empty & BITBOARD_RANK4) & empty;
    return w_pawns_able_to_push(wpawns, emptyRank3);
}

uint64_t b_pawns_able_to_push(uint64_t bpawns, uint64_t empty) {
    return north_one(empty) & bpawns;
}

uint64_t b_pawns_able_to_double_push(uint64_t bpawns, uint64_t empty) {
    uint64_t emptyRank6 = north_one(empty & BITBOARD_RANK5) & empty;
    return b_pawns_able_to_push(bpawns, emptyRank6);
}

uint64_t pawns_able_to_push(uint64_t pawns, uint64_t empty, int color) {
    switch(color) {
        case WHITE:
            return w_pawns_able_to_push(pawns, empty);
        case BLACK:
            return b_pawns_able_to_push(pawns, empty);
        default:
            fprintf(stderr, "Invalid color\n");
            exit(1);
    }
}

uint64_t pawns_able_to_double_push(uint64_t pawns, uint64_t empty, int color) {
    switch(color) {
        case WHITE:
            return w_pawns_able_to_double_push(pawns, empty);
        case BLACK:
            return b_pawns_able_to_double_push(pawns, empty);
        default:
            fprintf(stderr, "Invalid color\n");
            exit(1);
    }
}

/*
 *  Pawn lever functions
 */

uint64_t w_east_lever(uint64_t wpawns, uint64_t bpawns) {
    return wpawns & b_pawn_west_attacks(bpawns);
}

uint64_t w_west_lever(uint64_t wpawns, uint64_t bpawns) {
    return wpawns & b_pawn_east_attacks(bpawns);
}

uint64_t w_any_lever(uint64_t wpawns, uint64_t bpawns) {
    return w_east_lever(wpawns, bpawns) | 
        w_west_lever(wpawns, bpawns);
}



uint64_t b_east_lever(uint64_t wpawns, uint64_t bpawns) {
    return bpawns & w_pawn_west_attacks(wpawns);
}

uint64_t b_west_lever(uint64_t wpawns, uint64_t bpawns) {
    return bpawns & w_pawn_east_attacks(wpawns);
}

uint64_t b_any_lever(uint64_t wpawns, uint64_t bpawns) {
    return b_east_lever(wpawns, bpawns) | 
        b_west_lever(wpawns, bpawns);
}

/*
 *  Static exchange eval for pawn attacks
 */

uint64_t w_safe_pawn_squares(uint64_t wpawns, uint64_t bpawns) {
    return w_pawn_double_attacks(wpawns) | ~b_pawn_any_attacks(bpawns) |
        (w_pawn_single_attacks(wpawns) & ~b_pawn_double_attacks(bpawns));
}

uint64_t b_safe_pawn_squares(uint64_t wpawns, uint64_t bpawns) {
    return b_pawn_double_attacks(bpawns) | ~w_pawn_any_attacks(wpawns) |
        (b_pawn_single_attacks(bpawns) & ~w_pawn_double_attacks(wpawns));
}

/*
 *  Pawn captures: returns set of pawns able to capture
 */

uint64_t w_pawns_able_to_capture_east(uint64_t wpawns, uint64_t bpieces) {
    return wpawns & b_pawn_west_attacks(bpieces);
}

uint64_t w_pawns_able_to_capture_west(uint64_t wpawns, uint64_t bpieces) {
    return wpawns & b_pawn_east_attacks(bpieces);
}

uint64_t w_pawns_able_to_capture_any(uint64_t wpawns, uint64_t bpieces) {
    return wpawns & b_pawn_any_attacks(bpieces);
}


uint64_t b_pawns_able_to_capture_east(uint64_t bpawns, uint64_t wpieces) {
    return bpawns & w_pawn_west_attacks(wpieces);
}

uint64_t b_pawns_able_to_capture_west(uint64_t bpawns, uint64_t wpieces) {
    return bpawns & w_pawn_east_attacks(wpieces);
}

uint64_t b_pawns_able_to_capture_any(uint64_t bpawns, uint64_t wpieces) {
    return bpawns & w_pawn_any_attacks(wpieces);
}

uint64_t pawns_able_to_capture_east(uint64_t pawns, uint64_t pieces, int color) {
    switch(color) {
        case WHITE:
            return w_pawns_able_to_capture_east(pawns, pieces);
        case BLACK:
            return b_pawns_able_to_capture_east(pawns, pieces);
        default:
            fprintf(stderr, "Invalid color\n");
            exit(1);
    }
}

uint64_t pawns_able_to_capture_west(uint64_t pawns, uint64_t pieces, int color) {
    switch(color) {
        case WHITE:
            return w_pawns_able_to_capture_west(pawns, pieces);
        case BLACK:
            return b_pawns_able_to_capture_west(pawns, pieces);
        default:
            fprintf(stderr, "Invalid color\n");
            exit(1);
    }
}

uint64_t pawns_able_to_capture_any(uint64_t pawns, uint64_t pieces, int color) {
    switch(color) {
        case WHITE:
            return w_pawns_able_to_capture_any(pawns, pieces);
        case BLACK:
            return b_pawns_able_to_capture_any(pawns, pieces);
        default:
            fprintf(stderr, "Invalid color\n");
            exit(1);
    }
}

uint64_t pawn_capture_targets(uint64_t pawns, uint64_t pieces, int color) {
    switch(color) {
        case WHITE:
            return w_pawn_capture_targets(pawns, pieces);
        case BLACK:
            return b_pawn_capture_targets(pawns, pieces);
        default:
            fprintf(stderr, "Invalid color\n");
            exit(1);
    }
}

/*
 *  returns defended pawns
 */

uint64_t w_pawn_defended_from_west(uint64_t wpawns) {
    return wpawns & w_pawn_east_attacks(wpawns);
}

uint64_t w_pawn_defended_from_east(uint64_t wpawns) {
    return wpawns & w_pawn_west_attacks(wpawns);
}

uint64_t b_pawn_defended_from_west(uint64_t bpawns) {
    return bpawns & b_pawn_east_attacks(bpawns);
}

uint64_t b_pawn_defended_from_east(uint64_t bpawns) {
    return bpawns & b_pawn_west_attacks(bpawns);
}

/*
 *  returns pawn defenders
 */

uint64_t w_pawn_defenders_from_west(uint64_t wpawns) {
    return wpawns & b_pawn_west_attacks(wpawns);
}

uint64_t w_pawn_defenders_from_east(uint64_t wpawns) {
    return wpawns & b_pawn_east_attacks(wpawns);
}

uint64_t b_pawn_defenders_from_west(uint64_t bpawns) {
    return bpawns & w_pawn_west_attacks(bpawns);
}

uint64_t b_pawn_defenders_from_east(uint64_t bpawns) {
    return bpawns & w_pawn_east_attacks(bpawns);
}

/*
 *  returns pawns with neighbors
 */

uint64_t pawns_with_east_adj_neighbors(uint64_t pawns) {
    return pawns & west_one(pawns);
}

uint64_t pawns_with_west_adj_neighbors(uint64_t pawns) {
    return pawns & east_one(pawns);
}

/*
 *  returns closed files
 */

uint64_t closed_files(uint64_t wpawns, uint64_t bpawns) {
    return file_fill(wpawns) & file_fill(bpawns);
}

/*
 *  returns open files
 */

uint64_t open_files(uint64_t wpawns, uint64_t bpawns) {
    return ~file_fill(wpawns | bpawns);
}


/*
 *  returns half-open files
 */

uint64_t w_half_open_files(uint64_t wpawns, uint64_t bpawns) {
    return ~file_fill(wpawns) ^ open_files(wpawns, bpawns);
}

uint64_t b_half_open_files(uint64_t wpawns, uint64_t bpawns) {
    return ~file_fill(bpawns) ^ open_files(wpawns, bpawns);
}

/*
 *  TODO: half-closed files
 */

/*
 *  Filesets 
 */

fset bboard_to_fset(uint64_t pawns) {
    return (fset)south_fill(pawns);
}

uint64_t fset_to_file_fill(fset f) {
    uint64_t fill = f;
    return fill * BITBOARD_AFILE;
}

/*
 *  For doubled/tripled pawns, or with file and popcount
 */


/*
 *  Pawn Islands
 *  To count pawn islands, do popcnt(islands_east_files(pawns))
 */
    
fset islands_east_files(fset pawns) { 
    return pawns & ~(pawns >> 1);
}

fset islands_west_files(fset pawns) {
    return pawns & ~(pawns << 1);
}

/*
 *  Pawn Isolanis and Hanging Pawns
 */

uint64_t no_east_neighbors(uint64_t pawns) {
    return pawns & ~west_attack_file_fill(pawns);
}

uint64_t no_west_neighbors(uint64_t pawns) {
    return pawns & ~east_attack_file_fill(pawns);
}

uint64_t isolanis(uint64_t pawns) {
    return no_east_neighbors(pawns) & no_west_neighbors(pawns);
}

uint64_t half_isolanis(uint64_t pawns) {
    return no_east_neighbors(pawns) ^ no_west_neighbors(pawns);
}

/* TODO: hanging pawns */
uint64_t hanging_pawns() {
    return 0;
}

/*
 *  Unfree pawns
 */

uint64_t w_unfree_pawns(uint64_t wpawns, uint64_t bpawns) {
    return wpawns & b_front_span(bpawns);
}

uint64_t b_unfree_pawns(uint64_t wpawns, uint64_t bpawns) {
    return bpawns & w_front_span(wpawns);
}

/*
 *  Open pawns (opposite of unfree pawns)
 */

uint64_t w_open_pawns(uint64_t wpawns, uint64_t bpawns) {
    return wpawns & ~b_front_span(bpawns);
}

uint64_t b_open_pawns(uint64_t wpawns, uint64_t bpawns) {
    return bpawns & ~w_front_span(wpawns);
}

/*
 *  Passed Pawns
 */

uint64_t w_passed_pawns(uint64_t wpawns, uint64_t bpawns) {
    uint64_t all_front_spans = b_front_span(bpawns);
    all_front_spans |= east_one(all_front_spans) | west_one(all_front_spans);
    return wpawns & ~all_front_spans;
}

uint64_t b_passed_pawns(uint64_t wpawns, uint64_t bpawns) {
    uint64_t all_front_spans = w_front_span(wpawns);
    all_front_spans |= east_one(all_front_spans) | west_one(all_front_spans);
    return bpawns & ~all_front_spans;
}

/*
 *  TODO: Candidate Pawns
 */


/*
 *   Backward Pawns
 */

uint64_t w_backward(uint64_t wpawns, uint64_t bpawns) {
    uint64_t w_attack_spans = w_east_attack_front_span(wpawns) | 
        w_west_attack_front_span(wpawns);
    uint64_t b_attacks = b_pawn_east_attacks(bpawns) |
        b_pawn_west_attacks(bpawns);
    return south_fill(b_attacks & ~w_attack_spans) &
        w_open_pawns(wpawns, bpawns);
}

uint64_t b_backward(uint64_t wpawns, uint64_t bpawns) {
    uint64_t b_attack_spans = b_east_attack_front_span(bpawns) | 
        b_west_attack_front_span(bpawns);
    uint64_t w_attacks = w_pawn_east_attacks(wpawns) |
        w_pawn_west_attacks(wpawns);
    return north_fill(w_attacks & ~b_attack_spans) &
        b_open_pawns(wpawns, bpawns);
}


