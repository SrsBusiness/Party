#ifndef ROOK_H
#define ROOK_H

#include "bitboard.h"

extern const magic_entry rook_magic_table[64];
uint64_t rook_attacks(uint64_t, uint64_t);
#endif
