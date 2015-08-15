#include "knight.h"

uint64_t knight_attacks0(uint64_t b){
    return ((b << 17) & ~BITBOARD_AFILE) |
        ((b << 10) & ~BITBOARD_AFILE & ~BITBOARD_BFILE) |
        ((b >>  6) & ~BITBOARD_AFILE & ~BITBOARD_BFILE) |
        ((b >> 15) & ~BITBOARD_AFILE) |
        ((b << 15) & ~BITBOARD_HFILE) |
        ((b <<  6) & ~BITBOARD_GFILE & ~BITBOARD_HFILE) |
        ((b >> 10) & ~BITBOARD_GFILE & ~BITBOARD_HFILE) |
        ((b >> 17) & ~BITBOARD_HFILE);
}

uint64_t knight_attacks1(uint64_t knights){
    uint64_t west, east, attacks;
    east     = east_one (knights);
    west     = west_one (knights);
    attacks  = (east|west) << 16;
    attacks |= (east|west) >> 16;
    east     = east_one (east);
    west     = west_one (west);
    attacks |= (east|west) <<  8;
    attacks |= (east|west) >>  8;
    return attacks; 
}

uint64_t knight_attacks2(uint64_t knights) {
    uint64_t l1 = (knights >> 1) & ~BITBOARD_HFILE;
    uint64_t l2 = (knights >> 2) & ~BITBOARD_GFILE & ~BITBOARD_HFILE;
    uint64_t r1 = (knights << 1) & ~BITBOARD_AFILE;
    uint64_t r2 = (knights << 2) & ~BITBOARD_AFILE & ~BITBOARD_BFILE;
    uint64_t h1 = l1 | r1;
    uint64_t h2 = l2 | r2;
    return (h1<<16) | (h1>>16) | (h2<<8) | (h2>>8);
}

uint64_t knight_fork(uint64_t targets) {
    uint64_t west, east, attak, forks;
    east = east_one(targets);
    west = west_one(targets);
    attak = east << 16;
    forks = (west << 16) & attak;
    attak |= west << 16;
    forks |= (east >> 16) & attak;
    attak |= east >> 16;
    forks |= (west >> 16) & attak;
    attak |= west >> 16;
    east = east_one(east);
    west = west_one(west);
    forks |= (east << 8) & attak;
    attak |= east << 8;
    forks |= (west << 8) & attak;
    attak |= west << 8;
    forks |= (east >> 8) & attak;
    attak |= east >> 8;
    forks |= (west >> 8) & attak;
    return forks;
}

// no set should be empty -> assert (b1 != 0 && b2 != 0 )
// b1 != b2
int knight_distance(uint64_t b1, uint64_t b2) {
    int d = 0;
    while ((b1 & b2) == 0) {
        b1 = knight_attacks(b1); // as long as sets are disjoint
        d++; // increment distance
    }
    return d;
}

int foo() {
    return 0;
}
