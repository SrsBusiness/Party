#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>

// File Masks
#define AFILE       0x0101010101010101
#define BFILE       0x0202020202020202
#define CFILE       0x0404040404040404
#define DFILE       0x0808080808080808
#define EFILE       0x1010101010101010
#define FFILE       0x2020202020202020
#define GFILE       0x4040404040404040
#define HFILE       0x8080808080808080

// Rank Masks
#define RANK1       0x00000000000000FF
#define RANK2       0x000000000000FF00
#define RANK3       0x0000000000FF0000
#define RANK4       0x00000000FF000000
#define RANK5       0x000000FF00000000
#define RANK6       0x0000FF0000000000
#define RANK7       0x00FF000000000000
#define RANK8       0xFF00000000000000

// Diagonals from bottom left to top right
#define A7B8        0x0201000000000000
#define A6C8        0x0402010000000000
#define A5D8        0x0804020100000000
#define A4E8        0x1008040201000000
#define A3F8        0x2010080402010000
#define A2G8        0x4020100804020100
#define A1H8        0x8040201008040201
#define B1H7        0x0080402010080402
#define C1H6        0x0000804020100804
#define D1H5        0x0000008040201008
#define E1H4        0x0000000080402010
#define F1H3        0x0000000000804020
#define G1H2        0x0000000000008040

// Diagonals from top left to bottom right
#define A2B1        0x0000000000000102
#define A3C1        0x0000000000010204
#define A4D1        0x0000000001020408
#define A5E1        0x0000000102040810
#define A6F1        0x0000010204081020
#define A7G1        0x0001020408102040
#define A8H1        0x0102040810204080
#define B8H2        0x0204081020408000
#define C8H3        0x0408102040800000
#define D8H4        0x0810204080000000
#define E8H5        0x1020408000000000
#define F8H6        0x2040800000000000
#define G8H7        0x4080000000000000

// Light/Dark square masks
#define LIGHT       0x55AA55AA55AA55AA
#define DARK        0xAA55AA55AA55AA55

// Starting Squares of pieces
#define KING        0x1000000000000010
#define QUEEN       0x0800000000000008
#define BISHOP      0x2400000000000024
#define KNIGHT      0x4200000000000042
#define ROOK        0x8100000000000081
#define PAWN        0x00FF00000000FF00
#define WHITE       0x000000000000FFFF
#define BLACK       0xFFFF000000000000


// macro functions on bitboards

// bitscan
#define bit_scan(board) (__builtin_ctzll(board))
//#define bit_scan(board) (__builtin_ffsll(board))

// flips the bytes of the word
#define flipv(board)    (__builtin_bswap64(board))

// returns the word rotated, hopefully compiler optimized
#define rolll(x, y) (((x) << (y)) | ((uint64_t)(x) >> (8 * sizeof(uint64_t) - (y))))
#define rorll(x, y) (((uint64_t)(x) >> (y)) | ((x) << (8 * sizeof(uint64_t) - (y))))

// directional shifts by one square
// kings/pawns
#define north_one(x)    ((x) << 8)
#define south_one(x)    ((x) >> 8)
#define east_one(x) (((x) << 1) & ~AFILE)
#define west_one(x) (((x) >> 1) & ~HFILE)
#define ne_one(x)   (((x) << 9) & ~AFILE)
#define nw_one(x)   (((x) << 7) & ~HFILE)
#define se_one(x)   (((x) >> 7) & ~AFILE)
#define sw_one(x)   (((x) >> 9) & ~HFILE)


// knights


#define bishop_attacks(x) (noEaOccl(x) | soEaOccl(x) | noWeOccl(x) | soWeOccl(x))
#define rook_attacks(x) (nortOccl(x) | eastOccl(x) | soutOccl(x) | westOccl(x))
#define queen_attacks(x) (bishop_attacks(x) | rook_attacks(x))



typedef uint64_t bboard;
// square of passed pawns

extern const bboard square_pp[64];

// for black's pov, just vertical flip the index, index into the table, then
// vertical flip the bitboard.


#define fliph(x)        fliph0(x)
bboard flipld(bboard);
bboard flipdd(bboard);
bboard rotate_pi(bboard);
bboard rotate_clockwise(bboard);
bboard rotate_anticlockwise(bboard);

bboard north_span(bboard);
bboard south_span(bboard);

#define w_front_span(x) (north_span(x))
#define w_rear_span(x)  (south_span(x))
#define b_front_span(x) (south_span(x))
#define b_rear_span(x)  (north_span(x))

bboard north_fill(bboard);
bboard south_fill(bboard);

#define w_front_fill(x) (north_fill(x))
#define w_rear_fill(x)  (south_fill(x))
#define b_front_fill(x) (south_fill(x))
#define b_rear_fill(x)  (north_fill(x))

#define file_fill(x)    (north_fill(x) | south_fill(x))
bboard soutOccl(bboard, bboard);
bboard nortOccl(bboard, bboard);
bboard eastOccl(bboard, bboard);
bboard noEaOccl(bboard, bboard);
bboard soEaOccl(bboard, bboard);
bboard westOccl(bboard, bboard);
bboard soWeOccl(bboard, bboard);
bboard noWeOccl(bboard, bboard);

// iterate through all pieces in a bboard
// bboard b;
// while(b){
//     int index = __builtin_ctz(b);
//     ...
//     b &= b - 1;
//
// }

void display_bboard(bboard, int, int);
void display_text(char *, int, int);

#endif
