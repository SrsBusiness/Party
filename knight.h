#ifndef KNIGHT_H
#define KNIGHT_H

#include "bitboard.h"

#define knight_attacks(x)       knight_attacks2(x)
#define knight_fill(x)          (knight_attacks(x) | x)
int knight_distance(bboard, bboard);
bboard knight_fork(bboard);
bboard knight_attacks0(bboard);
bboard knight_attacks1(bboard);
bboard knight_attacks2(bboard);

#endif
