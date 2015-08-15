#ifndef PAWN_H
#define PAWN_H

#include <stdint.h>
#include "bitboard.h"

#define w_pawn_east_attacks(x)  ne_one(x)
#define w_pawn_west_attacks(x)  nw_one(x)

#define b_pawn_east_attacks(x)  se_one(x)
#define b_pawn_west_attacks(x)  sw_one(x)

/* return squares attack by 1 or 2 pawns */
#define w_pawn_any_attacks(x)   (w_pawn_east_attacks(x) | w_pawn_west_attacks(x))
#define w_pawn_double_attacks(x)   (w_pawn_east_attacks(x) & w_pawn_west_attacks(x))
#define w_pawn_single_attacks(x)    (w_pawn_east_attacks(x) ^ w_pawn_west_attacks(x))
#define b_pawn_any_attacks(x)   (b_pawn_east_attacks(x) | b_pawn_west_attacks(x))
#define b_pawn_double_attacks(x)   (b_pawn_east_attacks(x) & b_pawn_west_attacks(x))
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

#define w_pawn_capture_targets(x, y)    (w_pawn_any_attacks(x) & (y))
#define b_pawn_capture_targets(x, y)    (b_pawn_any_attacks(x) & (y))

typedef uint8_t fset;

uint64_t w_single_push_targets(uint64_t, uint64_t);
uint64_t w_double_push_targets(uint64_t, uint64_t);
uint64_t b_single_push_targets(uint64_t, uint64_t);
uint64_t b_double_push_targets(uint64_t, uint64_t);
uint64_t single_push_targets(uint64_t, uint64_t, int);
uint64_t double_push_targets(uint64_t, uint64_t, int);
/*
 *  returns source squares of pawns able to be pushed
 */

uint64_t w_pawns_able_to_push(uint64_t, uint64_t);
uint64_t w_pawns_able_to_double_push(uint64_t, uint64_t);
uint64_t b_pawns_able_to_push(uint64_t, uint64_t);
uint64_t b_pawns_able_to_double_push(uint64_t, uint64_t);
uint64_t pawns_able_to_push(uint64_t, uint64_t, int);
uint64_t pawns_able_to_double_push(uint64_t, uint64_t, int);
/*
 *  Pawn lever functions
 */

uint64_t w_east_lever(uint64_t, uint64_t);
uint64_t w_west_lever(uint64_t, uint64_t);
uint64_t w_any_lever(uint64_t, uint64_t);
uint64_t b_east_lever(uint64_t, uint64_t);
uint64_t b_west_lever(uint64_t, uint64_t);
uint64_t b_any_lever(uint64_t, uint64_t);

/*
 *  Static exchange eval for pawn attacks
 */

uint64_t w_safe_pawn_squares(uint64_t, uint64_t);
uint64_t b_safe_pawn_squares(uint64_t, uint64_t);

/*
 *  Pawn captures: returns set of pawns able to capture
 */

uint64_t w_pawns_able_to_capture_east(uint64_t, uint64_t);
uint64_t w_pawns_able_to_capture_west(uint64_t, uint64_t);
uint64_t w_pawns_able_to_capture_any(uint64_t, uint64_t);
uint64_t b_pawns_able_to_capture_east(uint64_t, uint64_t);
uint64_t b_pawns_able_to_capture_west(uint64_t, uint64_t);
uint64_t b_pawns_able_to_capture_any(uint64_t, uint64_t);
uint64_t pawns_able_to_capture_east(uint64_t, uint64_t, int);
uint64_t pawns_able_to_capture_west(uint64_t, uint64_t, int);
uint64_t pawns_able_to_capture_any(uint64_t, uint64_t, int);

/*
 *  Pawn capture targets: return set of pawn capture target sets
 */

uint64_t pawn_capture_targets(uint64_t, uint64_t, int);

/*
 *  returns defended pawns
 */

uint64_t w_pawn_defended_from_west(uint64_t);
uint64_t w_pawn_defended_from_east(uint64_t);
uint64_t b_pawn_defended_from_west(uint64_t);
uint64_t b_pawn_defended_from_east(uint64_t);

/*
 *  returns pawn defenders
 */

uint64_t w_pawn_defenders_from_west(uint64_t);
uint64_t w_pawn_defenders_from_east(uint64_t);
uint64_t b_pawn_defenders_from_west(uint64_t);
uint64_t b_pawn_defenders_from_east(uint64_t);

/*
 *  returns pawns with neighbors
 */

uint64_t pawns_with_east_adj_neighbors(uint64_t);
uint64_t pawns_with_west_adj_neighbors(uint64_t);

/*
 *  returns closed files
 */

uint64_t closed_files(uint64_t, uint64_t);

/*
 *  returns open files
 */

uint64_t open_files(uint64_t, uint64_t);

/*
 *  returns half-open files
 */

uint64_t w_half_open_files(uint64_t, uint64_t);
uint64_t b_half_open_files(uint64_t, uint64_t);

/*
 *  TODO: half-closed files
 */

/*
 *  Filesets 
 */

fset bboard_to_fset(uint64_t);
uint64_t fset_to_file_fill(fset f);

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

uint64_t no_east_neighbors(uint64_t);
uint64_t no_west_neighbors(uint64_t);
uint64_t isolanis(uint64_t);
uint64_t half_isolanis(uint64_t);

/* TODO: hanging pawns */
uint64_t hanging_pawns();
/*
 *  Unfree pawns
 */

uint64_t w_unfree_pawns(uint64_t, uint64_t);
uint64_t b_unfree_pawns(uint64_t, uint64_t);

/*
 *  Open pawns (opposite of unfree pawns)
 */

uint64_t w_open_pawns(uint64_t, uint64_t);
uint64_t b_open_pawns(uint64_t, uint64_t);
/*
 *  Passed Pawns
 */

uint64_t w_passed_pawns(uint64_t, uint64_t);
uint64_t b_passed_pawns(uint64_t, uint64_t);

/*
 *  TODO: Candidate Pawns
 */


/*
 *   Backward Pawns
 */

uint64_t w_backward(uint64_t, uint64_t);
uint64_t b_backward(uint64_t, uint64_t);

#endif
