#include "bitboard.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

bboard fliph0 (bboard x) {
    const bboard k1 = 0x5555555555555555;
    const bboard k2 = 0x3333333333333333;
    const bboard k4 = 0x0f0f0f0f0f0f0f0f;
    x = ((x >> 1) & k1) | ((x & k1) << 1);
    x = ((x >> 2) & k2) | ((x & k2) << 2);
    x = ((x >> 4) & k4) | ((x & k4) << 4);
    return x;
}

bboard fliph1 (bboard x) {
    const bboard k1 = 0x5555555555555555;
    const bboard k2 = 0x3333333333333333;
    const bboard k4 = 0x0f0f0f0f0f0f0f0f;
    x = ((x >> 1) & k1) +  2*(x & k1);
    x = ((x >> 2) & k2) +  4*(x & k2);
    x = ((x >> 4) & k4) + 16*(x & k4);
    return x;
}

bboard fliph2 (bboard x) {
    const bboard k1 = 0x5555555555555555;
    const bboard k2 = 0x3333333333333333;
    const bboard k4 = 0x0f0f0f0f0f0f0f0f;
    x ^= k4 & (x ^ rolll(x, 8));
    x ^= k2 & (x ^ rolll(x, 4));
    x ^= k1 & (x ^ rolll(x, 2));
    return rorll(x, 7);
}

bboard flipdd(bboard x) { // light square diagonal
    bboard t;
    const bboard k1 = 0x5500550055005500;
    const bboard k2 = 0x3333000033330000;
    const bboard k4 = 0x0f0f0f0f00000000;
    t = k4 & (x ^ (x << 28));
    x ^= t ^ (t >> 28);
    t = k2 & (x ^ (x << 14));
    x ^= t ^ (t >> 14);
    t = k1 & (x ^ (x << 7));
    x ^= t ^ (t >> 7);
    return x;
}

bboard flipld(bboard x) { // dark square diagonal
    bboard t;
    const bboard k1 = 0xaa00aa00aa00aa00;
    const bboard k2 = 0xcccc0000cccc0000;
    const bboard k4 = 0xf0f0f0f00f0f0f0f;
    t = x ^ (x << 36);
    x ^= k4 & (t ^ (x >> 36));
    t = k2 & (x ^ (x << 18));
    x ^= t ^ (t >> 18);
    t = k1 & (x ^ (x << 9));
    x ^= t ^ (t >> 9);
    return x;
}

// 180 rotation
bboard rotate_pi(bboard x){
    // vertical flip
    x = flipv(x);
    // horizontal flip
    const bboard k1 = 0x5555555555555555;
    const bboard k2 = 0x3333333333333333;
    const bboard k4 = 0x0f0f0f0f0f0f0f0f;
    x = ((x >> 1) & k1) +  2*(x & k1);
    x = ((x >> 2) & k2) +  4*(x & k2);
    x = ((x >> 4) & k4) + 16*(x & k4);
    return x;
}

// 90 clockwise
bboard rotate_clockwise(bboard x){
    // to be optimized with inlining...
    return flipld(flipv(x));
}

// 90 anticlockwise
bboard rotate_anticlockwise(bboard x){
    // to be optimized with inlining...
    return flipdd(flipv(x));
}

bboard north_span(bboard x){
    x <<= 8;
    x |= (x << 16);
    x |= (x << 32);
    return x;
}

bboard south_span(bboard x){
    x >>= 8;
    x |= (x >> 16);
    x |= (x >> 32);
    return x;
}

bboard north_fill(bboard gen) {
   gen |= (gen <<  8);
   gen |= (gen << 16);
   gen |= (gen << 32);
   return gen;
}
 
bboard south_fill(bboard gen) {
   gen |= (gen >>  8);
   gen |= (gen >> 16);
   gen |= (gen >> 32);
   return gen;
}



// file fill = north_span(x) | south_span(x) | x, shows open/closed files

// front span = north_span(white), south_span(black) (rear spans are vice versa)

// Inter span = north_span(white) & south_span(black), occurs on closed files

// Pawn Attack front Span = east and west front spans
// Pawn Attack rear span = east and west rear spans
//
//
// Occlusive fill - fill until it is blocked
// Useful for sliding pieces 
//

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

bboard kingAttacks(bboard kingSet) {
    bboard attacks = east_one(kingSet) | west_one(kingSet);
    kingSet |= attacks;
    attacks |= north_one(kingSet) | south_one(kingSet);
    return attacks;
}

// algorithm based on the square of the passed pawn
// returns a bitboard set containing all squares which,
// if occupied, can be caught by a king, whose position
// is represented by an input bitboard
//

bboard catchable_passers_white(bboard king){
    return 0;
}

bboard catchable_passers_black(bboard king){
    return 0;
}



//int knight_distance_occl(bboard b1, bboard b2, bboard occupied){
//    int d = 0;
//    while ((b1 & b2) == 0) {
//        b1 = knight_attacks(b1) & ~occupied; // as long as sets are disjoint
//        d++; // increment distance
//    }
//    return d;
//}

//int knightDistance(bboard source, bboard dest) {
//    return calcKnightDistance(source, dest);
//}

/*
 * gen refers to the generation set i.e. set of pieces generating the fill
 * pro refers to the propgation set i.e. set of empty squares on the board 
 */

bboard soutOccl(bboard gen, bboard pro) {
    gen |= pro & (gen >> 8);
    pro &= (pro >>  8);
    gen |= pro & (gen >> 16);
    pro &= (pro >> 16);
    gen |= pro & (gen >> 32);
    return gen;
}

bboard nortOccl(bboard gen, bboard pro) {
    gen |= pro & (gen << 8);
    pro &= (pro <<  8);
    gen |= pro & (gen << 16);
    pro &= (pro << 16);
    gen |= pro & (gen << 32);
    return gen;
}

bboard eastOccl(bboard gen, bboard pro) {
    pro &= ~AFILE;
    gen |= pro & (gen << 1);
    pro &= (pro << 1);
    gen |= pro & (gen << 2);
    pro &= (pro << 2);
    gen |= pro & (gen << 4);
    return gen;
}

bboard noEaOccl(bboard gen, bboard pro) {
    pro &= ~AFILE;
    gen |= pro & (gen << 9);
    pro &= (pro <<  9);
    gen |= pro & (gen << 18);
    pro &= (pro << 18);
    gen |= pro & (gen << 36);
    return gen;
}

bboard soEaOccl(bboard gen, bboard pro) {
    pro &= ~AFILE;
    gen |= pro & (gen >> 7);
    pro &= (pro >>  7);
    gen |= pro & (gen >> 14);
    pro &= (pro >> 14);
    gen |= pro & (gen >> 28);
    return gen;
}

bboard westOccl(bboard gen, bboard pro) {
    pro &= ~HFILE;
    gen |= pro & (gen >> 1);
    pro &= (pro >> 1);
    gen |= pro & (gen >> 2);
    pro &= (pro >> 2);
    gen |= pro & (gen >> 4);
    return gen;
}

bboard soWeOccl(bboard gen, bboard pro) {
    pro &= ~HFILE;
    gen |= pro & (gen >> 9);
    pro &= (pro >>  9);
    gen |= pro & (gen >> 18);
    pro &= (pro >> 18);
    gen |= pro & (gen >> 36);
    return gen;
}

bboard noWeOccl(bboard gen, bboard pro) {
    pro &= ~HFILE;
    gen |= pro & (gen << 7);
    pro &= (pro <<  7);
    gen |= pro & (gen << 14);
    pro &= (pro << 14);
    gen |= pro & (gen << 28);
    return gen;
}


