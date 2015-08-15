#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "bitboard.h"

uint64_t fliph0 (uint64_t x) {
    const uint64_t k1 = 0x5555555555555555;
    const uint64_t k2 = 0x3333333333333333;
    const uint64_t k4 = 0x0f0f0f0f0f0f0f0f;
    x = ((x >> 1) & k1) | ((x & k1) << 1);
    x = ((x >> 2) & k2) | ((x & k2) << 2);
    x = ((x >> 4) & k4) | ((x & k4) << 4);
    return x;
}

uint64_t fliph1 (uint64_t x) {
    const uint64_t k1 = 0x5555555555555555;
    const uint64_t k2 = 0x3333333333333333;
    const uint64_t k4 = 0x0f0f0f0f0f0f0f0f;
    x = ((x >> 1) & k1) +  2*(x & k1);
    x = ((x >> 2) & k2) +  4*(x & k2);
    x = ((x >> 4) & k4) + 16*(x & k4);
    return x;
}

uint64_t fliph2 (uint64_t x) {
    const uint64_t k1 = 0x5555555555555555;
    const uint64_t k2 = 0x3333333333333333;
    const uint64_t k4 = 0x0f0f0f0f0f0f0f0f;
    x ^= k4 & (x ^ rolll(x, 8));
    x ^= k2 & (x ^ rolll(x, 4));
    x ^= k1 & (x ^ rolll(x, 2));
    return rorll(x, 7);
}

uint64_t flipdd(uint64_t x) { // light square diagonal
    uint64_t t;
    const uint64_t k1 = 0x5500550055005500;
    const uint64_t k2 = 0x3333000033330000;
    const uint64_t k4 = 0x0f0f0f0f00000000;
    t = k4 & (x ^ (x << 28));
    x ^= t ^ (t >> 28);
    t = k2 & (x ^ (x << 14));
    x ^= t ^ (t >> 14);
    t = k1 & (x ^ (x << 7));
    x ^= t ^ (t >> 7);
    return x;
}

uint64_t flipld(uint64_t x) { // dark square diagonal
    uint64_t t;
    const uint64_t k1 = 0xaa00aa00aa00aa00;
    const uint64_t k2 = 0xcccc0000cccc0000;
    const uint64_t k4 = 0xf0f0f0f00f0f0f0f;
    t = x ^ (x << 36);
    x ^= k4 & (t ^ (x >> 36));
    t = k2 & (x ^ (x << 18));
    x ^= t ^ (t >> 18);
    t = k1 & (x ^ (x << 9));
    x ^= t ^ (t >> 9);
    return x;
}

// 180 rotation
uint64_t rotate_pi(uint64_t x){
    // vertical flip
    x = flipv(x);
    // horizontal flip
    const uint64_t k1 = 0x5555555555555555;
    const uint64_t k2 = 0x3333333333333333;
    const uint64_t k4 = 0x0f0f0f0f0f0f0f0f;
    x = ((x >> 1) & k1) +  2*(x & k1);
    x = ((x >> 2) & k2) +  4*(x & k2);
    x = ((x >> 4) & k4) + 16*(x & k4);
    return x;
}

// 90 clockwise
uint64_t rotate_clockwise(uint64_t x){
    // to be optimized with inlining...
    return flipld(flipv(x));
}

// 90 anticlockwise
uint64_t rotate_anticlockwise(uint64_t x){
    // to be optimized with inlining...
    return flipdd(flipv(x));
}

uint64_t north_span(uint64_t x){
    x <<= 8;
    x |= (x << 8);
    x |= (x << 16);
    x |= (x << 32);
    return x;
}

uint64_t south_span(uint64_t x){
    x >>= 8;
    x |= (x >> 8);
    x |= (x >> 16);
    x |= (x >> 32);
    return x;
}

uint64_t east_span(uint64_t x) {
    uint64_t k1 = ~BITBOARD_AFILE;
    uint64_t k2 = k1 & (k1 << 1);
    uint64_t k3 = k2 & (k2 << 2);
    x = (x << 1) & k1;
    x |= (x << 1) & k1;
    x |= (x << 2) & k2;
    x |= (x << 4) & k3;
    return x;
}

