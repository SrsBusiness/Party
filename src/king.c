#include "king.h"

bboard king_attacks(bboard king_set) {
    bboard attacks = east_one(king_set) | west_one(king_set);
    king_set |= attacks;
    attacks |= north_one(king_set) | south_one(king_set);
    return attacks;
}
