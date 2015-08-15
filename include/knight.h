#ifndef KNIGHT_H
#define KNIGHT_H

#include "bitboard.h"

#define knight_attacks(x)       knight_attacks2(x)
#define knight_fill(x)          (knight_attacks(x) | x)
int knight_distance(uint64_t, uint64_t);
uint64_t knight_fork(uint64_t);
uint64_t knight_attacks0(uint64_t);
uint64_t knight_attacks1(uint64_t);
uint64_t knight_attacks2(uint64_t);

#endif
