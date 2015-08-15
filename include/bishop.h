#ifndef BISHOP_H
#define BISHOP_H

#include "bitboard.h"

extern const magic_entry bishop_magic_table[64];
uint64_t bishop_attacks(uint64_t, uint64_t);

#endif
