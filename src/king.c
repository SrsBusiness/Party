#include "king.h"

uint64_t king_attacks(uint64_t king_set) {
    uint64_t attacks = east_one(king_set) | west_one(king_set);
    king_set |= attacks;
    attacks |= north_one(king_set) | south_one(king_set);
    return attacks;
}
