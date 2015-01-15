#include "bitboard.h"
#include "pawn.h"

const bboard square_pp[64] = {0, 0, 0, 0, 0, 0, 0, 0,
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

bboard w_single_push_targets(bboard wpawns, bboard empty) {
    return north_one(wpawns) & empty;
}

bboard w_dbl_push_targets(bboard wpawns, bboard empty) {
    bboard single_pushs = w_single_push_targets(wpawns, empty);
    return north_one(single_pushs) & empty & RANK4;
}

bboard b_single_push_targets(bboard bpawns, bboard empty) {
    return south_one(bpawns) & empty;
}

bboard b_double_push_targets(bboard bpawns, bboard empty) {
    bboard single_pushs = b_single_push_targets(bpawns, empty);
    return south_one(single_pushs) & empty & RANK5;
}

/*
 *  returns source squares of pawns able to be pushed
 */

bboard w_pawns_able_2_push(bboard wpawns, bboard empty) {
    return south_one(empty) & wpawns;
}

bboard w_pawns_able_2_dbl_push(bboard wpawns, bboard empty) {
    bboard emptyRank3 = south_one(empty & RANK4) & empty;
    return w_pawns_able_2_push(wpawns, emptyRank3);
}

bboard b_pawns_able_2_push(bboard bpawns, bboard empty) {
    return north_one(empty) & bpawns;
}

bboard b_pawns_able_2_dbl_push(bboard bpawns, bboard empty) {
    bboard emptyRank6 = north_one(empty & RANK5) & empty;
    return w_pawns_able_2_push(bpawns, emptyRank6);
}

/*
 *  Pawn lever functions
 */

bboard w_east_lever(bboard wpawns, bboard bpawns) {
    return wpawns & b_pawn_west_attacks(bpawns);
}

bboard w_west_lever(bboard wpawns, bboard bpawns) {
    return wpawns & b_pawn_east_attacks(bpawns);
}

bboard w_any_lever(bboard wpawns, bboard bpawns) {
    return w_east_lever(wpawns, bpawns)
        | w_west_lever(wpawns, bpawns);
}



bboard b_east_lever(bboard wpawns, bboard bpawns) {
    return bpawns & w_pawn_west_attacks(wpawns);
}

bboard b_west_lever(bboard wpawns, bboard bpawns) {
    return bpawns & w_pawn_east_attacks(wpawns);
}

bboard b_any_lever(bboard wpawns, bboard bpawns) {
    return b_east_lever(bpawns, wpawns)
        | b_west_lever(bpawns, wpawns);
}

/*
 *  Static exchange eval for pawn attacks
 */

bboard w_safe_pawn_squares(bboard wpawns, bboard bpawns) {
    return w_pawn_dbl_attacks(wpawns) | ~b_pawn_any_attacks(bpawns) |
        (w_pawn_single_attacks(wpawns) & ~b_pawn_dbl_attacks(bpawns));
}

bboard b_safe_pawn_squares(bboard wpawns, bboard bpawns) {
    return b_pawn_dbl_attacks(bpawns) | ~w_pawn_any_attacks(wpawns) |
        (b_pawn_single_attacks(bpawns) & ~w_pawn_dbl_attacks(wpawns));
}

/*
 *  Pawn captures: returns set of pawns able to capture
 */

bboard w_pawns_able_2_capture_east(bboard wpawns, bboard bpieces) {
    return wpawns & b_pawn_west_attacks(bpieces);
}

bboard w_pawns_able_2_capture_west(bboard wpawns, bboard bpieces) {
    return wpawns & b_pawn_east_attacks(bpieces);
}

bboard w_pawns_able_2_capture_any(bboard wpawns, bboard bpieces) {
    return wpawns & b_pawn_any_attacks(bpieces);
}


bboard b_pawns_able_2_capture_east(bboard bpawns, bboard wpieces) {
    return bpawns & w_pawn_west_attacks(wpieces);
}

bboard b_pawns_able_2_capture_west(bboard bpawns, bboard wpieces) {
    return bpawns & w_pawn_east_attacks(wpieces);
}

bboard b_pawns_able_2_capture_any(bboard bpawns, bboard wpieces) {
    return bpawns & w_pawn_any_attacks(wpieces);
}

/*
 *  returns defended pawns
 */

bboard w_pawn_defended_from_west(bboard wpawns) {
    return wpawns & w_pawn_east_attacks(wpawns);
}

bboard w_pawn_defended_from_east(bboard wpawns) {
    return wpawns & w_pawn_west_attacks(wpawns);
}

bboard b_pawn_defended_from_west(bboard bpawns) {
    return bpawns & b_pawn_east_attacks(bpawns);
}

bboard b_pawn_defended_from_east(bboard bpawns) {
    return bpawns & b_pawn_west_attacks(bpawns);
}

/*
 *  returns pawn defenders
 */

bboard w_pawn_defenders_from_west(bboard wpawns) {
    return wpawns & b_pawn_west_attacks(wpawns);
}

bboard w_pawn_defenders_from_east(bboard wpawns) {
    return wpawns & b_pawn_east_attacks(wpawns);
}

bboard b_pawn_defenders_from_west(bboard bpawns) {
    return bpawns & w_pawn_west_attacks(bpawns);
}

bboard b_pawn_defenders_from_east(bboard bpawns) {
    return bpawns & w_pawn_east_attacks(bpawns);
}

/*
 *  returns pawns with neighbors
 */

bboard pawns_with_east_neighbors(bboard pawns) {
    return pawns & west_one (pawns);
}

bboard pawns_with_west_neighbors(bboard pawns) {
    return pawns & east_one (pawns);
}

/*
 *  returns closed files
 */

bboard closed_files(bboard wpawns, bboard bpawns) {
    return file_fill(wpawns) & file_fill(bpawns);
}

/*
 *  returns open files
 */

bboard open_files(bboard wpawns, bboard bpawns) {
    return ~file_fill(wpawns | bpawns);
}


/*
 *  returns half-open files
 */

bboard w_half_open_files(bboard wpawns, bboard bpawns) {
    return ~file_fill(wpawns) ^ open_files(wpawns, bpawns);
}

bboard b_half_open_files(bboard wpawns, bboard bpawns) {
    return ~file_fill(bpawns) ^ open_files(wpawns, bpawns);
}

/*
 *  TODO: half-closed files
 */

/*
 *  For doubled/tripled pawns, or with file and popcount
 */


