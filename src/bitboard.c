#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "bitboard.h"

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
    x |= (x << 8);
    x |= (x << 16);
    x |= (x << 32);
    return x;
}

bboard south_span(bboard x){
    x >>= 8;
    x |= (x >> 8);
    x |= (x >> 16);
    x |= (x >> 32);
    return x;
}

bboard east_span(bboard x) {
    bboard k1 = ~BITBOARD_AFILE;
    bboard k2 = k1 & (k1 << 1);
    bboard k3 = k2 & (k2 << 2);
    x = (x << 1) & k1;
    x |= (x << 1) & k1;
    x |= (x << 2) & k2;
    x |= (x << 4) & k3;
    return x;
}

bboard west_span(bboard x) {
    bboard k1 = ~BITBOARD_HFILE;
    bboard k2 = k1 & (k1 >> 1);
    bboard k3 = k2 & (k2 >> 2);
    x = (x >> 1) & k1;
    x |= (x >> 1) & k1;
    x |= (x >> 2) & k2;
    x |= (x >> 4) & k3;
    return x;
}

bboard ne_span(bboard x) {
    bboard k1 = ~BITBOARD_AFILE;
    bboard k2 = k1 & (k1 << 9);
    bboard k3 = k2 & (k2 << 18);
    x = (x << 9) & k1;
    x |= (x << 9) & k1;
    x |= (x << 18) & k2;
    x |= (x << 36) & k3;
    return x;
}

bboard nw_span(bboard x) {
    bboard k1 = ~BITBOARD_HFILE;
    bboard k2 = k1 & (k1 << 7);
    bboard k3 = k2 & (k2 << 14);
    x = (x << 7) & k1;
    x |= (x << 7) & k1;
    x |= (x << 14) & k2;
    x |= (x << 28) & k3;
    return x;
}

bboard se_span(bboard x) {
    bboard k1 = ~BITBOARD_AFILE;
    bboard k2 = k1 & (k1 >> 7);
    bboard k3 = k2 & (k2 >> 14);
    x = (x >> 7) & k1;
    x |= (x >> 7) & k1;
    x |= (x >> 14) & k2;
    x |= (x >> 28) & k3;
    return x;
}

bboard sw_span(bboard x) {
    bboard k1 = ~BITBOARD_HFILE;
    bboard k2 = k1 & (k1 >> 9);
    bboard k3 = k2 & (k2 >> 18);
    x = (x >> 9) & k1;
    x |= (x >> 9) & k1;
    x |= (x >> 18) & k2; 
    x |= (x >> 36) & k3;
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

bboard east_fill(bboard x) {
    bboard k1 = ~BITBOARD_AFILE;
    bboard k2 = k1 & (k1 << 1);
    bboard k3 = k2 & (k2 << 2);
    x |= (x << 1) & k1;
    x |= (x << 2) & k2;
    x |= (x << 4) & k3;
    return x;
}

bboard west_fill(bboard x) {
    bboard k1 = ~BITBOARD_HFILE;
    bboard k2 = k1 & (k1 >> 1);
    bboard k3 = k2 & (k2 >> 2);
    x |= (x >> 1) & k1;
    x |= (x >> 2) & k2;
    x |= (x >> 4) & k3;
    return x;
}

bboard ne_fill(bboard x) {
    bboard k1 = ~BITBOARD_AFILE;
    bboard k2 = k1 & (k1 << 9);
    bboard k3 = k2 & (k2 << 18);
    x |= (x << 9) & k1;
    x |= (x << 18) & k2;
    x |= (x << 36) & k3;
    return x;
}

bboard nw_fill(bboard x) {
    bboard k1 = ~BITBOARD_HFILE;
    bboard k2 = k1 & (k1 << 7);
    bboard k3 = k2 & (k2 << 14);
    x |= (x << 7) & k1;
    x |= (x << 14) & k2;
    x |= (x << 28) & k3;
    return x;
}

bboard se_fill(bboard x) {
    bboard k1 = ~BITBOARD_AFILE;
    bboard k2 = k1 & (k1 >> 7);
    bboard k3 = k2 & (k2 >> 14);
    x |= (x >> 7) & k1;
    x |= (x >> 14) & k2;
    x |= (x >> 28) & k3;
    return x;
}

bboard sw_fill(bboard x) {
    bboard k1 = ~BITBOARD_HFILE;
    bboard k2 = k1 & (k1 >> 9);
    bboard k3 = k2 & (k2 >> 18);
    x |= (x >> 9) & k1;
    x |= (x >> 18) & k2; 
    x |= (x >> 36) & k3;
    return x;
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

bboard south_occluded_fill(bboard gen, bboard pro) {
    gen |= pro & (gen >> 8);
    pro &= (pro >>  8);
    gen |= pro & (gen >> 16);
    pro &= (pro >> 16);
    gen |= pro & (gen >> 32);
    return gen;
}

bboard north_occluded_fill(bboard gen, bboard pro) {
    gen |= pro & (gen << 8);
    pro &= (pro <<  8);
    gen |= pro & (gen << 16);
    pro &= (pro << 16);
    gen |= pro & (gen << 32);
    return gen;
}

bboard east_occluded_fill(bboard gen, bboard pro) {
    pro &= ~BITBOARD_AFILE;
    gen |= pro & (gen << 1);
    pro &= (pro << 1);
    gen |= pro & (gen << 2);
    pro &= (pro << 2);
    gen |= pro & (gen << 4);
    return gen;
}

bboard ne_occluded_fill(bboard gen, bboard pro) {
    pro &= ~BITBOARD_AFILE;
    gen |= pro & (gen << 9);
    pro &= (pro <<  9);
    gen |= pro & (gen << 18);
    pro &= (pro << 18);
    gen |= pro & (gen << 36);
    return gen;
}

bboard se_occluded_fill(bboard gen, bboard pro) {
    pro &= ~BITBOARD_AFILE;
    gen |= pro & (gen >> 7);
    pro &= (pro >>  7);
    gen |= pro & (gen >> 14);
    pro &= (pro >> 14);
    gen |= pro & (gen >> 28);
    return gen;
}

bboard west_occluded_fill(bboard gen, bboard pro) {
    pro &= ~BITBOARD_HFILE;
    gen |= pro & (gen >> 1);
    pro &= (pro >> 1);
    gen |= pro & (gen >> 2);
    pro &= (pro >> 2);
    gen |= pro & (gen >> 4);
    return gen;
}

bboard sw_occluded_fill(bboard gen, bboard pro) {
    pro &= ~BITBOARD_HFILE;
    gen |= pro & (gen >> 9);
    pro &= (pro >>  9);
    gen |= pro & (gen >> 18);
    pro &= (pro >> 18);
    gen |= pro & (gen >> 36);
    return gen;
}

bboard nw_occluded_fill(bboard gen, bboard pro) {
    pro &= ~BITBOARD_HFILE;
    gen |= pro & (gen << 7);
    pro &= (pro <<  7);
    gen |= pro & (gen << 14);
    pro &= (pro << 14);
    gen |= pro & (gen << 28);
    return gen;
}