uint64_t west_span(uint64_t x) {
    uint64_t k1 = ~BITBOARD_HFILE;
    uint64_t k2 = k1 & (k1 >> 1);
    uint64_t k3 = k2 & (k2 >> 2);
    x = (x >> 1) & k1;
    x |= (x >> 1) & k1;
    x |= (x >> 2) & k2;
    x |= (x >> 4) & k3;
    return x;
}

uint64_t ne_span(uint64_t x) {
    uint64_t k1 = ~BITBOARD_AFILE;
    uint64_t k2 = k1 & (k1 << 9);
    uint64_t k3 = k2 & (k2 << 18);
    x = (x << 9) & k1;
    x |= (x << 9) & k1;
    x |= (x << 18) & k2;
    x |= (x << 36) & k3;
    return x;
}

uint64_t nw_span(uint64_t x) {
    uint64_t k1 = ~BITBOARD_HFILE;
    uint64_t k2 = k1 & (k1 << 7);
    uint64_t k3 = k2 & (k2 << 14);
    x = (x << 7) & k1;
    x |= (x << 7) & k1;
    x |= (x << 14) & k2;
    x |= (x << 28) & k3;
    return x;
}

uint64_t se_span(uint64_t x) {
    uint64_t k1 = ~BITBOARD_AFILE;
    uint64_t k2 = k1 & (k1 >> 7);
    uint64_t k3 = k2 & (k2 >> 14);
    x = (x >> 7) & k1;
    x |= (x >> 7) & k1;
    x |= (x >> 14) & k2;
    x |= (x >> 28) & k3;
    return x;
}

uint64_t sw_span(uint64_t x) {
    uint64_t k1 = ~BITBOARD_HFILE;
    uint64_t k2 = k1 & (k1 >> 9);
    uint64_t k3 = k2 & (k2 >> 18);
    x = (x >> 9) & k1;
    x |= (x >> 9) & k1;
    x |= (x >> 18) & k2; 
    x |= (x >> 36) & k3;
    return x;
}

uint64_t north_fill(uint64_t gen) {
   gen |= (gen <<  8);
   gen |= (gen << 16);
   gen |= (gen << 32);
   return gen;
}
 
uint64_t south_fill(uint64_t gen) {
   gen |= (gen >>  8);
   gen |= (gen >> 16);
   gen |= (gen >> 32);
   return gen;
}

uint64_t east_fill(uint64_t x) {
    uint64_t k1 = ~BITBOARD_AFILE;
    uint64_t k2 = k1 & (k1 << 1);
    uint64_t k3 = k2 & (k2 << 2);
    x |= (x << 1) & k1;
    x |= (x << 2) & k2;
    x |= (x << 4) & k3;
    return x;
}

uint64_t west_fill(uint64_t x) {
    uint64_t k1 = ~BITBOARD_HFILE;
    uint64_t k2 = k1 & (k1 >> 1);
    uint64_t k3 = k2 & (k2 >> 2);
    x |= (x >> 1) & k1;
    x |= (x >> 2) & k2;
    x |= (x >> 4) & k3;
    return x;
}

uint64_t ne_fill(uint64_t x) {
    uint64_t k1 = ~BITBOARD_AFILE;
    uint64_t k2 = k1 & (k1 << 9);
    uint64_t k3 = k2 & (k2 << 18);
    x |= (x << 9) & k1;
    x |= (x << 18) & k2;
    x |= (x << 36) & k3;
    return x;
}

uint64_t nw_fill(uint64_t x) {
    uint64_t k1 = ~BITBOARD_HFILE;
    uint64_t k2 = k1 & (k1 << 7);
    uint64_t k3 = k2 & (k2 << 14);
    x |= (x << 7) & k1;
    x |= (x << 14) & k2;
    x |= (x << 28) & k3;
    return x;
}

