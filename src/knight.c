#include "knight.h"

bboard knight_attacks0(bboard b){
    return ((b << 17) & ~AFILE) |
        ((b << 10) & ~AFILE & ~BFILE) |
        ((b >>  6) & ~AFILE & ~BFILE) |
        ((b >> 15) & ~AFILE) |
        ((b << 15) & ~HFILE) |
        ((b <<  6) & ~GFILE & ~HFILE) |
        ((b >> 10) & ~GFILE & ~HFILE) |
        ((b >> 17) & ~HFILE);
}

bboard knight_attacks1(bboard knights){
    bboard west, east, attacks;
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

bboard knight_attacks2(bboard knights) {
    bboard l1 = (knights >> 1) & ~HFILE;
    bboard l2 = (knights >> 2) & ~GFILE & ~HFILE;
    bboard r1 = (knights << 1) & ~AFILE;
    bboard r2 = (knights << 2) & ~AFILE & ~BFILE;
    bboard h1 = l1 | r1;
    bboard h2 = l2 | r2;
    return (h1<<16) | (h1>>16) | (h2<<8) | (h2>>8);
}

bboard knight_fork(bboard targets) {
    bboard west, east, attak, forks;
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
int knight_distance(bboard b1, bboard b2) {
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
