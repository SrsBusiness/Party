#ifndef PAWN_H
#define PAWN_H

#include <stdint.h>
#include "bitboard.h"

#define w_pawn_east_attacks(x)  ne_one(x)
#define w_pawn_west_attacks(x)  nw_one(x)

#define b_pawn_east_attacks(x)  se_one(x)
#define b_pawn_west_attacks(x)  sw_one(x)

#define w_pawn_any_attacks(x)   (w_pawn_east_attacks(x) | w_pawn_west_attacks(x))
#define w_pawn_dbl_attacks(x)   (w_pawn_east_attacks(x) & w_pawn_west_attacks(x))
#define w_pawn_single_attacks(x)    (w_pawn_east_attacks(x) ^ w_pawn_west_attacks(x))
#define b_pawn_any_attacks(x)   (b_pawn_east_attacks(x) | b_pawn_west_attacks(x))
#define b_pawn_dbl_attacks(x)   (b_pawn_east_attacks(x) & b_pawn_west_attacks(x))
#define b_pawn_single_attacks(x)    (b_pawn_east_attacks(x) ^ b_pawn_west_attacks(x))

#define w_east_attack_front_span(x) (east_one(w_front_span(x)))
#define w_west_attack_front_span(x) (west_one(w_front_span(x)))
#define b_east_attack_front_span(x) (east_one(b_front_span(x)))
#define b_west_attack_front_span(x) (west_one(b_front_span(x)))

#define w_east_attack_rear_span(x)  (east_one(w_rear_fill(x)))
#define w_west_attack_rear_span(x)  (west_one(w_rear_fill(x)))
#define b_east_attack_rear_span(x)  (east_one(b_rear_fill(x)))
#define b_west_attack_rear_span(x)  (west_one(b_rear_fill(x)))

#define east_attack_file_fill(x)    (east_one(file_fill(x)))
#define west_attack_file_fill(x)    (west_one(file_fill(x)))

typedef uint8_t fset;

bboard w_single_push_targets(bboard, bboard);
bboard w_dbl_push_targets(bboard, bboard);
bboard b_single_push_targets(bboard, bboard);
bboard b_double_push_targets(bboard, bboard);

/*
 *  returns source squares of pawns able to be pushed
 */

bboard w_pawns_able_2_push(bboard, bboard);
bboard w_pawns_able_2_dbl_push(bboard, bboard);
bboard b_pawns_able_2_push(bboard, bboard);
bboard b_pawns_able_2_dbl_push(bboard, bboard);

/*
 *  Pawn lever functions
 */

bboard w_east_lever(bboard, bboard);
bboard w_west_lever(bboard, bboard);
bboard w_any_lever(bboard, bboard);
bboard b_east_lever(bboard, bboard);
bboard b_west_lever(bboard, bboard);
bboard b_any_lever(bboard, bboard);

/*
 *  Static exchange eval for pawn attacks
 */

bboard w_safe_pawn_squares(bboard, bboard);
bboard b_safe_pawn_squares(bboard, bboard);

/*
 *  Pawn captures: returns set of pawns able to capture
 */

bboard w_pawns_able_2_capture_east(bboard, bboard);
bboard w_pawns_able_2_capture_west(bboard, bboard);
bboard w_pawns_able_2_capture_any(bboard, bboard);
bboard b_pawns_able_2_capture_east(bboard, bboard);
bboard b_pawns_able_2_capture_west(bboard, bboard);
bboard b_pawns_able_2_capture_any(bboard, bboard);

/*
 *  returns defended pawns
 */

bboard w_pawn_defended_from_west(bboard);
bboard w_pawn_defended_from_east(bboard);
bboard b_pawn_defended_from_west(bboard);
bboard b_pawn_defended_from_east(bboard);

/*
 *  returns pawn defenders
 */

bboard w_pawn_defenders_from_west(bboard);
bboard w_pawn_defenders_from_east(bboard);
bboard b_pawn_defenders_from_west(bboard);
bboard b_pawn_defenders_from_east(bboard);

/*
 *  returns pawns with neighbors
 */

bboard pawns_with_east_adj_neighbors(bboard);
bboard pawns_with_west_adj_neighbors(bboard);

/*
 *  returns closed files
 */

bboard closed_files(bboard, bboard);

/*
 *  returns open files
 */

bboard open_files(bboard, bboard);

/*
 *  returns half-open files
 */

bboard w_half_open_files(bboard, bboard);
bboard b_half_open_files(bboard, bboard);

/*
 *  TODO: half-closed files
 */

/*
 *  Filesets 
 */

fset bboard_2_fset(bboard);
bboard fset_2_file_fill(fset f);

/*
 *  For doubled/tripled pawns, or with file and popcount
 */


/*
 *  Pawn Islands
 *  To count pawn islands, do popcnt(islands_east_files(pawns))
 */
    
fset islands_east_files(fset pawns);
fset islands_west_files(fset pawns);

/*
 *  Pawn Isolanis and Hanging Pawns
 */

bboard no_east_neighbors(bboard);
bboard no_west_neighbors(bboard);
bboard isolanis(bboard);
bboard half_isolanis(bboard);

/* TODO: hanging pawns */
bboard hanging_pawns();
/*
 *  Unfree pawns
 */

bboard w_unfree_pawns(bboard, bboard);
bboard b_unfree_pawns(bboard, bboard);

/*
 *  Open pawns (opposite of unfree pawns)
 */

bboard w_open_pawns(bboard, bboard);
bboard b_open_pawns(bboard, bboard);
/*
 *  Passed Pawns
 */

bboard w_passed_pawns(bboard, bboard);
bboard b_passed_pawns(bboard, bboard);

/*
 *  TODO: Candidate Pawns
 */


/*
 *   Backward Pawns
 */

bboard w_backward(bboard, bboard);
bboard b_backward(bboard, bboard);

#endif
