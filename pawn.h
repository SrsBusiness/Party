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