uint64_t se_fill(uint64_t x) {
    uint64_t k1 = ~BITBOARD_AFILE;
    uint64_t k2 = k1 & (k1 >> 7);
    uint64_t k3 = k2 & (k2 >> 14);
    x |= (x >> 7) & k1;
    x |= (x >> 14) & k2;
    x |= (x >> 28) & k3;
    return x;
}

uint64_t sw_fill(uint64_t x) {
    uint64_t k1 = ~BITBOARD_HFILE;
    uint64_t k2 = k1 & (k1 >> 9);
    uint64_t k3 = k2 & (k2 >> 18);
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

uint64_t catchable_passers_white(uint64_t king){
    return 0;
}

uint64_t catchable_passers_black(uint64_t king){
    return 0;
}



//int knight_distance_occl(uint64_t b1, uint64_t b2, uint64_t occupied){
//    int d = 0;
//    while ((b1 & b2) == 0) {
//        b1 = knight_attacks(b1) & ~occupied; // as long as sets are disjoint
//        d++; // increment distance
//    }
//    return d;
//}

//int knightDistance(uint64_t source, uint64_t dest) {
//    return calcKnightDistance(source, dest);
//}

/*
 * gen refers to the generation set i.e. set of pieces generating the fill
 * pro refers to the propgation set i.e. set of empty squares on the board 
 */

uint64_t south_occluded_fill(uint64_t gen, uint64_t pro) {
    gen |= pro & (gen >> 8);
    pro &= (pro >>  8);
    gen |= pro & (gen >> 16);
    pro &= (pro >> 16);
    gen |= pro & (gen >> 32);
    return gen;
}

uint64_t north_occluded_fill(uint64_t gen, uint64_t pro) {
    gen |= pro & (gen << 8);
    pro &= (pro <<  8);
    gen |= pro & (gen << 16);
    pro &= (pro << 16);
    gen |= pro & (gen << 32);
    return gen;
}

uint64_t east_occluded_fill(uint64_t gen, uint64_t pro) {
    pro &= ~BITBOARD_AFILE;
    gen |= pro & (gen << 1);
    pro &= (pro << 1);
    gen |= pro & (gen << 2);
    pro &= (pro << 2);
    gen |= pro & (gen << 4);
    return gen;
}

uint64_t ne_occluded_fill(uint64_t gen, uint64_t pro) {
    pro &= ~BITBOARD_AFILE;
    gen |= pro & (gen << 9);
    pro &= (pro <<  9);
    gen |= pro & (gen << 18);
    pro &= (pro << 18);
    gen |= pro & (gen << 36);
    return gen;
}

uint64_t se_occluded_fill(uint64_t gen, uint64_t pro) {
    pro &= ~BITBOARD_AFILE;
    gen |= pro & (gen >> 7);
    pro &= (pro >>  7);
    gen |= pro & (gen >> 14);
    pro &= (pro >> 14);
    gen |= pro & (gen >> 28);
    return gen;
}

uint64_t west_occluded_fill(uint64_t gen, uint64_t pro) {
    pro &= ~BITBOARD_HFILE;
    gen |= pro & (gen >> 1);
    pro &= (pro >> 1);
    gen |= pro & (gen >> 2);
    pro &= (pro >> 2);
    gen |= pro & (gen >> 4);
    return gen;
}

uint64_t sw_occluded_fill(uint64_t gen, uint64_t pro) {
    pro &= ~BITBOARD_HFILE;
    gen |= pro & (gen >> 9);
    pro &= (pro >>  9);
    gen |= pro & (gen >> 18);
    pro &= (pro >> 18);
    gen |= pro & (gen >> 36);
    return gen;
}

uint64_t nw_occluded_fill(uint64_t gen, uint64_t pro) {
    pro &= ~BITBOARD_HFILE;
    gen |= pro & (gen << 7);
    pro &= (pro <<  7);
    gen |= pro & (gen << 14);
    pro &= (pro << 14);
    gen |= pro & (gen << 28);
    return gen;
}
